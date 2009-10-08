/*
 * (C) Copyright 2004-2008
 * Texas Instruments, <www.ti.com>
 *
 * Author :
 *	Nishanth Menon <nm@ti.com>
 *
 * Derived from Beagle Board and 3430 SDP code by
 *	Sunil Kumar <sunilsaini05@gmail.com>
 *	Shashi Ranjan <shashiranjanmca05@gmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <asm/io.h>
#include <asm/arch/mem.h>
#include <asm/arch/mux.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-types.h>
#include "lv_som.h"

/******************************************************************************
 * Routine: board_init
 * Description: Early hardware init.
 *****************************************************************************/
int board_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	gpmc_init(); /* in SRAM or SDRAM, finish GPMC */
	/* board id for Linux */
	gd->bd->bi_arch_number = MACH_TYPE_OMAP3530_LV_SOM;
	/* boot param addr */
	gd->bd->bi_boot_params = (OMAP34XX_SDRC_CS0 + 0x100);

	return 0;
}

/******************************************************************************
 * Routine: misc_init_r
 * Description: Configure zoom board specific configurations
 *****************************************************************************/
int misc_init_r(void)
{
	power_init_r();

#if defined(CONFIG_CMD_NET)
	setup_net_chip();
#endif

	dieid_num_r();
	return 0;
}

/******************************************************************************
 * Routine: setup_net_chip
 * Description: Setting up the configuration GPMC registers specific to the
 *		Ethernet hardware.
 *****************************************************************************/
static void setup_net_chip(void)
{
	// gpio_t *gpio3_base = (gpio_t *)OMAP34XX_GPIO3_BASE;
	gpmc_csx_t *gpmc_cs1_base = (gpmc_csx_t *)GPMC_CONFIG_CS1_BASE;
	ctrl_t *ctrl_base = (ctrl_t *)OMAP34XX_CTRL_BASE;

	/* Configure GPMC registers */
	writel(0x00000000, &gpmc_cs1_base->config7);
	sdelay(1000);

	writel(LV_SOM_NET_GPMC_CONFIG1, &gpmc_cs1_base->config1);
	writel(LV_SOM_NET_GPMC_CONFIG2, &gpmc_cs1_base->config2);
	writel(LV_SOM_NET_GPMC_CONFIG3, &gpmc_cs1_base->config3);
	writel(LV_SOM_NET_GPMC_CONFIG4, &gpmc_cs1_base->config4);
	writel(LV_SOM_NET_GPMC_CONFIG5, &gpmc_cs1_base->config5);
	writel(LV_SOM_NET_GPMC_CONFIG6, &gpmc_cs1_base->config6);
	writel(LV_SOM_NET_GPMC_CONFIG7, &gpmc_cs1_base->config7);
	sdelay(2000);

	/* Enable off mode for NWE in PADCONF_GPMC_NWE register */
	writew(readw(&ctrl_base ->gpmc_nwe) | 0x0E00, &ctrl_base->gpmc_nwe);
	/* Enable off mode for NOE in PADCONF_GPMC_NADV_ALE register */
	writew(readw(&ctrl_base->gpmc_noe) | 0x0E00, &ctrl_base->gpmc_noe);
	/* Enable off mode for ALE in PADCONF_GPMC_NADV_ALE register */
	writew(readw(&ctrl_base->gpmc_nadv_ale) | 0x0E00,
		&ctrl_base->gpmc_nadv_ale);

#if 0
	/* Make GPIO 64 as output pin */
	writel(readl(&gpio3_base->oe) & ~(GPIO0), &gpio3_base->oe);

	/* Now send a pulse on the GPIO pin */
	writel(GPIO0, &gpio3_base->setdataout);
	udelay(1);
	writel(GPIO0, &gpio3_base->cleardataout);
	udelay(1);
	writel(GPIO0, &gpio3_base->setdataout);
#endif
}

/******************************************************************************
 * Routine: set_muxconf_regs
 * Description: Setting up the configuration Mux registers specific to the
 *		hardware. Many pins need to be moved from protect to primary
 *		mode.
 *****************************************************************************/
