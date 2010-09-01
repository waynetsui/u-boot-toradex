/*
 * (C) Copyright 2008-2009 DENX Software Engineering
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
 * ADS5125 board configuration file
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_ADS5125
/*
 * Memory map for the ADS5125 board:
 *
 * 0x0000_0000 - 0x00FF_FFFF	DDR RAM (16 MB)
 * 0x3000_0000 - 0x3001_FFFF	SRAM (128 KB)
 * 0x4000_0000 - 0x400F_FFFF	NAND FLASH CONTROLLER
 * 0x8000_0000 - 0x803F_FFFF	IMMR (4 MB)
 * 0x8200_0000 - 0x8200_001F	CPLD (32 B)
 * 0xFC00_0000 - 0xFFFF_FFFF	NOR Boot FLASH (64 MB)
 */

/*
 * High Level Configuration Options
 */
#define CONFIG_E300		1	/* E300 Family */
#define CONFIG_MPC512X		1	/* MPC512X family */
#define CONFIG_FSL_DIU_FB	1	/* FSL DIU */

#ifdef CONFIG_NAND_U_BOOT
#define CONFIG_SYS_NAND
#endif

#define CONFIG_CMD_UBI
#define CONFIG_RBTREE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_YAFFS2
#define	CONFIG_UBOOT_YAFFS2_START	0xc00000
#define	CONFIG_UBOOT_YAFFS2_SIZE		0x10c00000
#define	BOARD_TYPE_ADS5125	1
#define	BOARD_TYPE_5125_MPU	2
#define	BOARD_TYPE			BOARD_TYPE_5125_MPU

#define	HDMI_CHIP_SIL9034	1
#define	HDMI_CHIP_SIL9022A	2
#define	HDMI_CHIP_SELECT	HDMI_CHIP_SIL9022A
#if (HDMI_CHIP_SELECT==HDMI_CHIP_SIL9022A)
#define CONFIG_HDMI_CHIP_SIL9022A
#endif
#define CONFIG_MISC_INIT_R
/* video */
#undef CONFIG_VIDEO

#if defined(CONFIG_VIDEO)
#define CONFIG_CFB_CONSOLE
#define CONFIG_VGA_AS_SINGLE_DEVICE
#endif
/*
 * Enable Fast boot
 */
#define CONFIG_FASTBOOT

#define CONFIG_SYS_MPC512X_CLKIN	32768000 	/* in Hz Change by Cloudy Chen<chen_yunsong@mtcera.com> */

#define CONFIG_BOARD_EARLY_INIT_F		/* call board_early_init_f() */
/*
#define CONFIG_MISC_INIT_R
*/

#define CFG_SYS_IMMR		0x80000000
#define CFG_IMMR		0x80000000
#define CONFIG_SYS_IMMR		0x80000000
#define CONFIG_SYS_DIU_ADDR		(CONFIG_SYS_IMMR+0x2100)
#define CONFIG_SYS_IOCTRL_ADDR		(CONFIG_SYS_IMMR+0xA000)

#define CONFIG_SYS_MEMTEST_START	0x00200000      /* memtest region */
#define CONFIG_SYS_MEMTEST_END		0x00400000

/*
 * DDR Setup - manually set all parameters as there's no SPD etc.
 */
#define CONFIG_SYS_DDR_SIZE		256		/* MB */
#define CONFIG_SYS_DDR_BASE		0x00000000	/* DDR is system memory*/
#define CONFIG_SYS_SDRAM_BASE		CONFIG_SYS_DDR_BASE

