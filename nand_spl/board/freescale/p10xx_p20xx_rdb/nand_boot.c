/*
 * Copyright (C) 2007-2009 Freescale Semiconductor, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#include <common.h>
#include <mpc85xx.h>
#include <asm-ppc/io.h>
#include <ns16550.h>
#include <nand.h>
#include <asm-ppc/io.h>

#define udelay(x) {int i, j; for (i = 0; i < x; i++) for (j = 0; j < 10000; j++); }

#define BOARDREV_MASK	0x00100000

void initsdram(void)
{

	volatile ccsr_ddr_t *ddr= (ccsr_ddr_t *)CONFIG_SYS_MPC85xx_DDR_ADDR;
	int d_init, dbw;
	u32 val, temp;
	volatile ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);
	unsigned int ddr_size;
	sys_info_t sysinfo;

	ddr->cs0_bnds = CONFIG_SYS_DDR_CS0_BNDS;
	ddr->cs0_config = CONFIG_SYS_DDR_CS0_CONFIG;
	ddr->cs0_config_2 = CONFIG_SYS_DDR_CS0_CONFIG_2;
	ddr->sdram_data_init = CONFIG_SYS_DDR_DATA_INIT;

	/* On P2020/P1020 RDB boards DDR size varies as follows:
	 * REV A board (512MB P2020 and 256MB P1020)
	 * REV B board (1GB P2020 and 256MB P1020)
	 * FIXME:: must also program cs0_bnds register accordingly.
	 * currently CSO_BNDS is programmed for 1G.
	 */
	val = pgpio->gpdat;
	temp = val & BOARDREV_MASK;
	if(temp == 0) {
		/* Rev A board*/
		ddr->timing_cfg_3 = CONFIG_SYS_DDR_TIMING_3_REVA;
		ddr->timing_cfg_0 = CONFIG_SYS_DDR_TIMING_0_REVA;
		ddr->timing_cfg_1 = CONFIG_SYS_DDR_TIMING_1_REVA;
		ddr->timing_cfg_2 = CONFIG_SYS_DDR_TIMING_2_REVA;
		ddr->timing_cfg_4 = CONFIG_SYS_DDR_TIMING_4_REVA;
		ddr->timing_cfg_5 = CONFIG_SYS_DDR_TIMING_5_REVA;
		ddr->sdram_mode = CONFIG_SYS_DDR_MODE_1_REVA;
		ddr->sdram_mode_2 = CONFIG_SYS_DDR_MODE_2_REVA;
		ddr->sdram_interval = CONFIG_SYS_DDR_INTERVAL_REVA;
		ddr->sdram_clk_cntl = CONFIG_SYS_DDR_CLK_CTRL_REVA;
		ddr->sdram_cfg = CONFIG_SYS_DDR_CONTROL_REVA;
		ddr->sdram_cfg_2 = CONFIG_SYS_DDR_CONTROL2_REVA;
	}
	else {
		ddr->timing_cfg_3 = CONFIG_SYS_DDR_TIMING_3_667_REVB;
		ddr->timing_cfg_0 = CONFIG_SYS_DDR_TIMING_0_667_REVB;
		ddr->timing_cfg_1 = CONFIG_SYS_DDR_TIMING_1_667_REVB;
		ddr->timing_cfg_2 = CONFIG_SYS_DDR_TIMING_2_667_REVB;
		ddr->timing_cfg_4 = CONFIG_SYS_DDR_TIMING_4_667_REVB;
		ddr->timing_cfg_5 = CONFIG_SYS_DDR_TIMING_5_667_REVB;
		ddr->sdram_mode = CONFIG_SYS_DDR_MODE_1_667_REVB;
		ddr->sdram_mode_2 = CONFIG_SYS_DDR_MODE_2_667_REVB;
		ddr->sdram_interval = CONFIG_SYS_DDR_INTERVAL_667_REVB;
		ddr->sdram_clk_cntl = CONFIG_SYS_DDR_CLK_CTRL_667_REVB;
		ddr->sdram_cfg = CONFIG_SYS_DDR_CONTROL_667_REVB;
		ddr->sdram_cfg_2 = CONFIG_SYS_DDR_CONTROL2_667_REVB;
	}
 
#if defined (CONFIG_DDR_ECC)
	ddr->err_int_en = CONFIG_SYS_DDR_ERR_INT_EN;
	ddr->err_disable = CONFIG_SYS_DDR_ERR_DIS;
	ddr->err_sbe = CONFIG_SYS_DDR_SBE;
#endif
	asm("sync;isync");

	udelay(500);

	ddr->sdram_cfg |= 0x80000000;

}

void board_init_f_nand(void)
{
	u8 sysclk_ratio;
	uint plat_ratio, bus_clk, sys_clk;
	volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
	u32 val, temp;
	volatile ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);

	/* initialize selected port with appropriate baud rate */
	val = pgpio->gpdat;
	temp = val & BOARDREV_MASK;
	if(temp == 0)
		sys_clk = 66666666;
	else
		sys_clk = 50000000;
	plat_ratio = (gur->porpllsr) & 0x0000003e;
	plat_ratio >>= 1;
	bus_clk = plat_ratio * sys_clk;
	NS16550_init((NS16550_t)(CONFIG_SYS_CCSRBAR + 0x4500),
			bus_clk / 16 / CONFIG_BAUDRATE);


	/* board specific DDR initialization */
	initsdram();

	/* copy code to DDR and jump to it - this should not return */
	/* NOTE - code has to be copied out of NAND buffer before
	 * other blocks can be read.
	 */
	relocate_code(CONFIG_SYS_NAND_U_BOOT_RELOC + 0x10000, 0,
			CONFIG_SYS_NAND_U_BOOT_RELOC);
}

void board_init_r(gd_t *gd, ulong dest_addr)
{
	nand_boot();
}

void putc(char c)
{
	if (c == '\n')
		NS16550_putc((NS16550_t)(CONFIG_SYS_CCSRBAR + 0x4500), '\r');

	NS16550_putc((NS16550_t)(CONFIG_SYS_CCSRBAR + 0x4500), c);
}

void puts(const char *str)
{
	while (*str)
		putc(*str++);
}

