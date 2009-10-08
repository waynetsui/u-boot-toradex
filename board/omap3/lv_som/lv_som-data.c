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
// #include <asm/arch/bits.h>
#include <asm/arch/mux.h>
#include <asm/arch/sys_proto.h>
// #include <asm/arch/sys_info.h>
#include <asm/arch/clocks.h>
#include <asm/arch/mem.h>
#include <i2c.h>
#include <asm/mach-types.h>
#include "product_id.h"
#include "lv_som-gpio.h"

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

#if 0
#define 	MUX_VAL(OFFSET,VALUE)					\
 		__raw_writew((VALUE), OMAP34XX_CTRL_BASE + (OFFSET))
#define		CP(x)	(CONTROL_PADCONF_##x)
#endif

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

#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)
int production_data_valid;

struct product_id_data product_id_data;

void board_get_nth_enetaddr (unsigned char *enetaddr, int which)
{
	unsigned char *mac = &product_id_data.d.zone2.mac[which][0];
	char *s = NULL, *e;
	int i;
	char ethbuf[18];
	// If the data is bogus, or the MAC address is 00:00:00 or ff:ff:ff
	// then blow zeros into the address.

	// We only handle the first two interfaces (LAN/WiFi)...
	if (which >= 2)
		return;

#if 0
	printf("%s:%d valid %d which %d %02x:%02x:%02x\n", __FUNCTION__, __LINE__, production_data_valid, which,
	       mac[0], mac[1], mac[2]);
#endif

	memset(enetaddr, '\0', 6);
	if (!production_data_valid || 
	    (mac[0] == 0xff && mac[1] == 0xff && mac[2] == 0xff)
	    || (mac[0] == 0x00 && mac[1] == 0x00 && mac[2] == 0x00)) {
		s = getenv("ethaddr");

		// Skip the WiFi for rev 1 boards (no WiFi address)
		if (which == 1)
		  return;

#ifdef CONFIG_ETHADDR
		if (!s)
			s = MK_STR(CONFIG_ETHADDR);
#endif
#if 1
		printf("%s:%d %s\n", __FUNCTION__, __LINE__, s);
#endif
		for (i = 0; i < 6; ++i) {
			enetaddr[i] = s ? simple_strtoul (s, &e, 16) : 0;
			if (s)
				s = (*e) ? e + 1 : e;
		}
		goto set_it;
	}

	// Use Logic's prefix
	enetaddr[0] = 0x00;
	enetaddr[1] = 0x08;
	enetaddr[2] = 0xee;
	enetaddr[3] = mac[0];
	enetaddr[4] = mac[1];
	enetaddr[5] = mac[2];

 set_it:
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

static int extract_product_id_part_number(struct product_id_data *p, char *buf, int buflen)
{
	int size;

	buf[0] = '\0';
	if (p->d.u_zone0.pz_0r0.header_version == LOGIC_HEADER_VERSION_0) {
		size = sizeof(p->d.u_zone0.pz_0r0.part_number);
		if (buflen < sizeof(p->d.u_zone0.pz_0r0.part_number))
			size = buflen;
		strncpy(buf, p->d.u_zone0.pz_0r0.part_number, sizeof(p->d.u_zone0.pz_0r0.part_number));
		buf[sizeof(p->d.u_zone0.pz_0r0.part_number)] = '\0';
		return 0;
	}

	if (p->d.u_zone0.pz_0r0.header_version == LOGIC_HEADER_VERSION_1) {
		size = sizeof(p->d.u_zone0.pz_0r1.part_number);
		if (buflen < sizeof(p->d.u_zone0.pz_0r1.part_number))
			size = buflen;
		strncpy(buf, p->d.u_zone0.pz_0r1.part_number, sizeof(p->d.u_zone0.pz_0r1.part_number));
		buf[sizeof(p->d.u_zone0.pz_0r1.part_number)] = '\0';
		return 0;
	}

	return -1;
}


static int extract_header_version(struct product_id_data *p, int *header_version)
{
	if (p->d.u_zone0.pz_0r0.header_version == LOGIC_HEADER_VERSION_0) {
		*header_version = p->d.u_zone0.pz_0r0.header_version;
		return 0;
	}

	if (p->d.u_zone0.pz_0r1.header_version == LOGIC_HEADER_VERSION_1) {
		*header_version = p->d.u_zone0.pz_0r1.header_version;
		return 0;
	}

	*header_version = p->d.u_zone0.pz_0r0.header_version;
	return -1;
  
}

static int extract_serial_number(struct product_id_data *p, char *buf, int buflen)
{
	buf[0] = '\0';
	if (p->d.u_zone0.pz_0r0.header_version == LOGIC_HEADER_VERSION_0) {
		sprintf(buf, "%02d%02d%c%05d", p->d.u_zone0.pz_0r0.sn_week,
			 p->d.u_zone0.pz_0r0.sn_year, p->d.u_zone0.pz_0r0.sn_site,
			 p->d.u_zone0.pz_0r0.sn_cnt);
		return 0;
	}
	if (p->d.u_zone0.pz_0r1.header_version == LOGIC_HEADER_VERSION_1) {
		sprintf(buf, "%02d%02d%c%05d", p->d.u_zone0.pz_0r1.sn_week,
			 p->d.u_zone0.pz_0r1.sn_year, p->d.u_zone0.pz_0r1.sn_site,
			 p->d.u_zone0.pz_0r1.sn_cnt);
		return 0;
	}
	return -1;
}

static void extract_model_number_revision(struct product_id_data *p, char *buf, int buflen)
{
	int i;

	strncpy(buf, product_id_data.d.zone1.model_number, buflen);
	buf[buflen-1] = '\0';
	i = strlen(buf);
	if (i + 3 < buflen) {
		buf[i] = '-';
		buf[i+1] = product_id_data.d.zone1.model_revision;
		buf[i+2] = '\0';
	}
}

int fetch_production_data(void)
{
  int err = 0;
  char buf[36];
  int header_version;
  int checksum;

  gpio_i2c_init(50000);

  printf("Read production data: ");
  if (set_user_zone(0) || read_user_zone((unsigned char *)&product_id_data.d.u_zone0, sizeof(product_id_data.d.u_zone0), 0)) {
    printf("failed!\n");
    err = -1;
    goto out;
  }

  // If the header doesn't match, we can't map any of the data
  if (extract_header_version(&product_id_data, &header_version)) {
    err = -2;
    printf("failed - invalid header version %d!\n", header_version);
    goto out;
  }

  if (set_user_zone(1) || read_user_zone((unsigned char *)&product_id_data.d.zone1, sizeof(product_id_data.d.zone1), 0)) {
    printf("failed!\n");
    err = -3;
    goto out;
  }

  if (set_user_zone(2) || read_user_zone((unsigned char *)&product_id_data.d.zone2, sizeof(product_id_data.d.zone2), 0)) {
    printf("failed!\n");
    err = -4;
    goto out;
  }

  printf("done\n");

  // Correct endianess issues
  product_id_data.d.zone2.processor_type = le16_to_cpu(product_id_data.d.zone2.processor_type);
  product_id_data.d.zone2.features = le32_to_cpu(product_id_data.d.zone2.features);
  product_id_data.d.zone2.platform_bits = le32_to_cpu(product_id_data.d.zone2.platform_bits);

    // Print out the name, model number, and set MAC addresses
  extract_product_id_part_number(&product_id_data, buf, sizeof(buf));

    printf("Part Number  : %s\n", buf);
    extract_model_number_revision(&product_id_data, buf, sizeof(buf));
    printf("Model Name   : %s\n", buf);
    extract_serial_number(&product_id_data, buf, sizeof(buf));
    printf("Serial Number: %s\n", buf);

  out:
    production_data_valid = !err;

    // Restore pins back to their intended use
    gpio_i2c_restore_pins();

    // Clone the production data into SRAM
    checksum = calculate_checksum(&product_id_data.d, sizeof(product_id_data.d));
    product_id_data.checksum = checksum;
    *(struct product_id_data *)(SRAM_BASE) = product_id_data;

    return err;
}