#define CFG_DDR_BASE		CONFIG_SYS_DDR_BASE
/* DDR Controller Configuration
 *
 * SYS_CFG:
 *	[31:31]	MDDRC Soft Reset:	Diabled
 *	[30:30]	DRAM CKE pin:		Enabled
 *	[29:29]	DRAM CLK:		Enabled
 *	[28:28]	Command Mode:		Enabled (For initialization only)
 *	[27:25]	DRAM Row Select:	dram_row[15:0] = magenta_address[25:10]
 *	[24:21]	DRAM Bank Select:	dram_bank[1:0] = magenta_address[11:10]
 *	[20:19]	Read Test:		DON'T USE
 *	[18:18]	Self Refresh:		Enabled
 *	[17:17]	16bit Mode:		Disabled
 *	[16:13] Ready Delay:		2
 *	[12:12]	Half DQS Delay:		Disabled
 *	[11:11]	Quarter DQS Delay:	Disabled
 *	[10:08]	Write Delay:		2
 *	[07:07]	Early ODT:		Disabled
 *	[06:06]	On DIE Termination:	Disabled
 *	[05:05]	FIFO Overflow Clear:	DON'T USE here
 *	[04:04]	FIFO Underflow Clear:	DON'T USE here
 *	[03:03]	FIFO Overflow Pending:	DON'T USE here
 *	[02:02]	FIFO Underlfow Pending:	DON'T USE here
 *	[01:01]	FIFO Overlfow Enabled:	Enabled
 *	[00:00]	FIFO Underflow Enabled:	Enabled
 * TIME_CFG0
 *	[31:16]	DRAM Refresh Time:	0 CSB clocks
 *	[15:8]	DRAM Command Time:	0 CSB clocks
 *	[07:00]	DRAM Precharge Time:	0 CSB clocks
 * TIME_CFG1
 *	[31:26]	DRAM tRFC:
 *	[25:21]	DRAM tWR1:
 *	[20:17]	DRAM tWRT1:
 *	[16:11]	DRAM tDRR:
 *	[10:05]	DRAM tRC:
 *	[04:00]	DRAM tRAS:
 * TIME_CFG2
 *	[31:28]	DRAM tRCD:
 *	[27:23]	DRAM tFAW:
 *	[22:19]	DRAM tRTW1:
 *	[18:15]	DRAM tCCD:
 *	[14:10] DRAM tRTP:
 *	[09:05]	DRAM tRP:
 *	[04:00] DRAM tRPA
 */

#define MDDRC_SYS_CFG		0xfa804A00
#define MDDRC_SYS_CFG_RUN	0xea804A00
#define CFG_MDDRC_SYS_CFG_RUN	MDDRC_SYS_CFG_RUN

#define MDDRC_TIME_CFG1		0x68ec1189	//0x690e1189
#define CFG_MDDRC_TIME_CFG1	MDDRC_TIME_CFG1
#define MDDRC_TIME_CFG2		0x34310864	//0x34a90864
#define CFG_MDDRC_TIME_CFG2	MDDRC_TIME_CFG2
#define MDDRC_SYS_CFG_EN	0xF0000000
#define CFG_MDDRC_SYS_CFG_EN	MDDRC_SYS_CFG_EN
#define MDDRC_TIME_CFG0		0x00003c2d	//0x00003D2E
#define CFG_MDDRC_TIME_CFG0	MDDRC_TIME_CFG0
#define MDDRC_TIME_CFG0_RUN	0x06183c2d
#define CFG_MDDRC_TIME_CFG0_RUN		MDDRC_TIME_CFG0_RUN

#define DDR_ODT_150		0x40
#define DDR_ODT_75		0x04
#define DDR_ODT_50		0x44
#define DDR_OCD_DFLT_MASK	0x00000380
#define DDR_MRS_CAS(n)		(n << 4)
#define DDR_MRS_WR(n)		((n-1) << 9)
#define MDDRC_SYS_CFG_CLK_BIT       (1 << 29)
#define MDDRC_SYS_CFG_CKE_BIT       (1 << 30)

#define DDR_RFSH		0x01080000
#define CFG_MICRON_RFSH	DDR_RFSH
#define DDR_INIT_DEV_OP		0x01000432 /*0x01000002 | DDR_MRS_WR(4) | DDR_MRS_CAS(4)*/
#define CFG_MICRON_INIT_DEV_OP	DDR_INIT_DEV_OP
#define DDR_NOP			0x01380000
#define CFG_MICRON_NOP		DDR_NOP
#define DDR_PCHG_ALL		0x01100400
#define CFG_MICRON_PCHG_ALL	DDR_PCHG_ALL
#define DDR_EM2			0x01020000
#define CFG_MICRON_EM2		DDR_EM2
#define DDR_EM3			0x01030000
#define CFG_MICRON_EM3		DDR_EM3
#define DDR_EN_DLL		0x01010000
#define CFG_MICRON_EN_DLL	DDR_EN_DLL
#define DDR_RES_DLL		DDR_INIT_DEV_OP | 0x00000100
#define DDR_RFSH		0x01080000
#define DDR_OCD_DEFAULT		0x01010400 | DDR_OCD_DFLT_MASK | DDR_ODT_50
#define CFG_MICRON_OCD_DEFAULT	DDR_OCD_DEFAULT
#define DDR_OCD_EXIT		DDR_OCD_DEFAULT & ~DDR_OCD_DFLT_MASK

