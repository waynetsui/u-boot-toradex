/*
 * (C) Copyright 2008
 * Logic Produc Development, <www.logicpd.com>
 * Peter Barada <peter.barada@logicpd.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/arch/cpu.h>
#include <asm/io.h>
#include <asm/arch/bits.h>
#include <asm/arch/mux.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/sys_info.h>
#include <asm/arch/clocks.h>
#include <asm/arch/mem.h>
#include <i2c.h>
#include <asm/mach-types.h>

// GPIO i2c code to access at88 chip

enum {
	RX_MODE_FIRST_BYTE,
	RX_MODE_MIDDLE_BYTE,
	RX_MODE_NEXT_TO_LAST_BYTE,
	RX_MODE_LAST_BYTE,
	RX_MODE_ONE_BYTE
} I2C_RX_MODE;

typedef enum {
  GPIO_I2C_SDATA,
  GPIO_I2C_SCLK,
} GPIO_I2C_PIN;

static enum {
  GPIO_I2C_UNINIT,
  GPIO_I2C_STOPPED,
  GPIO_I2C_STARTED,
} gpio_i2c_bus_state;

typedef enum {
  GPIO_I2C_INPUT,
  GPIO_I2C_OUTPUT,
} GPIO_I2C_DIRECTION;

static int gpio_i2c_clock_high_width, gpio_i2c_clock_low_width;
static int gpio_i2c_coarse_delay;

#define DEBUG_PRODUCTION_DATA 0
#define DEBUG_PRODUCTION_DATA_BUF 0
#define STATIC static


#define 	MUX_VAL(OFFSET,VALUE)\
		__raw_writew((VALUE), OMAP34XX_CTRL_BASE + (OFFSET))

#define		CP(x)	(CONTROL_PADCONF_##x)
/*
 * IEN  - Input Enable
 * IDIS - Input Disable
 * PTD  - Pull type Down
 * PTU  - Pull type Up
 * DIS  - Pull type selection is inactive
 * EN   - Pull type selection is active
 * M0   - Mode 0
 * The commented string gives the final mux configuration for that pin
 */

// Put SCLK/SDA pins connected to the product ID into GPIO mode
STATIC void gpio_i2c_config_pins(void)
{
	MUX_VAL(CP(I2C3_SCL),       (IEN  | PTU | EN  | M4)); /*I2C3_SCL*/
	MUX_VAL(CP(I2C3_SDA),       (IEN  | PTU | EN  | M4)); /*I2C3_SDA*/
}

// Restore SCLK/SDA pins connected to the product ID back to I2C mode

STATIC void gpio_i2c_restore_pins(void)
{
	MUX_VAL(CP(I2C3_SCL),       (IEN  | PTU | EN  | M0)); /*I2C3_SCL*/
	MUX_VAL(CP(I2C3_SDA),       (IEN  | PTU | EN  | M0)); /*I2C3_SDA*/
}

#define GPIO_I2C_GPIO_SCLK  184
#define GPIO_I2C_GPIO_SDATA 185

STATIC void gpio_i2c_config_pin(GPIO_I2C_PIN pin, GPIO_I2C_DIRECTION dir)
{
  if (dir == GPIO_I2C_INPUT) {
    if (pin == GPIO_I2C_SCLK)
      pin_init_gpio(GPIO_I2C_GPIO_SCLK, 1);
    else
      pin_init_gpio(GPIO_I2C_GPIO_SDATA, 1);
  } else if (dir == GPIO_I2C_OUTPUT) {
    if (pin == GPIO_I2C_SCLK)
      pin_init_gpio(GPIO_I2C_GPIO_SCLK, 0);
    else
      pin_init_gpio(GPIO_I2C_GPIO_SDATA, 0);
  }
}