void set_muxconf_regs(void)
{
	/*
	 * IEN	- Input Enable
	 * IDIS	- Input Disable
	 * PTD	- Pull type Down
	 * PTU	- Pull type Up
	 * DIS	- Pull type selection is inactive
	 * EN	- Pull type selection is active
	 * M0	- Mode 0
	 * The commented string gives the final mux configuration for that pin
	 */

	/*SDRC*/
	MUX_VAL(CP(SDRC_D0),		(IEN  | PTD | DIS | M0)); /*SDRC_D0*/
	MUX_VAL(CP(SDRC_D1),		(IEN  | PTD | DIS | M0)); /*SDRC_D1*/
	MUX_VAL(CP(SDRC_D2),		(IEN  | PTD | DIS | M0)); /*SDRC_D2*/
	MUX_VAL(CP(SDRC_D3),		(IEN  | PTD | DIS | M0)); /*SDRC_D3*/
	MUX_VAL(CP(SDRC_D4),		(IEN  | PTD | DIS | M0)); /*SDRC_D4*/
	MUX_VAL(CP(SDRC_D5),		(IEN  | PTD | DIS | M0)); /*SDRC_D5*/
	MUX_VAL(CP(SDRC_D6),		(IEN  | PTD | DIS | M0)); /*SDRC_D6*/
	MUX_VAL(CP(SDRC_D7),		(IEN  | PTD | DIS | M0)); /*SDRC_D7*/
	MUX_VAL(CP(SDRC_D8),		(IEN  | PTD | DIS | M0)); /*SDRC_D8*/
	MUX_VAL(CP(SDRC_D9),		(IEN  | PTD | DIS | M0)); /*SDRC_D9*/
	MUX_VAL(CP(SDRC_D10),		(IEN  | PTD | DIS | M0)); /*SDRC_D10*/
	MUX_VAL(CP(SDRC_D11),		(IEN  | PTD | DIS | M0)); /*SDRC_D11*/
	MUX_VAL(CP(SDRC_D12),		(IEN  | PTD | DIS | M0)); /*SDRC_D12*/
	MUX_VAL(CP(SDRC_D13),		(IEN  | PTD | DIS | M0)); /*SDRC_D13*/
	MUX_VAL(CP(SDRC_D14),		(IEN  | PTD | DIS | M0)); /*SDRC_D14*/
	MUX_VAL(CP(SDRC_D15),		(IEN  | PTD | DIS | M0)); /*SDRC_D15*/
	MUX_VAL(CP(SDRC_D16),		(IEN  | PTD | DIS | M0)); /*SDRC_D16*/
	MUX_VAL(CP(SDRC_D17),		(IEN  | PTD | DIS | M0)); /*SDRC_D17*/
	MUX_VAL(CP(SDRC_D18),		(IEN  | PTD | DIS | M0)); /*SDRC_D18*/
	MUX_VAL(CP(SDRC_D19),		(IEN  | PTD | DIS | M0)); /*SDRC_D19*/
	MUX_VAL(CP(SDRC_D20),		(IEN  | PTD | DIS | M0)); /*SDRC_D20*/
	MUX_VAL(CP(SDRC_D21),		(IEN  | PTD | DIS | M0)); /*SDRC_D21*/
	MUX_VAL(CP(SDRC_D22),		(IEN  | PTD | DIS | M0)); /*SDRC_D22*/
	MUX_VAL(CP(SDRC_D23),		(IEN  | PTD | DIS | M0)); /*SDRC_D23*/
	MUX_VAL(CP(SDRC_D24),		(IEN  | PTD | DIS | M0)); /*SDRC_D24*/
	MUX_VAL(CP(SDRC_D25),		(IEN  | PTD | DIS | M0)); /*SDRC_D25*/
	MUX_VAL(CP(SDRC_D26),		(IEN  | PTD | DIS | M0)); /*SDRC_D26*/
	MUX_VAL(CP(SDRC_D27),		(IEN  | PTD | DIS | M0)); /*SDRC_D27*/
	MUX_VAL(CP(SDRC_D28),		(IEN  | PTD | DIS | M0)); /*SDRC_D28*/
	MUX_VAL(CP(SDRC_D29),		(IEN  | PTD | DIS | M0)); /*SDRC_D29*/
	MUX_VAL(CP(SDRC_D30),		(IEN  | PTD | DIS | M0)); /*SDRC_D30*/
	MUX_VAL(CP(SDRC_D31),		(IEN  | PTD | DIS | M0)); /*SDRC_D31*/
	MUX_VAL(CP(SDRC_CLK),		(IEN  | PTD | DIS | M0)); /*SDRC_CLK*/
	MUX_VAL(CP(SDRC_DQS0),		(IEN  | PTD | DIS | M0)); /*SDRC_DQS0*/
	MUX_VAL(CP(SDRC_DQS1),		(IEN  | PTD | DIS | M0)); /*SDRC_DQS1*/
	MUX_VAL(CP(SDRC_DQS2),		(IEN  | PTD | DIS | M0)); /*SDRC_DQS2*/
	MUX_VAL(CP(SDRC_DQS3),		(IEN  | PTD | DIS | M0)); /*SDRC_DQS3*/
	/*GPMC*/
	MUX_VAL(CP(GPMC_A1),		(IDIS | PTD | DIS | M0)); /*GPMC_A1*/
	MUX_VAL(CP(GPMC_A2),		(IDIS | PTD | DIS | M0)); /*GPMC_A2*/
	MUX_VAL(CP(GPMC_A3),		(IDIS | PTD | DIS | M0)); /*GPMC_A3*/
	MUX_VAL(CP(GPMC_A4),		(IDIS | PTD | DIS | M0)); /*GPMC_A4*/
	MUX_VAL(CP(GPMC_A5),		(IDIS | PTD | DIS | M0)); /*GPMC_A5*/
	MUX_VAL(CP(GPMC_A6),		(IDIS | PTD | DIS | M0)); /*GPMC_A6*/
	MUX_VAL(CP(GPMC_A7),		(IDIS | PTD | DIS | M0)); /*GPMC_A7*/
	MUX_VAL(CP(GPMC_A8),		(IDIS | PTD | DIS | M0)); /*GPMC_A8*/
	MUX_VAL(CP(GPMC_A9),		(IDIS | PTD | DIS | M0)); /*GPMC_A9*/
	MUX_VAL(CP(GPMC_A10),		(IDIS | PTD | DIS | M0)); /*GPMC_A10*/
	MUX_VAL(CP(GPMC_D0),		(IEN  | PTD | DIS | M0)); /*GPMC_D0*/
	MUX_VAL(CP(GPMC_D1),		(IEN  | PTD | DIS | M0)); /*GPMC_D1*/
	MUX_VAL(CP(GPMC_D2),		(IEN  | PTD | DIS | M0)); /*GPMC_D2*/
	MUX_VAL(CP(GPMC_D3),		(IEN  | PTD | DIS | M0)); /*GPMC_D3*/
	MUX_VAL(CP(GPMC_D4),		(IEN  | PTD | DIS | M0)); /*GPMC_D4*/
	MUX_VAL(CP(GPMC_D5),		(IEN  | PTD | DIS | M0)); /*GPMC_D5*/
	MUX_VAL(CP(GPMC_D6),		(IEN  | PTD | DIS | M0)); /*GPMC_D6*/
	MUX_VAL(CP(GPMC_D7),		(IEN  | PTD | DIS | M0)); /*GPMC_D7*/
	MUX_VAL(CP(GPMC_D8),		(IEN  | PTD | DIS | M0)); /*GPMC_D8*/
	MUX_VAL(CP(GPMC_D9),		(IEN  | PTD | DIS | M0)); /*GPMC_D9*/
	MUX_VAL(CP(GPMC_D10),		(IEN  | PTD | DIS | M0)); /*GPMC_D10*/
	MUX_VAL(CP(GPMC_D11),		(IEN  | PTD | DIS | M0)); /*GPMC_D11*/
	MUX_VAL(CP(GPMC_D12),		(IEN  | PTD | DIS | M0)); /*GPMC_D12*/
	MUX_VAL(CP(GPMC_D13),		(IEN  | PTD | DIS | M0)); /*GPMC_D13*/
	MUX_VAL(CP(GPMC_D14),		(IEN  | PTD | DIS | M0)); /*GPMC_D14*/
	MUX_VAL(CP(GPMC_D15),		(IEN  | PTD | DIS | M0)); /*GPMC_D15*/
	MUX_VAL(CP(GPMC_NCS0),		(IDIS | PTU | EN  | M0)); /*GPMC_nCS0*/
	MUX_VAL(CP(GPMC_NCS1),		(IDIS | PTU | EN  | M0)); /*GPMC_nCS1*/
	MUX_VAL(CP(GPMC_NCS2),		(IDIS | PTU | EN  | M0)); /*GPMC_nCS2*/
	MUX_VAL(CP(GPMC_NCS3),		(IEN  | PTD | DIS | M0)); /*GPMC_nCS3*/
	MUX_VAL(CP(GPMC_NCS4),		(IDIS | PTU | EN  | M7)); /*GPMC_nCS4*/
	MUX_VAL(CP(GPMC_NCS5),		(IDIS | PTD | DIS | M7)); /*GPMC_nCS5*/
	MUX_VAL(CP(GPMC_NCS6),		(IEN  | PTD | DIS | M7)); /*GPMC_nCS6*/
	MUX_VAL(CP(GPMC_NCS7),		(IEN  | PTU | EN  | M1)); /*GPMC_IO_CIR*/ 
	MUX_VAL(CP(GPMC_CLK),		(IDIS | PTD | DIS | M0)); /*GPMC_CLK*/
	MUX_VAL(CP(GPMC_NADV_ALE),	(IDIS | PTD | DIS | M0)); /*GPMC_nADV_ALE*/
	MUX_VAL(CP(GPMC_NOE),		(IDIS | PTD | DIS | M0)); /*GPMC_nOE*/
	MUX_VAL(CP(GPMC_NWE),		(IDIS | PTD | DIS | M0)); /*GPMC_nWE*/
	MUX_VAL(CP(GPMC_NWP),		(IDIS | PTU | DIS | M0)); /*GPMC_nWP*/
	MUX_VAL(CP(GPMC_NBE0_CLE),	(IDIS | PTD | DIS | M0)); /*GPMC_nBE0_CLE*/
	MUX_VAL(CP(GPMC_NBE1),		(IEN  | PTD | DIS | M0)); /*GPMC_nBE1*/
	MUX_VAL(CP(GPMC_WAIT0),		(IEN  | PTD | EN  | M0)); /*GPMC_WAIT0*/
	MUX_VAL(CP(GPMC_WAIT1),		(IEN  | PTU | EN  | M0)); /*GPMC_WAIT1*/
	MUX_VAL(CP(GPMC_WAIT2),		(IEN  | PTU | EN  | M0)); /*GPMC_WAIT2*/
	MUX_VAL(CP(GPMC_WAIT3),		(IEN  | PTU | EN  | M0)); /*GPMC_WAIT3*/
	MUX_VAL(CP(I2C1_SCL),		(IEN  | PTU | EN  | M0)); /*I2C1_SCL*/
	MUX_VAL(CP(I2C1_SDA),		(IEN  | PTU | EN  | M0)); /*I2C1_SDA*/
	MUX_VAL(CP(I2C2_SCL),		(IEN  | PTU | EN  | M0)); /*I2C1_SCL*/
	MUX_VAL(CP(I2C2_SDA),		(IEN  | PTU | EN  | M0)); /*I2C1_SDA*/
	MUX_VAL(CP(I2C3_SCL),		(IEN  | PTU | EN  | M0)); /*I2C1_SCL*/
	MUX_VAL(CP(I2C3_SDA),		(IEN  | PTU | EN  | M0)); /*I2C1_SDA*/
	MUX_VAL(CP(I2C4_SCL),		(IEN  | PTU | EN  | M0)); /*I2C4_SCL*/
	MUX_VAL(CP(I2C4_SDA),		(IEN  | PTU | EN  | M0)); /*I2C4_SDA*/
}
