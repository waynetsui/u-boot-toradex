/*
 * (C) Copyright 2009
 * Martha Marx, Silicon Turnkey Express, mmarx@silicontkx.com
 *
 * Based on original start.S done by
 * 	Copyright (C) 1998  Dan Malek <dmalek@jlc.net>
 * 	Copyright (C) 1999  Magnus Damm <kieraypc01.p.y.kie.era.ericsson.se>
 * 	Copyright (C) 2000, 2001, 2002, 2007 Wolfgang Denk <wd@denx.de>
 * start.S for mpc512x was originally based on the MPC83xx code.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 *  U-Boot - NAND Boot Startup Code for MPC5121 Embedded Boards
 */
#define DEBUG

#include <config.h>
#include <mpc512x.h>
#include <version.h>
#include "dram.h"
//#include "nfc.h"

#define CONFIG_521X	1	/* needed for Linux kernel header files*/

#include <ppc_asm.tmpl>
#include <ppc_defs.h>

#include <asm/cache.h>
#include <asm/mmu.h>

#ifndef  CONFIG_IDENT_STRING
#define  CONFIG_IDENT_STRING "MPC512X"
#endif

/*
 * Floating Point enable, Machine Check and Recoverable Interr.
 */
#undef	MSR_KERNEL
#ifdef DEBUG
#define MSR_KERNEL (MSR_FP|MSR_RI)
#else
#define MSR_KERNEL (MSR_FP|MSR_ME|MSR_RI)
#endif

/* Macros for manipulating CSx_START/STOP */
#define START_REG(start)	((start) >> 16)
#define STOP_REG(start, size)	(((start) + (size) - 1) >> 16)
#define SET_MEM_BASE(r, b) 	\
	lis     r,(b)@h;	\
	ori     r,r,(b)@l;	\

#define SET_REG32(r, v, offset, mr)	\
	lis     r, v@h; 	\
	ori     r, r, v@l;	\
	stw     r, offset(mr);	\

#define SET_REG16(r, v, offset, mr)	\
	li	r, v;		\
	sth	r, offset(mr);	\


	.text
	.globl	version_string
version_string:
	.ascii U_BOOT_VERSION
	.ascii " (", __DATE__, " - ", __TIME__, ")"
	.ascii " ", CONFIG_IDENT_STRING, " "
	.ascii "2K NAND BOOT ","\0"
	. = EXC_OFF_SYS_RESET

	.globl	_start
	/* Start from here after reset/power on */
_start:
boot_cold:
	/* Save msr contents */
	mfmsr	r5
	lis	r4, CONFIG_DEFAULT_IMMR@h

	/* Set IMMR area to our preferred location */
	mfspr	r6, MBAR
	lis	r3, CFG_IMMR@h
	ori	r3, r3, CFG_IMMR@l

	cmpw	r3, r6
	beq	1f /* it has already been set to what we want it to be */
		   /* -- nice to chk if coming out of the BDI          */


	stw	r3, IMMRBAR(r4)
	mtspr	MBAR, r3 /* IMMRBAR is mirrored into the MBAR SPR (311) */
	isync 
1:	lis	r4, START_REG(CFG_FLASH_BASE)
	ori	r4, r4, STOP_REG(CFG_FLASH_BASE, CFG_FLASH_SIZE)
	stw	r4, LPBAW(r3)
	stw	r4, LPCS0AW(r3)
	isync
	/* Initialise the machine */
	bl	cpu_early_init
	isync

	/*
	 * The SRAM window has a fixed size (256K),
	 * so only the start addressis necessary
	 */
	lis     r3, CFG_IMMR@h
	ori     r3, r3, CFG_IMMR@l
	lis	r4, START_REG(CFG_SRAM_BASE) & 0xff00
	stw	r4, SRAMBAR(r3)

	/*
	 * According to MPC5121e RM, configuring local access windows should
	 * be followed by a dummy read of the config register that was
	 * modified last and an isync
	 */
	lwz	r4, SRAMBAR(r3)
	isync
	
#if 0
#ifdef CONFIG_ADS5125 /* CS2 FUNC MUX must be done before CS is enabled */
	lis     r4, (CONFIG_SYS_IOCTRL_ADDR)@h
	ori     r4, r4, (CONFIG_SYS_IOCTRL_ADDR)@l
	li	r5, IOCTRL_MUX_CS2
	stb	r5, IO_CTRL_LPC_AX03(r4)