STATIC int gpio_i2c_read_pin(GPIO_I2C_PIN pin)
{
  if (pin == GPIO_I2C_SCLK)
    return pin_get_gpio_input(GPIO_I2C_GPIO_SCLK);
  else
    return pin_get_gpio_input(GPIO_I2C_GPIO_SDATA);
  return 0;
}

STATIC void gpio_i2c_set_pin_level(GPIO_I2C_PIN pin, int level)
{
  uint8_t pin_level;

  if (pin == GPIO_I2C_SCLK) {
    pin_level = pin_get_gpio_input(GPIO_I2C_GPIO_SCLK);
    if (((level == 1) && (pin_level == 0)) ||
	((level == 0) && (pin_level == 1)))
      pin_set_gpio_dataout(GPIO_I2C_GPIO_SCLK, level);
  } else if (pin == GPIO_I2C_SDATA) {
    if (level == 0) {
      gpio_i2c_config_pin(pin, GPIO_I2C_OUTPUT);
      pin_set_gpio_dataout(GPIO_I2C_GPIO_SDATA, 0);
    } else {
      gpio_i2c_config_pin(pin, GPIO_I2C_INPUT);
    }
  }
}


STATIC void gpio_i2c_init(int bps)
{
  gpio_i2c_bus_state = GPIO_I2C_UNINIT;

  // Config SCLK, SDATA pins
  gpio_i2c_config_pin(GPIO_I2C_SCLK, GPIO_I2C_OUTPUT);
  gpio_i2c_set_pin_level(GPIO_I2C_SCLK, 1);

  gpio_i2c_config_pin(GPIO_I2C_SDATA, GPIO_I2C_INPUT);

  gpio_i2c_config_pins();

  // Assume 1:1 clock duty cycle
  gpio_i2c_clock_high_width = gpio_i2c_clock_low_width
    = 1000000 / bps / 2;

  gpio_i2c_coarse_delay = gpio_i2c_clock_high_width;
}

STATIC int gpio_i2c_busy(void)
{
  return (gpio_i2c_bus_state == GPIO_I2C_STARTED);
}

STATIC void gpio_i2c_tx_stop(void)
{
  if (gpio_i2c_bus_state == GPIO_I2C_STARTED) {
    udelay(gpio_i2c_coarse_delay);

    // Pull SDATA low
    gpio_i2c_set_pin_level(GPIO_I2C_SDATA, 0);
    udelay(gpio_i2c_coarse_delay);

    // Push SCLK high
    gpio_i2c_set_pin_level(GPIO_I2C_SCLK, 1);
    udelay(gpio_i2c_coarse_delay);

    // Now drive SDATA high - thats a STOP.
    gpio_i2c_set_pin_level(GPIO_I2C_SDATA, 1);
    udelay(gpio_i2c_coarse_delay);
    gpio_i2c_bus_state = GPIO_I2C_STOPPED;
  }
  
}

STATIC void gpio_i2c_tx_start(void)
{
  if (gpio_i2c_bus_state == GPIO_I2C_UNINIT || gpio_i2c_bus_state == GPIO_I2C_STOPPED) {
    udelay(gpio_i2c_coarse_delay);
    gpio_i2c_set_pin_level(GPIO_I2C_SDATA, 1);
    udelay(gpio_i2c_coarse_delay);
    gpio_i2c_set_pin_level(GPIO_I2C_SCLK, 1);
    udelay(gpio_i2c_coarse_delay);
    gpio_i2c_set_pin_level(GPIO_I2C_SDATA, 0);
    udelay(gpio_i2c_coarse_delay);
    gpio_i2c_set_pin_level(GPIO_I2C_SCLK, 0);
    udelay(gpio_i2c_coarse_delay);
    gpio_i2c_bus_state = GPIO_I2C_STARTED;
  }
}

