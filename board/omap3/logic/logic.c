/*
 * (C) Copyright 2009
 * Logic Product Development, <www.logicpd.com>
 *
 * Author :
 *	Peter Barada <peterb@logicpd.com>
 *
 * Derived from Beagle Board and 3430 SDP code by
 *	Richard Woodruff <r-woodruff2@ti.com>
 *	Syed Mohammed Khasim <khasim@ti.com>
 *
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
#include <netdev.h>
#include <twl4030.h>
#include <asm/io.h>
#include <asm/arch/mem.h>
#include <asm/arch/mux.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/gpio.h>
#include <asm/mach-types.h>
#include "logic.h"

/*
 * Routine: logic_identify
 * Description: Detect if we are running on a Logic or Torpedo.
 *              This can be done by GPIO_189. If its low after driving it high,
 *              then its an LOGIC, else Torpedo.
 */
unsigned int logic_identify(void)
{
	unsigned int val;
	int i;

	MUX_LOGIC_HSUSB0_D5_GPIO_MUX();

	if (!omap_request_gpio(189)) {

		omap_set_gpio_direction(189, 0);
		omap_set_gpio_dataout(189, 1);

		// Let it soak for a bit
		for (i=0; i<0x100; ++i)
			asm("nop");

		omap_set_gpio_direction(189, 1);
		val = omap_get_gpio_datain(189);
		omap_free_gpio(189);
	}

	printf("Board: ");
	if (val) {
		printf("Torpedo\n");
		val = MACH_TYPE_OMAP3_TORPEDO;
	} else {
		printf("LV SOM\n");
		val = MACH_TYPE_OMAP3530_LV_SOM;
	}
	return val;
}

/*
 * Routine: board_init
 * Description: Early hardware init.
 */
int board_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	gpmc_init(); /* in SRAM or SDRAM, finish GPMC */
	/* board id for Linux (placeholder until can ID board) */
	gd->bd->bi_arch_number = MACH_TYPE_OMAP3530_LV_SOM;
	/* boot param addr */
	gd->bd->bi_boot_params = (OMAP34XX_SDRC_CS0 + 0x100);

	return 0;
}


static void setup_net_chip(void);
static void setup_isp1760_chip(void);
static void fix_flash_sync(void);

/*
 * Routine: misc_init_r
 * Description: Configure board specific parts
 */
int misc_init_r(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	struct gpio *gpio5_base = (struct gpio *)OMAP34XX_GPIO5_BASE;
	struct gpio *gpio6_base = (struct gpio *)OMAP34XX_GPIO6_BASE;

#ifdef CONFIG_DRIVER_OMAP34XX_I2C
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
#endif

#if defined(CONFIG_CMD_NET)
	setup_net_chip();
#endif

	/* Setup access to the isp1760 chip on CS6 */
	setup_isp1760_chip();

	/* Fix the flash sync */
	twl4030_power_init();
	twl4030_led_init();

	/* Configure GPIOs to output */
	writel(~(GPIO23 | GPIO10 | GPIO8 | GPIO2 | GPIO1), &gpio6_base->oe);
	writel(~(GPIO31 | GPIO30 | GPIO29 | GPIO28 | GPIO22 | GPIO21 |
		GPIO15 | GPIO14 | GPIO13 | GPIO12), &gpio5_base->oe);

	/* Set GPIOs */
	writel(GPIO23 | GPIO10 | GPIO8 | GPIO2 | GPIO1,
		&gpio6_base->setdataout);
	writel(GPIO31 | GPIO30 | GPIO29 | GPIO28 | GPIO22 | GPIO21 |
		GPIO15 | GPIO14 | GPIO13 | GPIO12, &gpio5_base->setdataout);

	gd->bd->bi_arch_number = logic_identify();

	fix_flash_sync();

	dieid_num_r();

	return 0;
}

/******************************************************************************
 * Routine: late_board_init
 * Description: Late hardware init.
 *****************************************************************************/
int board_late_init(void)
{
	unsigned char enetaddr[6];

	// DECLARE_GLOBAL_DATA_PTR;

	fetch_production_data(); // Extract production data
	// Fetch the ethaddr of the LAN
	board_get_nth_enetaddr(enetaddr, 0);
#ifdef CONFIG_HAS_ETH1
	// Fetch the ethaddr of the WiFi
	board_get_nth_enetaddr(enetaddr, 1);
#endif
	return 0;
}

/*
 * Routine: set_muxconf_regs
 * Description: Setting up the configuration Mux registers specific to the
 *		hardware. Many pins need to be moved from protect to primary
 *		mode.
 */
void set_muxconf_regs(void)
{
	MUX_LOGIC();
}

// GPMC settings for LV SOM Ethernet chip
#define LOGIC_NET_GPMC_CONFIG1  0x00001000
#define LOGIC_NET_GPMC_CONFIG2  0x00080802
#define LOGIC_NET_GPMC_CONFIG3  0x00000000
#define LOGIC_NET_GPMC_CONFIG4  0x08020802
#define LOGIC_NET_GPMC_CONFIG5  0x00080a0a
#define LOGIC_NET_GPMC_CONFIG6  0x00000000
#define LOGIC_NET_GPMC_CONFIG7  0x00000f48