/* DDR Priority Manager Configuration */
#define MDDRCGRP_PM_CFG1	0x000777aa	//0x00077777
#define CFG_MDDRCGRP_PM_CFG1 	MDDRCGRP_PM_CFG1
#define MDDRCGRP_PM_CFG2	0x00000055	//0x00000000
#define CFG_MDDRCGRP_PM_CFG2 	MDDRCGRP_PM_CFG2
#define MDDRCGRP_HIPRIO_CFG	0x00000000	//0x00000001
#define CFG_MDDRCGRP_HIPRIO_CFG	MDDRCGRP_HIPRIO_CFG
#define MDDRCGRP_LUT0_MU	0x11111117	//0xFFEEDDCC
#define CFG_MDDRCGRP_LUT0_MU	MDDRCGRP_LUT0_MU
#define MDDRCGRP_LUT0_ML	0x7777777a	//0xBBAAAAAA
#define CFG_MDDRCGRP_LUT0_ML	MDDRCGRP_LUT0_ML
#define MDDRCGRP_LUT1_MU	0x444eeeee	//0x66666666
#define CFG_MDDRCGRP_LUT1_MU	MDDRCGRP_LUT1_MU
#define MDDRCGRP_LUT1_ML	0xeeeeeeee	//0x55555555
#define CFG_MDDRCGRP_LUT1_ML	MDDRCGRP_LUT1_ML
#define MDDRCGRP_LUT2_MU	0x44444444
#define CFG_MDDRCGRP_LUT2_MU	MDDRCGRP_LUT2_MU
#define MDDRCGRP_LUT2_ML	0x44444444
#define CFG_MDDRCGRP_LUT2_ML	MDDRCGRP_LUT2_ML
#define MDDRCGRP_LUT3_MU	0x55555555
#define CFG_MDDRCGRP_LUT3_MU	MDDRCGRP_LUT3_MU
#define MDDRCGRP_LUT3_ML	0x55555558
#define CFG_MDDRCGRP_LUT3_ML	MDDRCGRP_LUT3_ML
#define MDDRCGRP_LUT4_MU	0x11111111
#define CFG_MDDRCGRP_LUT4_MU	MDDRCGRP_LUT4_MU
#define MDDRCGRP_LUT4_ML	0x1111117c	//0x11111122
#define CFG_MDDRCGRP_LUT4_ML	MDDRCGRP_LUT4_ML
#define MDDRCGRP_LUT0_AU	0x33333377	//0xaaaaaaaa
#define CFG_MDDRCGRP_LUT0_AU	MDDRCGRP_LUT0_AU
#define MDDRCGRP_LUT0_AL	0x7777eeee //0xaaaaaaaa
#define CFG_MDDRCGRP_LUT0_AL	MDDRCGRP_LUT0_AL
#define MDDRCGRP_LUT1_AU		0x11111111	//0x66666666
#define CFG_MDDRCGRP_LUT1_AU 	MDDRCGRP_LUT1_AU
#define MDDRCGRP_LUT1_AL	0x11111111	//0x66666666
#define CFG_MDDRCGRP_LUT1_AL	MDDRCGRP_LUT1_AL
#define MDDRCGRP_LUT2_AU	0x11111111
#define CFG_MDDRCGRP_LUT2_AU 	MDDRCGRP_LUT2_AU
#define MDDRCGRP_LUT2_AL	0x11111111
#define CFG_MDDRCGRP_LUT2_AL	MDDRCGRP_LUT2_AL
#define MDDRCGRP_LUT3_AU	0x11111111
#define CFG_MDDRCGRP_LUT3_AU	MDDRCGRP_LUT3_AU
#define MDDRCGRP_LUT3_AL	0x11111111
#define CFG_MDDRCGRP_LUT3_AL	MDDRCGRP_LUT3_AL
#define MDDRCGRP_LUT4_AU	0x11111111
#define CFG_MDDRCGRP_LUT4_AU	MDDRCGRP_LUT4_AU
#define MDDRCGRP_LUT4_AL	0x11111111
#define CFG_MDDRCGRP_LUT4_AL	MDDRCGRP_LUT4_AL