// Return !0 if NACK
STATIC int gpio_i2c_tx_byte(uint8_t data)
{
  uint8_t clock, tx_bit_mask=0x80, nack;

  if (gpio_i2c_bus_state != GPIO_I2C_STARTED)
    printf("%s: Unexpected I2C bus state!\n", __FUNCTION__);

  udelay(gpio_i2c_coarse_delay);

  for (clock=0; clock <= 7; ++clock) {
    if (data & tx_bit_mask)
      gpio_i2c_set_pin_level(GPIO_I2C_SDATA, 1);
    else
      gpio_i2c_set_pin_level(GPIO_I2C_SDATA, 0);
    udelay(gpio_i2c_clock_low_width);
    gpio_i2c_set_pin_level(GPIO_I2C_SCLK, 1);
    udelay(gpio_i2c_clock_high_width);
    gpio_i2c_set_pin_level(GPIO_I2C_SCLK, 0);
    tx_bit_mask >>= 1;
  }
  gpio_i2c_config_pin(GPIO_I2C_SDATA, GPIO_I2C_INPUT);
  udelay(gpio_i2c_clock_low_width);
  gpio_i2c_set_pin_level(GPIO_I2C_SCLK, 1);
  udelay(gpio_i2c_clock_high_width);
  nack = gpio_i2c_read_pin(GPIO_I2C_SDATA);
  gpio_i2c_set_pin_level(GPIO_I2C_SCLK, 0);
  return (nack != 0);
}

STATIC int gpio_i2c_rx_byte(uint8_t *data, int rx_mode)
{
  uint8_t clock, data_bit;

  *data = 0;

  gpio_i2c_config_pin(GPIO_I2C_SDATA, GPIO_I2C_INPUT);

  udelay(gpio_i2c_coarse_delay);

  for (clock=0; clock<=8; ++clock) {
    if (clock < 8) {
      gpio_i2c_set_pin_level(GPIO_I2C_SCLK, 1);
      udelay(gpio_i2c_clock_high_width);
      data_bit = gpio_i2c_read_pin(GPIO_I2C_SDATA);
      gpio_i2c_set_pin_level(GPIO_I2C_SCLK, 0);
      *data = (*data << 1) | data_bit;
    } else {
      if ((rx_mode == RX_MODE_LAST_BYTE) || (rx_mode == RX_MODE_ONE_BYTE))
	gpio_i2c_set_pin_level(GPIO_I2C_SDATA, 1);
      else
	gpio_i2c_set_pin_level(GPIO_I2C_SDATA, 0);

      gpio_i2c_set_pin_level(GPIO_I2C_SCLK, 1);
      udelay(gpio_i2c_clock_high_width);
      gpio_i2c_set_pin_level(GPIO_I2C_SCLK, 0);
    }
    udelay(gpio_i2c_clock_low_width);
  }

  return 0;
}


STATIC int send_packet(uint8_t *data, int len, uint8_t *rxbuf, int rxlen)
{
	int timeout = 1000;
	int retry;
	int rx_mode;
	int tick, err, idx;

	if (DEBUG_PRODUCTION_DATA) {
	  char buf[128];
	  int i, offset;
	  for (offset = 0, i=0; i<len; ++i) {
	    if (!i)
	      offset = sprintf(buf, "%02x", data[i]);
	    else
	      offset += sprintf(&buf[offset], " %02x", data[i]);
	  }
	  printf("%s: %s\n", __FUNCTION__, buf);
	}

	// Wait for bus
	while (gpio_i2c_busy() && timeout--)
		udelay(100);

	if (!timeout)
	  printf("%s:%d i2c_busy never return zero!\n", __FUNCTION__, __LINE__);

	retry = 0;
	do {
		tick = 0;
		do {
			gpio_i2c_tx_stop();
			gpio_i2c_tx_start();

			// send cmd
			err = gpio_i2c_tx_byte(data[0]);
			tick++;
		} while (err && tick < 100);

		if (tick > 3)
			printf("I2C ACK polling tick %d!\n", tick);

		for (idx = 1; idx<len; ++idx) {
			err = gpio_i2c_tx_byte(data[idx]);
			if (err) {
				printf("%s:%d NACK idx %d\n", __FUNCTION__, __LINE__, idx);
			}
		}
	} while (err && (retry++ < 5));

	if (err)
	  return err;

	// Are we expecting a response?
	if (rxbuf) {
	  for (idx = 0; idx < rxlen; ++idx) {
	    if (rxlen == 1)
	      rx_mode = RX_MODE_ONE_BYTE;
	    else if (idx == (rxlen - 1))
	      rx_mode = RX_MODE_LAST_BYTE;
	    else if (idx == (rxlen - 2))
	      rx_mode = RX_MODE_NEXT_TO_LAST_BYTE;
	    else if (idx == 0)
	      rx_mode = RX_MODE_FIRST_BYTE;
	    else
	      rx_mode = RX_MODE_MIDDLE_BYTE;

	    err = gpio_i2c_rx_byte(&rxbuf[idx], rx_mode);
	    if (DEBUG_PRODUCTION_DATA) {
	      if (err)
		printf("%s:%d err idx %d\n", __FUNCTION__, __LINE__, idx);
	    }
	    // tx_ack();
	  }
	}

	gpio_i2c_tx_stop();
	return err;
}