/*
 * Routine: setup_net_chip
 * Description: Setting up the configuration GPMC registers specific to the
 *		Ethernet hardware.
 */
static void setup_net_chip(void)
{
	struct ctrl *ctrl_base = (struct ctrl *)OMAP34XX_CTRL_BASE;

	/* Configure GPMC registers */
	writel(LOGIC_NET_GPMC_CONFIG1, &gpmc_cfg->cs[1].config1);
	writel(LOGIC_NET_GPMC_CONFIG2, &gpmc_cfg->cs[1].config2);
	writel(LOGIC_NET_GPMC_CONFIG3, &gpmc_cfg->cs[1].config3);
	writel(LOGIC_NET_GPMC_CONFIG4, &gpmc_cfg->cs[1].config4);
	writel(LOGIC_NET_GPMC_CONFIG5, &gpmc_cfg->cs[1].config5);
	writel(LOGIC_NET_GPMC_CONFIG6, &gpmc_cfg->cs[1].config6);
	writel(LOGIC_NET_GPMC_CONFIG7, &gpmc_cfg->cs[1].config7);

	/* Enable off mode for NWE in PADCONF_GPMC_NWE register */
	writew(readw(&ctrl_base ->gpmc_nwe) | 0x0E00, &ctrl_base->gpmc_nwe);
	/* Enable off mode for NOE in PADCONF_GPMC_NADV_ALE register */
	writew(readw(&ctrl_base->gpmc_noe) | 0x0E00, &ctrl_base->gpmc_noe);
	/* Enable off mode for ALE in PADCONF_GPMC_NADV_ALE register */
	writew(readw(&ctrl_base->gpmc_nadv_ale) | 0x0E00,
		&ctrl_base->gpmc_nadv_ale);

}

// GPMC settings for LOGIC 1760 chip
#define LOGIC_ISP1760_GPMC_CONFIG1  0x00001200
#define LOGIC_ISP1760_GPMC_CONFIG2  0x00090901
#define LOGIC_ISP1760_GPMC_CONFIG3  0x00091001
#define LOGIC_ISP1760_GPMC_CONFIG4  0x07031002
#define LOGIC_ISP1760_GPMC_CONFIG5  0x00080c0a
#define LOGIC_ISP1760_GPMC_CONFIG6  0x08030200
#define LOGIC_ISP1760_GPMC_CONFIG7  0x00000f5c

/*
 * Routine: setup_isp1760_chip
 * Description: Setting up the configuration GPMC registers specific to the
 *		ISP1760 USB hardware.
 */
static void setup_isp1760_chip(void)
{
	/* Configure GPMC registers */
	writel(LOGIC_ISP1760_GPMC_CONFIG1, &gpmc_cfg->cs[6].config1);
	writel(LOGIC_ISP1760_GPMC_CONFIG2, &gpmc_cfg->cs[6].config2);
	writel(LOGIC_ISP1760_GPMC_CONFIG3, &gpmc_cfg->cs[6].config3);
	writel(LOGIC_ISP1760_GPMC_CONFIG4, &gpmc_cfg->cs[6].config4);
	writel(LOGIC_ISP1760_GPMC_CONFIG5, &gpmc_cfg->cs[6].config5);
	writel(LOGIC_ISP1760_GPMC_CONFIG6, &gpmc_cfg->cs[6].config6);
	writel(LOGIC_ISP1760_GPMC_CONFIG7, &gpmc_cfg->cs[6].config7);
}

#define LOGIC_STNOR_ASYNC_GPMC_CONFIG1	0x00001211
#define LOGIC_STNOR_ASYNC_GPMC_CONFIG2	0x00080901
#define LOGIC_STNOR_ASYNC_GPMC_CONFIG3	0x00020201
#define LOGIC_STNOR_ASYNC_GPMC_CONFIG4	0x08010901
#define LOGIC_STNOR_ASYNC_GPMC_CONFIG5	0x0008090a
#define LOGIC_STNOR_ASYNC_GPMC_CONFIG6	0x08030200
#define LOGIC_STNOR_ASYNC_GPMC_CONFIG7	0x00000c50

#define LOGIC_STNOR_SYNC_GPMC_CONFIG1	0x68411213
#define LOGIC_STNOR_SYNC_GPMC_CONFIG2	0x000C1502
#define LOGIC_STNOR_SYNC_GPMC_CONFIG3	0x00040402
#define LOGIC_STNOR_SYNC_GPMC_CONFIG4	0x0B051505
#define LOGIC_STNOR_SYNC_GPMC_CONFIG5	0x020E0C15
#define LOGIC_STNOR_SYNC_GPMC_CONFIG6	0x0B0603C3
#define LOGIC_STNOR_SYNC_GPMC_CONFIG7	0x00000c50

#define LOGIC_FLASH_BASE 0x10000000