#define IOCTRL_MUX_CS2	0x43	/* CS2 IO pin must be set in start.S */
#define IOCTRL_MUX_PSC9	0x23	/* ditto for UART1 in case it's Console  */
#define IOCTRL_GBOBE_ON	0x01

/*
 * NOR FLASH on the Local Bus
 */
 #if(BOARD_TYPE==BOARD_TYPE_5125_MPU)
#define CONFIG_SYS_NO_FLASH
 #define CONFIG_SYS_MAX_FLASH_SECT	256		/* max sectors per device */
 #define CONFIG_SYS_MAX_FLASH_BANKS	1		/* number of banks */
 #else
#undef CONFIG_BKUP_FLASH
#define CONFIG_SYS_FLASH_CFI				/* use the Common Flash Interface */
#define CONFIG_FLASH_CFI_DRIVER			/* use the CFI driver */
#ifdef CONFIG_BKUP_FLASH
/* Backup and main flash may not be the same size & same sector in the future */
#define CONFIG_SYS_FLASH_BASE		0xFE000000	/* start of FLASH   */
#define CONFIG_SYS_FLASH_SIZE		0x02000000	/* max flash size in bytes */
#else
#define CONFIG_SYS_FLASH_BASE		0xFE000000	/* start of FLASH   */
#define CONFIG_SYS_FLASH_SIZE		0x02000000	/* max flash size in bytes */
#endif
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE
#define CONFIG_SYS_MAX_FLASH_BANKS	1		/* number of banks */
#define CONFIG_SYS_FLASH_BANKS_LIST	{CONFIG_SYS_FLASH_BASE}
#define CONFIG_SYS_MAX_FLASH_SECT	256		/* max sectors per device */
#define CONFIG_SYS_FLASH_SIZE		0x02000000
#define CFG_FLASH_SIZE	CONFIG_SYS_FLASH_SIZE
#endif
#undef CONFIG_SYS_FLASH_CHECKSUM

/*
 * NAND FLASH
 * drivers/mtd/nand/mpc5121_mpc.c (rev 2 silicon/rev 4 boards only)
 */
#define RCWHR		0xe04
#define NAND_BOOT	(1<<5)
/*
#define CONFIG_NAND_SPL
*/
#define CONFIG_NAND_FSL_NFC
#ifdef CONFIG_NAND_FSL_NFC
#ifdef CONFIG_NAND_SPL
#define CONFIG_SYS_NAND_BASE		0xFFF00000
#define CONFIG_SYS_NAND_SPACE		0x100000
#define CFG_FLASH_BASE		CONFIG_SYS_NAND_BASE
#define	CFG_FLASH_SIZE		CONFIG_SYS_NAND_SPACE
#define CFG_NAND_U_BOOT_SIZE  (256 << 10)
#define	CFG_LOADER_DDR_START		0x00100000
#define CFG_NAND_U_BOOT_DST   (0x1000000)
#define CFG_NAND_U_BOOT_START (CFG_NAND_U_BOOT_DST+0x100) /* 1st 2K page of NAND is copied so *
					  * we need to offset by 0x800       */

#else
#define CONFIG_SYS_NAND_BASE		0x40000000
#endif
#define CONFIG_CMD_NAND 1

#define CFG_NAND_BASE CONFIG_SYS_NAND_BASE
/*
 * The flash on ADS5121 board is two flash chips in one package
 */
 #define CFG_NAND_BLOCK_SIZE	0x20000	/* 128K, 64 x 2K opages per block */
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define NAND_MAX_CHIPS		CONFIG_SYS_MAX_NAND_DEVICE
#define CONFIG_SYS_NAND_SELECT_DEVICE	1
/*
 * Configuration parameters for MPC5121 NAND driver
 */
 

#define CONFIG_FSL_NFC_WRITE_SIZE 4096
#define CONFIG_FSL_NFC_SPARE_SIZE 128

#define CONFIG_FSL_NFC_WIDTH 1

#define CONFIG_FSL_NFC_CHIPS 1