/* change the pin muxing on PSC9 here in case it is being used as console*/
	li	r5, IOCTRL_MUX_PSC9
	stb	r5, IO_CTRL_I2C1_SCL(r4)
	stb	r5, IO_CTRL_I2C1_SDA(r4)

#endif
#endif

	/* r3: BOOTFLAG */
	mr	r3, r21
	
	bl	dram_init


	/* r3: BOOTFLAG */
	mr	r3, r21
	lis	r1, (CFG_INIT_RAM_ADDR + CFG_GBL_DATA_OFFSET)@h
	ori	r1, r1, (CFG_INIT_RAM_ADDR + CFG_GBL_DATA_OFFSET)@l

	/*copy sram to ddr*/
	bl	sram_to_ddr
	/* copy the full U-Boot into DDR  */
	/* and jump to it */
jump_uboot:
	SET_MEM_BASE(r10, nandload-CONFIG_SYS_NAND_BASE+CFG_LOADER_DDR_START)
	mtlr	r10
	isync
	blr

	/* NOTREACHED - nand_boot() does not return */
/*
 * This code initialises the machine,
 * it expects original MSR contents to be in r5
 */
cpu_early_init:
	/* Initialize machine status; enable machine check interrupt */
	/*-----------------------------------------------------------*/

	li	r3, MSR_KERNEL		/* Set ME and RI flags */
	rlwimi	r3, r5, 0, 25, 25	/* preserve IP bit */
#ifdef DEBUG
	rlwimi	r3, r5, 0, 21, 22	/* debugger might set SE, BE bits */
#endif
	mtmsr	r3
	SYNC
	mtspr	SRR1, r3		/* Mirror current MSR state in SRR1 */

	lis	r3, CFG_IMMR@h

	/* Disable the watchdog */
	/*----------------------*/
	lwz r4, SWCRR(r3)
	/*
	 * Check to see if it's enabled for disabling: once disabled by s/w
	 * it's not possible to re-enable it
	 */
	andi. r4, r4, 0x4
	beq 1f
	xor r4, r4, r4
	stw r4, SWCRR(r3)
1:

	/* Initialize the Hardware Implementation-dependent Registers */
	/* HID0 also contains cache control			*/
	/*------------------------------------------------------*/
	lis	r3, CFG_HID0_INIT@h
	ori	r3, r3, CFG_HID0_INIT@l
	SYNC
	mtspr	HID0, r3

	blr

