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

int do_dump_osc_clk (cmd_tbl_t *cmdtp, int flah, int argc, char *argv[])
{
	extern u32 get_osc_clk_speed(void);

	printf("get_osc_clk_speed() = %d\n", get_osc_clk_speed());

	return 0;
}

U_BOOT_CMD(
	dump_osc_clk,	1,	1,	do_dump_osc_clk,
	"dump_osc_clk - display current timer output\n",
	"\n    - display current timer output\n"
);

#define MOD_CM 0
#define MOD_PRM 1

#define OCP_MOD						0x000
#define MPU_MOD						0x100
#define CORE_MOD					0x200
#define GFX_MOD						0x300
#define WKUP_MOD					0x400
#define PLL_MOD						0x500

#define OMAP3430_IVA2_MOD				-0x800
#define OMAP3430ES2_SGX_MOD				GFX_MOD
#define OMAP3430_CCR_MOD				PLL_MOD
#define OMAP3430_DSS_MOD				0x600
#define OMAP3430_CAM_MOD				0x700
#define OMAP3430_PER_MOD				0x800
#define OMAP3430_EMU_MOD				0x900
#define OMAP3430_GR_MOD					0xa00
#define OMAP3430_NEON_MOD				0xb00
#define OMAP3430ES2_USBHOST_MOD				0xc00

struct pm_module_def {
	char name[8]; /* Name of the module */
	short type; /* CM or PRM */
	short offset;
	int low; /* First register address on this module */
	int high; /* Last register address on this module */
};

static const struct pm_module_def pm_dbg_reg_modules[] = {
	{ "IVA2", MOD_CM, OMAP3430_IVA2_MOD, 0, 0x4c },
	{ "OCP", MOD_CM, OCP_MOD, 0, 0x10 },
	{ "MPU", MOD_CM, MPU_MOD, 4, 0x4c },
	{ "CORE", MOD_CM, CORE_MOD, 0, 0x4c },
	{ "SGX", MOD_CM, OMAP3430ES2_SGX_MOD, 0, 0x4c },
	{ "WKUP", MOD_CM, WKUP_MOD, 0, 0x40 },
	{ "CCR", MOD_CM, PLL_MOD, 0, 0x70 },
	{ "DSS", MOD_CM, OMAP3430_DSS_MOD, 0, 0x4c },
	{ "CAM", MOD_CM, OMAP3430_CAM_MOD, 0, 0x4c },
	{ "PER", MOD_CM, OMAP3430_PER_MOD, 0, 0x4c },
	{ "EMU", MOD_CM, OMAP3430_EMU_MOD, 0x40, 0x54 },
	{ "NEON", MOD_CM, OMAP3430_NEON_MOD, 0x20, 0x48 },
	{ "USB", MOD_CM, OMAP3430ES2_USBHOST_MOD, 0, 0x4c },

	{ "IVA2", MOD_PRM, OMAP3430_IVA2_MOD, 0x50, 0xfc },
	{ "OCP", MOD_PRM, OCP_MOD, 4, 0x1c },
	{ "MPU", MOD_PRM, MPU_MOD, 0x58, 0xe8 },
	{ "CORE", MOD_PRM, CORE_MOD, 0x58, 0xf8 },
	{ "SGX", MOD_PRM, OMAP3430ES2_SGX_MOD, 0x58, 0xe8 },
	{ "WKUP", MOD_PRM, WKUP_MOD, 0xa0, 0xb0 },
	{ "CCR", MOD_PRM, PLL_MOD, 0x40, 0x70 },
	{ "DSS", MOD_PRM, OMAP3430_DSS_MOD, 0x58, 0xe8 },
	{ "CAM", MOD_PRM, OMAP3430_CAM_MOD, 0x58, 0xe8 },
	{ "PER", MOD_PRM, OMAP3430_PER_MOD, 0x58, 0xe8 },
	{ "EMU", MOD_PRM, OMAP3430_EMU_MOD, 0x58, 0xe4 },
	{ "GLBL", MOD_PRM, OMAP3430_GR_MOD, 0x20, 0xe4 },
	{ "NEON", MOD_PRM, OMAP3430_NEON_MOD, 0x58, 0xe8 },
	{ "USB", MOD_PRM, OMAP3430ES2_USBHOST_MOD, 0x58, 0xe8 },
	{ "", 0, 0, 0, 0 },
};
#define OMAP3430_CM_BASE	0x48004800
#define OMAP3430_PRM_BASE	0x48306800