#ifndef __ASSEMBLY__
/*
 * ADS board as a custom chip select
 */
extern void ads5125_fsl_nfc_board_cs(int);
#define CONFIG_FSL_NFC_BOARD_CS_FUNC ads5125_fsl_nfc_board_cs
#endif /* __ASSEMBLY__ */
#endif /* CONFIG_NAND_FSL_NFC */

#if(BOARD_TYPE!=BOARD_TYPE_5125_MPU)
/*
 * CPLD registers area is really only 32 bytes in size, but the smallest possible LP
 * window is 64KB
 */
#define CONFIG_SYS_CPLD_BASE		0x82000000
#define CONFIG_SYS_CPLD_SIZE		0x00010000	/* 64 KB */
#define IS_CFG1_SWITCH		(in_8((u8 *)(CONFIG_SYS_CPLD_BASE + 0x13)) & 0x80)
#endif

#define CONFIG_SYS_SRAM_BASE		0x30000000
#define CONFIG_SYS_SRAM_SIZE		0x00008000	/* 32 KB */
#define CFG_SRAM_BASE		CONFIG_SYS_SRAM_BASE
#define CFG_SRAM_SIZE		CONFIG_SYS_SRAM_SIZE	/* 32 KB */


#define CONFIG_SYS_CS0_CFG		0x05059110	/* ALE active low, data size 2bytes */
#define CONFIG_SYS_CS2_CFG		0x05059010	/* ALE active low, data size 1byte  */
#define CONFIG_SYS_CS_ALETIMING	0x00000005	/* Use alternative CS timing for CS0 and CS2 */

/* Use SRAM for initial stack */
#define CONFIG_SYS_INIT_RAM_ADDR	CONFIG_SYS_SRAM_BASE		/* Initial RAM address */
#define CONFIG_SYS_INIT_RAM_END	CONFIG_SYS_SRAM_SIZE		/* End of used area in RAM */

#define CONFIG_SYS_GBL_DATA_SIZE	0x100			/* num bytes initial data */
#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_END - CONFIG_SYS_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

#define CONFIG_SYS_MONITOR_BASE	TEXT_BASE		/* Start of monitor */
#define CFG_MONITOR_BASE	CONFIG_SYS_MONITOR_BASE		/* Start of monitor */

#define CONFIG_SYS_MONITOR_LEN		(512 * 1024)		/* Reserve 256 kB for Mon */
/* Use SRAM for initial stack */
#define CFG_INIT_RAM_ADDR	CFG_SRAM_BASE		/* Initial RAM address */
#define CFG_INIT_RAM_END	CFG_SRAM_SIZE		/* End of used area in RAM */
#define CFG_GBL_DATA_SIZE	CONFIG_SYS_GBL_DATA_SIZE			/* num bytes initial data */
#define CFG_GBL_DATA_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET
#define CFG_INIT_SP_OFFSET	CONFIG_SYS_INIT_SP_OFFSET

#ifdef	CONFIG_FSL_DIU_FB
#define CONFIG_SYS_MALLOC_LEN		(6 * 1024 * 1024)	/* Reserved for malloc */
#define CONFIG_SYS_SPLASH_SIZE		(2 * 1024 * 1024)
#else
#define CONFIG_SYS_MALLOC_LEN		(512 * 1024)
#endif

/*
 * Serial Port
 */
#define CONFIG_CONS_INDEX     1
#undef CONFIG_SERIAL_SOFTWARE_FIFO

/*
 * Serial console configuration
 */
#define CONFIG_PSC_CONSOLE	1	/* console is on PSC9 */
#if CONFIG_PSC_CONSOLE != 1
#error CONFIG_PSC_CONSOLE must be 1
#endif
#define CONFIG_PSC_CONSOLE2	9	/* other console is on PSC1 */
#define CONFIG_BAUDRATE		115200	/* ... at 115200 bps */
#define CONFIG_SYS_BAUDRATE_TABLE  \
	{300, 600, 1200, 2400, 4800, 9600, 19200, 38400,115200}

#define CONSOLE_FIFO_TX_SIZE	FIFOC_PSC1_TX_SIZE
#define CONSOLE_FIFO_TX_ADDR	FIFOC_PSC1_TX_ADDR
#define CONSOLE_FIFO_RX_SIZE	FIFOC_PSC1_RX_SIZE
#define CONSOLE_FIFO_RX_ADDR	FIFOC_PSC1_RX_ADDR