#define CMD_SYSTEM_WRITE    0xB4

STATIC int
set_user_zone(int zone)
{
  unsigned char cmd[] = { CMD_SYSTEM_WRITE, 0x03, 0x00, 0x00 };
  if (DEBUG_PRODUCTION_DATA)
    printf("%s: zone %d\n", __FUNCTION__, zone);
  cmd[2] = zone;
  return send_packet(cmd, sizeof(cmd), NULL, 0);
}

#define CMD_READ_USER_ZONE  0xB2

STATIC int
read_user_zone(unsigned char *buf, int len, int startoff)
{
  unsigned char cmd[] = { CMD_READ_USER_ZONE, 0x00, 0x00, 0x00 };
  int ret;
  cmd[2] = startoff;
  cmd[3] = len;
  ret = send_packet(cmd, sizeof(cmd), buf, len);

  if (DEBUG_PRODUCTION_DATA_BUF) {
    char obuf[128];
    int i,j,offset;
    for (i = 0, offset=0; i<len; i+=16) {
      for (j = 0; j<16 && i+j<len; ++j)
	if (!j)
	  offset = sprintf(obuf, "%02x", buf[i+j]);
	else
	  offset += sprintf(&obuf[offset], " %02x", buf[i+j]);
      printf("%s\n", obuf);
    }
  }
  return ret;
}


#define LOGIC_HEADER_VERSION 0
struct {
  char header_version;
  char part_number[11];
  char revision;
  char sn_week;
  char sn_year;
  char sn_site;
  int sn_cnt;
  char maturity;
} product_zone_0;

struct {
  char model_revision;
  char model_number[31];
} product_zone_1;

struct {
  unsigned char mac[4][3];
  char nor0_size;
  char nor1_size;
  char nand0_size;
  char nand1_size;
  char sdram0_size;
  char sdram1_size;
  short processor_type;
  int features;
  int platform_bits;
} product_zone_2;

#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)
int production_data_valid;