#define OMAP2_CM_BASE			OMAP3430_CM_BASE
#define OMAP2_PRM_BASE			OMAP3430_PRM_BASE

unsigned long cm_read_mod_reg(s16 module, u16 idx)
{
  return 0x12345678;
}

unsigned long prm_read_mod_reg(s16 module, u16 idx)
{
  return 0x12345678;
}

int do_dump_prcm_regs (cmd_tbl_t *cmdtp, int flah, int argc, char *argv[])
{
  int i, j;
  unsigned long val;
  int regs, linefeed;
  unsigned int addr;
  for (i=0; pm_dbg_reg_modules[i].name[0] != 0; ++i) {
    regs = 0;
    linefeed = 0;

    if (pm_dbg_reg_modules[0].type == MOD_CM) {
      addr = (u32)(OMAP2_CM_BASE + pm_dbg_reg_modules[i].offset);
      printf("MOD: CM_%s (%08x)\n", pm_dbg_reg_modules[i].name, addr);
    } else {
      addr = (u32)(OMAP2_PRM_BASE + pm_dbg_reg_modules[i].offset);
      printf("MOD: PRM_%s (%08x)\n", pm_dbg_reg_modules[i].name, addr);

    }
    for (j=pm_dbg_reg_modules[i].low;
			j <= pm_dbg_reg_modules[i].high; j += 4) {

#if 1
      // printf("addr %x offset %x j %x\n", addr, pm_dbg_reg_modules[i].offset, j);
      val = readl(addr + j);
#else
      if (pm_dbg_reg_modules[i].type == MOD_CM)
	val = cm_read_mod_reg(pm_dbg_reg_modules[i].offset, j);
      else
	val = prm_read_mod_reg(pm_dbg_reg_modules[i].offset, j);
#endif
      if (val != 0) {
	regs++;
	if (linefeed) {
	  printf("\n");
	  linefeed = 0;
	}
	printf("  %02x -> %08lx", j, val);
	if (regs % 4 == 0)
	  linefeed = 1;
      }
    }
    printf("\n");
  }

  return 0;

}



U_BOOT_CMD(
	dump_prcm_regs,	1,	1,	do_dump_prcm_regs,
	"dump_prcm_regs - display PRCM register values\n",
	"\n    - display PRCM register values\n"
);

void gpmc_dump_config(int cs)
{
  u32 gpmc_base = GPMC_CONFIG_CS0_BASE + (cs * GPMC_CONFIG_WIDTH);
  u32 config7;
  u32 base, len;
  gpmc_csx_t *cs_base = (gpmc_csx_t *)gpmc_base;

  config7 = cs_base->config7;
  if (config7 & (1<<6)) {
    len = (((config7 >> 7 & 0xf) ^ 0xf) + 1) << 24;
    base = (config7 & 0x3F) << 24;
  printf("%d:%08x %08x %08x %08x %08x %08x %03x %08x-%08x\n",
	 cs, 
	 cs_base->config1, cs_base->config2, cs_base->config3,
	 cs_base->config4, cs_base->config5, cs_base->config6,
	 cs_base->config7, base, base+len-1);
  }
}

int do_dump_cs (cmd_tbl_t *cmdtp, int flah, int argc, char *argv[])
{
	int i;
	gpmc_t *gpmc_base = (gpmc_t *)GPMC_BASE;
	printf("CS: GPMC_CONFIG [%08x]\n", gpmc_base->config);
	printf("CS: GPMC_IRQENABLE [%08x]\n", gpmc_base->irqenable);
	printf("CS: GPMC_TIMEOUT_CONTROL [%08x]\n", gpmc_base->timeout_control);
	for (i=0; i<8; ++i)
		gpmc_dump_config(i);

	return 0;
}

U_BOOT_CMD(
	dump_cs,	1,	1,	do_dump_cs,
	"dump_cs - display OMAP Chip Select registers\n",
	"\n    - display active OMAP Chip Select registers\n"
);