#define CONFIG_CMDLINE_EDITING	1	/* add command line history	*/
/* Use the HUSH parser */
#define CONFIG_SYS_HUSH_PARSER
#ifdef  CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2 "> "
#endif


/* I2C */
#define CONFIG_HARD_I2C			/* defd in ads5121 I2C with hardware support */
#undef CONFIG_SOFT_I2C			/* so disable bit-banged I2C */
#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_I2C_CMD_TREE
#define CONFIG_SYS_I2C_SPEED		100000	/* I2C speed and slave address */
#define CONFIG_SYS_I2C_SLAVE		0x7F
#if 0
#define CONFIG_SYS_I2C_NOPROBES	{{0,0x69}}	/* Don't probe these addrs */
#endif

/*
 * EEPROM configuration
 */
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN		2	/* 16-bit EEPROM address */
#define CONFIG_SYS_I2C_EEPROM_ADDR		0x50	/* Atmel: AT24C32A-10TQ-2.7 */
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	5	/* 10ms of delay */
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	5	/* 32-Byte Page Write Mode */

/*
 * Configure on-board RTC
 */
#define CONFIG_RTC_M41T62			/* use M41T62 rtc via i2 */
#define CONFIG_SYS_I2C_RTC_ADDR		0x68	/* at address 0x68		*/

#endif /* defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C) */
/*
 * Ethernet configuration
 */
#define CONFIG_MPC512x_FEC	1
#define CONFIG_NET_MULTI
#define CONFIG_PHY_ADDR		0x1
#define CONFIG_PHY2_ADDR	0x1
#define CONFIG_MII		1	/* MII PHY management		*/
#define CONFIG_FEC_AN_TIMEOUT	1
#define CONFIG_HAS_ETH0
#define CONFIG_HAS_ETH1


/*
 * Environment
 */
 #if defined(CONFIG_NAND_U_BOOT)
#define CONFIG_ENV_IS_NOWHERE	1	/* Store ENV in memory only */
#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE-0x2000)
#define CONFIG_SYS_ENV_ADDR	CONFIG_ENV_ADDR
#define CONFIG_ENV_IS_IN_NAND	1
#define CONFIG_ENV_OFFSET		(512 * 1024)
#define CONFIG_ENV_SECT_SIZE	CFG_NAND_BLOCK_SIZE
#define CONFIG_SYS_ENV_SECT_SIZE	CONFIG_ENV_SECT_SIZE
#define CONFIG_ENV_SIZE		CONFIG_ENV_SECT_SIZE
#define CONFIG_SYS_ENV_SIZE	CONFIG_ENV_SIZE

#define CMD_SAVEENV
#define	CONFIG_ENV_START_PAGE		0x200

#define CONFIG_SYS_REDUNDAND_ENVIRONMENT

#define CONFIG_ENV_RANGE		(CONFIG_ENV_SECT_SIZE * 4)
#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + CONFIG_ENV_RANGE)
#elif !defined(CFG_RAMBOOT)
#define CONFIG_ENV_IS_IN_FLASH	1
/* This has to be a multiple of the Flash sector size */
#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN)
#define CONFIG_SYS_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN)
#define CONFIG_SYS_ENV_SIZE		0x2000
#define CONFIG_ENV_SIZE		0x2000
#ifdef CONFIG_BKUP_FLASH
#define CONFIG_ENV_SECT_SIZE	0x20000	/* one sector (128K) for env */
#define CONFIG_SYS_ENV_SECT_SIZE	0x20000	/* one sector (128K) for env */
#else
#define CONFIG_ENV_SECT_SIZE	0x20000	/* one sector (128K) for env */
#define CONFIG_SYS_ENV_SECT_SIZE	0x20000	/* one sector (128K) for env */
#endif
#else
#define CONFIG_ENV_IS_NOWHERE	1	/* Store ENV in memory only */
#define CONFIG_ENV_ADDR		(CFG_MONITOR_BASE - 0x1000)
#define CONFIG_ENV_SIZE		0x2000
#endif
/* Address and size of Redundant Environment Sector	*/
#define CONFIG_ENV_ADDR_REDUND	(CONFIG_SYS_ENV_ADDR + CONFIG_SYS_ENV_SECT_SIZE)
#define CONFIG_ENV_SIZE_REDUND	(CONFIG_SYS_ENV_SIZE)