dram_init:

	SET_MEM_BASE(r3, CFG_IMMR + IOCTL_BASE_ADDR)
	SET_REG32(r4, (IOCTRL_MUX_DDR<<24), IOCTL_MEM , r3)

	SET_MEM_BASE(r3, CFG_IMMR)
	SET_REG32(r4, CFG_DDR_BASE & 0xFFFFF000, DDR_LAW_BAR, r3)
	SET_REG32(r4, 0x0000001b, DDR_LAW_AR, r3)
	lwz     r0, DDR_LAW_AR(r3)
	isync

	SET_MEM_BASE(r3, CFG_IMMR + MDDRC_BASE_OFFSET)
	SET_REG32(r4, CFG_MDDRC_SYS_CFG_EN, DDR_SYS_CONFIG, r3)

	SET_REG32(r4, CFG_MDDRCGRP_PM_CFG1, DRAMPRIOM_PRIOMAN_CONFIG1, r3)
	SET_REG32(r4, CFG_MDDRCGRP_PM_CFG2, DRAMPRIOM_PRIOMAN_CONFIG2, r3)
	SET_REG32(r4, CFG_MDDRCGRP_HIPRIO_CFG, DRAMPRIOM_HIPRIO_CONFIG, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT0_MU, DRAMPRIOM_LUT_TABLE0_MAIN_UP, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT0_ML, DRAMPRIOM_LUT_TABLE0_MAIN_LOW, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT1_MU, DRAMPRIOM_LUT_TABLE1_MAIN_UP, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT1_ML, DRAMPRIOM_LUT_TABLE1_MAIN_LOW, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT2_MU, DRAMPRIOM_LUT_TABLE2_MAIN_UP, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT2_ML, DRAMPRIOM_LUT_TABLE2_MAIN_LOW, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT3_MU, DRAMPRIOM_LUT_TABLE3_MAIN_UP, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT3_ML, DRAMPRIOM_LUT_TABLE3_MAIN_LOW, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT4_MU, DRAMPRIOM_LUT_TABLE4_MAIN_UP, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT4_ML, DRAMPRIOM_LUT_TABLE4_MAIN_LOW, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT0_AU, DRAMPRIOM_LUT_TABLE0_ALT_UP, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT0_AL, DRAMPRIOM_LUT_TABLE0_ALT_LOW, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT1_AU, DRAMPRIOM_LUT_TABLE1_ALT_UP, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT1_AL, DRAMPRIOM_LUT_TABLE1_ALT_LOW, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT2_AU, DRAMPRIOM_LUT_TABLE2_ALT_UP, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT2_AL, DRAMPRIOM_LUT_TABLE2_ALT_LOW, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT3_AU, DRAMPRIOM_LUT_TABLE3_ALT_UP, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT3_AL, DRAMPRIOM_LUT_TABLE3_ALT_LOW, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT4_AU, DRAMPRIOM_LUT_TABLE4_ALT_UP, r3)
	SET_REG32(r4, CFG_MDDRCGRP_LUT4_AL, DRAMPRIOM_LUT_TABLE4_ALT_LOW, r3)

	/* Initialize MDDRC */
	SET_REG32(r4, CFG_MDDRC_SYS_CFG_EN, DDR_SYS_CONFIG, r3)
	SET_REG32(r4, CFG_MDDRC_TIME_CFG0, DDR_TIME_CONFIG0, r3)
	SET_REG32(r4, CFG_MDDRC_TIME_CFG1, DDR_TIME_CONFIG1, r3)
	SET_REG32(r4, CFG_MDDRC_TIME_CFG2, DDR_TIME_CONFIG2, r3)


	/* Initialize DDR */
	SET_REG32(r4, CFG_MICRON_NOP, DDR_COMMAND, r3)
	stw     r4, DDR_COMMAND(r3);
	stw     r4, DDR_COMMAND(r3);
	stw     r4, DDR_COMMAND(r3);
	stw     r4, DDR_COMMAND(r3);
	stw     r4, DDR_COMMAND(r3);
	stw     r4, DDR_COMMAND(r3);
	stw     r4, DDR_COMMAND(r3);
	stw     r4, DDR_COMMAND(r3);
	stw     r4, DDR_COMMAND(r3);
	stw     r4, DDR_COMMAND(r3);

	SET_REG32(r4, CFG_MICRON_PCHG_ALL, DDR_COMMAND, r3)
	SET_REG32(r5, CFG_MICRON_NOP, DDR_COMMAND, r3)
	SET_REG32(r4, CFG_MICRON_RFSH, DDR_COMMAND, r3)
	stw     r5, DDR_COMMAND(r3);
	SET_REG32(r4, CFG_MICRON_RFSH, DDR_COMMAND, r3)
	stw     r5, DDR_COMMAND(r3);
	SET_REG32(r4, CFG_MICRON_INIT_DEV_OP, DDR_COMMAND, r3)
	stw     r5, DDR_COMMAND(r3);
	SET_REG32(r4, CFG_MICRON_EM2, DDR_COMMAND, r3)
	stw     r5, DDR_COMMAND(r3);
	SET_REG32(r4, CFG_MICRON_PCHG_ALL, DDR_COMMAND, r3)
	SET_REG32(r4, CFG_MICRON_EM2, DDR_COMMAND, r3)
	SET_REG32(r4, CFG_MICRON_EM3, DDR_COMMAND, r3)
	SET_REG32(r4, CFG_MICRON_EN_DLL, DDR_COMMAND, r3)
	SET_REG32(r4, CFG_MICRON_INIT_DEV_OP, DDR_COMMAND, r3)
	SET_REG32(r4, CFG_MICRON_PCHG_ALL, DDR_COMMAND, r3)
	SET_REG32(r4, CFG_MICRON_RFSH, DDR_COMMAND, r3)
	SET_REG32(r4, CFG_MICRON_INIT_DEV_OP, DDR_COMMAND, r3)
	SET_REG32(r4, CFG_MICRON_OCD_DEFAULT, DDR_COMMAND, r3)
	SET_REG32(r4, CFG_MICRON_PCHG_ALL, DDR_COMMAND, r3)
	stw     r5, DDR_COMMAND(r3);

	/* Start MDDRC */
	SET_REG32(r4, CFG_MDDRC_TIME_CFG0_RUN, DDR_TIME_CONFIG0, r3)
	SET_REG32(r4, CFG_MDDRC_SYS_CFG_RUN, DDR_SYS_CONFIG, r3)
	isync
	blr

