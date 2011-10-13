/*
 *  (C) Copyright 2010,2011
 *  NVIDIA Corporation <www.nvidia.com>
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

#ifndef __TEGRA3_COMMON_H
#define __TEGRA3_COMMON_H
#include <asm/sizes.h>

#include "tegra-common.h"

#define CONFIG_TEGRA3			/* NVidia Tegra3 core */

/* include default commands */
#include <config_cmd_default.h>

/* remove unused commands */
#undef CONFIG_CMD_FLASH		/* flinfo, erase, protect */
#undef CONFIG_CMD_FPGA		/* FPGA configuration support */
#undef CONFIG_CMD_IMI
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_NFS		/* NFS support */
#undef CONFIG_CMD_NET

#define CONFIG_CMD_CACHE

/* turn on command-line edit/hist/auto */
#define CONFIG_CMDLINE_EDITING
#define CONFIG_COMMAND_HISTORY
#define CONFIG_AUTOCOMPLETE

#define CONFIG_SYS_NO_FLASH

/* Passed on the kernel command line to specify the console. */
#define CONFIG_LINUXCONSOLE "console=ttyS0,115200n8"

/*
 * Defines the standard boot args; these are used in the vboot case (which
 * doesn't run regen_all) as well as used as part of regen_all.
 */
#define CONFIG_BOOTARGS \
	CONFIG_LINUXCONSOLE

/*
 * Extra bootargs used for direct booting, but not for vboot.
 * Not needed for T30.
 */
#define CONFIG_DIRECT_BOOTARGS		CONFIG_BOOTARGS

#define CONFIG_SYS_LOAD_ADDR		(0x80A00800)	/* default */

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKBASE	(NV_PA_SDRAM_BASE + 0x2800000)	/* 40MB */
#define CONFIG_STACKSIZE	0x20000		/* 128K regular stack*/

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		TEGRA_SDRC_CS0
#define PHYS_SDRAM_1_SIZE	0x40000000	/* 1GB */

#define CONFIG_SYS_TEXT_BASE	0x80E08000
#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1

#endif /* __TEGRA3_COMMON_H */