void board_get_nth_enetaddr (unsigned char *enetaddr, int which)
{
	unsigned char *mac = &product_zone_2.mac[which][0];
	char *s, *e;
	int i;
	char ethbuf[18];
	// If the data is bogus, or the MAC address is 00:00:00 or ff:ff:ff
	// then blow zeros into the address.

	if (!production_data_valid ||
	    (mac[0] == 0xff && mac[1] == 0xff && mac[2] == 0xff)
	    || (mac[0] == 0x00 && mac[1] == 0x00 && mac[2] == 0x00)) {
#ifdef CONFIG_ETHADDR
		if (which == 0) {
			s = MK_STR(CONFIG_ETHADDR);
			for (i = 0; i < 6; ++i) {
				enetaddr[i] = s ? simple_strtoul (s, &e, 16) : 0;
				if (s)
					s = (*e) ? e + 1 : e;
			}
		} else
#endif
			memset(enetaddr, '\0', 6);
		goto exit;
	}

#if 0
	printf("%s:%d which %d %x:%x:%x\n", __FUNCTION__, __LINE__, which,
	       mac[0], mac[1], mac[2]);
#endif

	// Use Logic's prefix
	enetaddr[0] = 0x00;
	enetaddr[1] = 0x08;
	enetaddr[2] = 0xee;
	enetaddr[3] = mac[0];
	enetaddr[4] = mac[1];
	enetaddr[5] = mac[2];

 exit:
	if (which == 0) {
		sprintf(ethbuf, "%02x:%02x:%02x:%02x:%02x:%02x", enetaddr[0], enetaddr[1], enetaddr[2], enetaddr[3], enetaddr[4], enetaddr[5]);
		setenv("ethaddr", ethbuf);
	} else {
		char buf[32];
		sprintf(ethbuf, "%02x:%02x:%02x:%02x:%02x:%02x", enetaddr[0], enetaddr[1], enetaddr[2], enetaddr[3], enetaddr[4], enetaddr[5]);
		sprintf(buf, "eth%daddr", which);
		setenv(buf, ethbuf);
	}
}

#undef CONFIG_FETCH_ONLY_MAC_ADDRESSES

int fetch_production_data(void)
{
  int err = 0;
  char buf[12];

  gpio_i2c_init(50000);

#ifdef CONFIG_FETCH_ONLY_MAC_ADDRESSES
  printf("Read default MAC addresses: ");
  if (set_user_zone(2) || read_user_zone((unsigned char *)&product_zone_2.mac, sizeof(product_zone_2.mac), 0)) {
    printf("failed!\n");
    err = -4;
    goto out;
  }
#else
  printf("Read production data: ");
  if (set_user_zone(0) || read_user_zone((unsigned char *)&product_zone_0, sizeof(product_zone_0), 0)) {
    printf("failed!\n");
    err = -1;
    goto out;
  }

  // If the header doesn't match, we can't map any of the data
  if (product_zone_0.header_version != LOGIC_HEADER_VERSION) {
    err = -2;
    goto out;
  }

  if (set_user_zone(1) || read_user_zone((unsigned char *)&product_zone_1, sizeof(product_zone_1), 0)) {
    printf("failed!\n");
    err = -3;
    goto out;
  }

  if (set_user_zone(2) || read_user_zone((unsigned char *)&product_zone_2, sizeof(product_zone_2), 0)) {
    printf("failed!\n");
    err = -4;
    goto out;
  }
#endif
  printf("done\n");

#ifndef CONFIG_FETCH_ONLY_MAC_ADDRESSES
  // Correct endianess issues
  product_zone_2.processor_type = le16_to_cpu(product_zone_2.processor_type);
  product_zone_2.features = le32_to_cpu(product_zone_2.features);
  product_zone_2.platform_bits = le32_to_cpu(product_zone_2.platform_bits);

    // Print out the name, model number, and set MAC addresses
    strncpy(buf, product_zone_0.part_number, sizeof(product_zone_0.part_number));
    buf[sizeof(product_zone_0.part_number)] = '\0';
    printf("Part Number: %s\n", buf);
    printf("Model Name : %s\n", product_zone_1.model_number);
    // printf("Maturity   : %d\n", product_zone_0.maturity);
    printf("Model Rev  : %c\n", product_zone_1.model_revision);
    printf("Serial #   : %02d%02d%c%05d\n", product_zone_0.sn_week, product_zone_0.sn_year, product_zone_0.sn_site, product_zone_0.sn_cnt);
#endif

  out:
    production_data_valid = !err;

    // Restore pins back to their intended use
    gpio_i2c_restore_pins();

    return err;
}

