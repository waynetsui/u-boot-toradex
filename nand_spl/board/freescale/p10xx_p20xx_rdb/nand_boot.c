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

void initsdram(void)
{
	volatile ccsr_ddr_t *ddr = (void *)(CONFIG_SYS_MPC85xx_DDR_ADDR);

	out_be32(&ddr->cs0_bnds, CONFIG_SYS_DDR_CS0_BNDS);
	out_be32(&ddr->cs0_config, CONFIG_SYS_DDR_CS0_CONFIG);

	out_be32(&ddr->timing_cfg_3, CONFIG_SYS_DDR_TIMING_3_REVA);
	out_be32(&ddr->timing_cfg_0, CONFIG_SYS_DDR_TIMING_0_REVA);
	out_be32(&ddr->timing_cfg_1, CONFIG_SYS_DDR_TIMING_1_REVA);
	out_be32(&ddr->timing_cfg_2, CONFIG_SYS_DDR_TIMING_2_REVA);
	out_be32(&ddr->sdram_mode, CONFIG_SYS_DDR_MODE_1_REVA);
	out_be32(&ddr->sdram_mode_2, CONFIG_SYS_DDR_MODE_2_REVA);
	out_be32(&ddr->sdram_interval, CONFIG_SYS_DDR_INTERVAL_REVA);
	out_be32(&ddr->sdram_data_init, CONFIG_SYS_DDR_DATA_INIT);
	out_be32(&ddr->sdram_clk_cntl, CONFIG_SYS_DDR_CLK_CTRL_REVA);
	out_be32(&ddr->sdram_cfg_2, CONFIG_SYS_DDR_CONTROL2_REVA);

#if defined(CONFIG_DDR_ECC)
	out_be32(&ddr->err_int_en, CONFIG_SYS_DDR_ERR_INT_EN);
	out_be32(&ddr->err_disable, CONFIG_SYS_DDR_ERR_DIS);
	out_be32(&ddr->err_sbe, CONFIG_SYS_DDR_SBE);
#endif
	asm("sync;isync");

	udelay(200);
	out_be32(&ddr->sdram_cfg, CONFIG_SYS_DDR_CONTROL_REVA);
}

void board_init_f_nand(void)
{
	u8 sysclk_ratio;
	uint plat_ratio, bus_clk, sys_clk;
	volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);

	/* initialize selected port with appropriate baud rate */
#if 0
	sysclk_ratio = *((volatile unsigned char *)(PIXIS_BASE + PIXIS_SPD));
	sysclk_ratio &= 0x7;
	switch (sysclk_ratio) {
	case 0:
		sys_clk = 33333000;
		break;
	case 1:
		sys_clk = 39999600;
		break;
	case 2:
		sys_clk = 49999500;
		break;
	case 3:
		sys_clk = 66666000;
		break;
	case 4:
		sys_clk = 83332500;
		break;
	case 5:
		sys_clk = 99999000;
		break;
	case 6:
		sys_clk = 133332000;
		break;
	case 7:
		sys_clk = 166665000;
		break;
	}
#endif

	sys_clk = 66666000;
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