#define CONFIG_LOADS_ECHO	1	/* echo on for serial download */
#define CONFIG_SYS_LOADS_BAUD_CHANGE	1	/* allow baudrate change */

#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NFS
#define CONFIG_CMD_PING
#define CONFIG_CMD_REGINFO
#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)
#define CONFIG_CMD_EEPROM
#define CONFIG_CMD_DATE
#define CONFIG_CMD_I2C
#endif
/*
 * Watchdog timeout = CONFIG_SYS_WATCHDOG_VALUE * 65536 / IPS clock.
 * For example, when IPS is set to 66MHz and CONFIG_SYS_WATCHDOG_VALUE is set
 * to 0xFFFF, watchdog timeouts after about 64s. For details refer
 * to chapter 36 of the MPC5125e Reference Manual.
 */
/* #define CONFIG_WATCHDOG */		/* enable watchdog */
#define CONFIG_SYS_WATCHDOG_VALUE 0xFFFF

 /*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory */
#define CONFIG_SYS_LOAD_ADDR	0x2000000	/* default load address */
#define CONFIG_SYS_PROMPT	"=> "		/* Monitor Command Prompt */

#ifdef CONFIG_CMD_KGDB
	#define CONFIG_SYS_CBSIZE	1024	/* Console I/O Buffer Size */
#else
	#define CONFIG_SYS_CBSIZE	256	/* Console I/O Buffer Size */
#endif


#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16) /* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16		/* max number of command args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size */
#define CONFIG_SYS_HZ		1000		/* decrementer freq: 1ms ticks */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ	(8 << 20)	/* Initial Memory map for Linux*/

/* Cache Configuration */
#define CONFIG_SYS_DCACHE_SIZE		32768
#define CONFIG_SYS_CACHELINE_SIZE	32
#ifdef CONFIG_CMD_KGDB
#define CONFIG_SYS_CACHELINE_SHIFT	5	/*log base 2 of the above value*/
#endif

#define CONFIG_SYS_HID0_INIT   HID0_ICE | HID0_ICFI
#define CFG_HID0_INIT CONFIG_SYS_HID0_INIT
#define CONFIG_SYS_HID0_FINAL  HID0_ENABLE_MACHINE_CHECK | HID0_ICE
#define CONFIG_SYS_HID2	HID2_HBE

#define CONFIG_HIGH_BATS	1	/* High BATs supported */

/*
 * Internal Definitions
 *
 * Boot Flags
 */
#define BOOTFLAG_COLD		0x01	/* Normal Power-On: Boot from FLASH */
#define BOOTFLAG_WARM		0x02	/* Software reboot */

#ifdef CONFIG_CMD_KGDB
#define CONFIG_KGDB_BAUDRATE	230400	/* speed of kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use */
#endif

/*
 * Environment Configuration
 */
#define CONFIG_TIMESTAMP

#define CONFIG_HOSTNAME		ads5125
#define CONFIG_BOOTFILE		ads5125/uImage
#define CONFIG_ROOTPATH		/opt/eldk/pcc_6xx

#define CONFIG_LOADADDR		400000	/* default location for tftp and bootm */

#define CONFIG_BOOTDELAY	2	/* -1 disables auto-boot */
#undef  CONFIG_BOOTARGS			/* the boot command will set bootargs */

#define CONFIG_BAUDRATE		115200

#define CONFIG_PREBOOT	"echo;"	\
	"echo Type \\\"run flash_nfs\\\" to mount root filesystem over NFS;" \
	"echo"