/* These are bit definitions for the RCR register of the NOR flash       */
/* 28FxxxP30 device.  This register sets the bus configration for reads. */
/* settings, located on address pins A[15:0].                            */
#define FLASH_28FxxxP30_RCR_RM    0x8000
#define FLASH_28FxxxP30_RCR_R     0x4000
#define FLASH_28FxxxP30_RCR_LC(x) ((x & 0x7) << 11)
#define FLASH_28FxxxP30_RCR_WP    0x0400
#define FLASH_28FxxxP30_RCR_DH    0x0200
#define FLASH_28FxxxP30_RCR_WD    0x0100
#define FLASH_28FxxxP30_RCR_BS    0x0080
#define FLASH_28FxxxP30_RCR_CE    0x0040
#define FLASH_28FxxxP30_RCR_BW    0x0008
#define FLASH_28FxxxP30_RCR_BL(x) ((x & 0x7) << 0)
#define FLASH_28FxxxP30_BL_4      0x1
#define FLASH_28FxxxP30_BL_8      0x2
#define FLASH_28FxxxP30_BL_16     0x3
#define FLASH_28FxxxP30_BL_CONT   0x7

/*
 * Routine: fix_flash_sync
 * Description: Setting up the configuration GPMC registers specific to the
 *		NOR flash (and place in sync mode if not done).
 */
static void fix_flash_sync(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	int arch_number;
	u16 rcrval;

	/* Check the arch_number - Torpedo doesn't have NOR flash */
	arch_number = gd->bd->bi_arch_number;
	if (arch_number == MACH_TYPE_OMAP3_TORPEDO)
		return;

	/* Check CS2 config, if its already in sync, then return */
	if (!(readl(&gpmc_cfg->cs[2].config1) & TYPE_READTYPE)) {
		puts("NOR: initialize in sync mode\n");

		/* clear WAIT1 polarity */
		writel(readl(&gpmc_cfg->config) & ~0x200, &gpmc_cfg->config);

		/* clear GPMC_TIMEOUT */
		writel(0x0, &gpmc_cfg->timeout_control);

		/* Configure GPMC registers for async */
		writel(LOGIC_STNOR_ASYNC_GPMC_CONFIG1, &gpmc_cfg->cs[2].config1);
		writel(LOGIC_STNOR_ASYNC_GPMC_CONFIG2, &gpmc_cfg->cs[2].config2);
		writel(LOGIC_STNOR_ASYNC_GPMC_CONFIG3, &gpmc_cfg->cs[2].config3);
		writel(LOGIC_STNOR_ASYNC_GPMC_CONFIG4, &gpmc_cfg->cs[2].config4);
		writel(LOGIC_STNOR_ASYNC_GPMC_CONFIG5, &gpmc_cfg->cs[2].config5);
		writel(LOGIC_STNOR_ASYNC_GPMC_CONFIG6, &gpmc_cfg->cs[2].config6);
		writel(LOGIC_STNOR_ASYNC_GPMC_CONFIG7, &gpmc_cfg->cs[2].config7);

		/* 1st NOR cycle, send read config register setup 0x60 */
		*(volatile u16 *)LOGIC_FLASH_BASE = 0x0060;

		/* 2nd NOR cycle, send 0x03 to latch in read
		 * configuration register setttings, located on A[15:0] */
		rcrval = FLASH_28FxxxP30_RCR_LC(4) | FLASH_28FxxxP30_RCR_WP |
		  FLASH_28FxxxP30_RCR_BS | FLASH_28FxxxP30_RCR_CE |
		  FLASH_28FxxxP30_RCR_BW | FLASH_28FxxxP30_RCR_BL(FLASH_28FxxxP30_BL_4);
		*(volatile u16 *)(LOGIC_FLASH_BASE | (rcrval << 1)) = 0x0003;

		/* Give a chance for accesses to finish... */
		sdelay(2000);

		/* Third, set GPMC for sync. */
		writel(LOGIC_STNOR_SYNC_GPMC_CONFIG1, &gpmc_cfg->cs[2].config1);
		writel(LOGIC_STNOR_SYNC_GPMC_CONFIG2, &gpmc_cfg->cs[2].config2);
		writel(LOGIC_STNOR_SYNC_GPMC_CONFIG3, &gpmc_cfg->cs[2].config3);
		writel(LOGIC_STNOR_SYNC_GPMC_CONFIG4, &gpmc_cfg->cs[2].config4);
		writel(LOGIC_STNOR_SYNC_GPMC_CONFIG5, &gpmc_cfg->cs[2].config5);
		writel(LOGIC_STNOR_SYNC_GPMC_CONFIG6, &gpmc_cfg->cs[2].config6);
		writel(LOGIC_STNOR_SYNC_GPMC_CONFIG7, &gpmc_cfg->cs[2].config7);
		/* And lastly, set the WAIT1 polarity high */
		writel(readl(&gpmc_cfg->config) | 0x200, &gpmc_cfg->config);
	} else
		puts ("NOR: Already initialized in sync mode\n");
}


int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_SMC911X
	rc = smc911x_initialize(0, CONFIG_SMC911X_BASE);
#endif
	return rc;
}