#define	CONFIG_EXTRA_ENV_SETTINGS					\	
	"ethaddr=AA:BB:CC:DD:EE:FF\0"				\
	"ramdiskfile=ads5125/uRamdisk\0"				\
	"fdtfile=ads5125/ads5125.dtb\0"					\
	"u-boot=ads5125/u-boot.bin\0"					\
	"netdev=eth0\0"							\
	"ipaddr=192.168.10.205\0"						\
	"consdev=tty0\0"						\
	"serverip=192.168.10.74\0"						\
	"fdtaddr=4000000\0"						\
	"fdtfile=mpc5125-twr.dtb\0"						\
	"kernel_name=vmlinux-twr-5125.bin\0"						\
	"consoledev=ttyPSC0\0"						\
	"flash_kernel=0x300\0"						\
	"flash_dtb=0xb00\0"						\
	"nandboot=setenv bootargs root=/dev/mtdblock6 rw rootfstype=yaffs2 console=$consdev,$baudrate;"\
	"nand_r $kernel_loader_addr $flash_kernel 0x400000;nand_r $fdt_loader_addr $flash_dtb 0x3000;bootm $kernel_loader_addr - $fdt_loader_addr\0" \	
	"fdt_name=mpc5125-twr.dtb\0"						\
	"ramdisk_name=rootfs.ext2.gz.uboot-common\0"						\
	"ramdisk_flash_addr=0xc00\0"							\
	"kernel_loader_addr=0x2000000\0"						\
	"fdt_loader_addr=0x2800000\0"							\
	"ramdisk_loader_addr=0x3000000\0"						\
	"nand_ramboot=setenv bootargs root=/dev/ram rw console=$consoledev,$baudrate;"\
	"nand_r 0x2000000 $flash_kernel 0x400000;nand_r 0x2800000 $flash_dtb 0x3000;nand_r 0x3000000 0xc00  0x500000;;bootm $kernel_loader_addr $ramdisk_loader_addr $fdt_loader_addr\0"							\
	"uboot_name=u-boot-second-usb.bin\0"						\
	"uboot_name_first=u-boot-first-usb.bin\0"						\
	"uboot_size=0x60000\0"						\
	"uboot_update=tftp 0x1000000 u-boot-spl-2k.bin;nand_e 0x00 0x01;nand_loader 0x1000000 0x00 0x800;"\
	"tftp 0x1000000 $uboot_name_first ;nand_w 0x1000000 0x8 0x60000;"		\
	"tftp 0x1000000 $uboot_name ;nand_e 0x100 0x101;nand_w 0x1000000 0x100 0x60000\0"							\
	"nfsargs=setenv bootargs root=/dev/nfs rw "			\
		"nfsroot=${serverip}:${rootpath}\0"			\
	"ramargs=setenv bootargs root=/dev/ram rw\0"			\
	"addip=setenv bootargs ${bootargs} "				\
		"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}"	\
		":${hostname}:${netdev}:off panic=1\0"			\
	"addtty=setenv bootargs ${bootargs} "				\
		"console=${consdev},${baudrate}\0"			\	
	"mqx_name=extram_d.bin\0"						\
	"mqx_size=0xa00000\0"							\
	"mqx_addr=0x10000\0"						\
	"mqx_flash_addr_s=0x80000\0"						\
	"mqx_flash_addr_e=0x863ff\0"						\
	"kernel_rootfs_update=tftp 0x3000000  $fdt_name;nand_e $flash_dtb 0xb01;nand_w 0x3000000 $flash_dtb 0x3000;tftp 0x3000000 $kernel_name;nand_e $flash_kernel 0xaff;nand_w 0x3000000 $flash_kernel 0x400000;tftp 0x3000000 $ramdisk_name;nand_e 0xc00 0x13ff;nand_w 0x3000000 0xc00 0x500000\0"						\
	"mqx_update=tftp $mqx_addr $mqx_name;nand_e $mqx_flash_addr_s $mqx_flash_addr_e;nand_w $mqx_addr $mqx_flash_addr_s $mqx_size\0"\
	"mqxboot=nand_r $mqx_addr $mqx_flash_addr_s $mqx_size;go $mqx_addr\0"		\
	""

#define CONFIG_BOOTCOMMAND	"run nandboot"

#define CONFIG_OF_LIBFDT	1
#define CONFIG_OF_BOARD_SETUP	1
#define CONFIG_OF_SUPPORT_OLD_DEVICE_TREES	1

#define OF_CPU			"PowerPC,5125@0"
#define OF_SOC_COMPAT		"fsl,mpc5125-immr"
#define OF_TBCLK		(bd->bi_busfreq / 4)
#define OF_STDOUT_PATH		"/soc@80000000/serial@11300"

#endif	/* __CONFIG_H */
