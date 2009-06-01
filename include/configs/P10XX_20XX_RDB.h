/*
 * Copyright (C) 2009 Freescale Semiconductor, Inc. All rights reserved.
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

/*
 * P10XX P20XX RDB board configuration file
 * This file addresses a set of Freescale SOCs P2020,P1020,P1011,P1012
 *
 */
#ifndef __CONFIG_H
#define __CONFIG_H

/* High Level Configuration Options */
#define CONFIG_P2020		1
#define CONFIG_P1020		1
#define CONFIG_P1021		1
#define CONFIG_P1011		1
#define CONFIG_P1012		1
#define CONFIG_BOOKE		1	/* BOOKE */
#define CONFIG_E500		1	/* BOOKE e500 family */
#define CONFIG_MPC85xx		1	/* MPC8540/60/55/41/48/P1020/P2020, etc */
#define CONFIG_FSL_ELBC	1	/* Enable eLBC Support */
#define CONFIG_PCI		1	/* Enable PCI/PCIE */
#define CONFIG_PCIE1		1	/* PCIE controler 1 (slot 1) */
#define CONFIG_PCIE2		1	/* PCIE controler 2 (slot 2) */
#define CONFIG_FSL_PCI_INIT	1	/* Use common FSL init code */
#define CONFIG_FSL_PCIE_RESET	1	/* need PCIe reset errata */
#define CONFIG_SYS_PCI_64BIT	1	/* enable 64-bit PCI resources */
#define CONFIG_FSL_LAW		1	/* Use common FSL init code */
#define CONFIG_TSEC_ENET		/* tsec ethernet support */
#define CONFIG_ENV_OVERWRITE

/*
 * eSPI - Enhanced SPI
 */
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_SPANSION

#define CONFIG_HARD_SPI
#define CONFIG_FSL_ESPI

#define CONFIG_CMD_SF
#define CONFIG_SF_DEFAULT_SPEED 10000000
#define CONFIG_SF_DEFAULT_MODE 0

#ifndef __ASSEMBLY__
extern unsigned long get_board_sys_clk(unsigned long dummy);
#endif
#define CONFIG_SYS_CLK_FREQ	get_board_sys_clk(0) /*sysclk for P10xx20xxRDB */
//#define CONFIG_SYS_CLK_FREQ	66666666 /*sysclk for P10xx20xxRDB */
#define CONFIG_DDR_CLK_FREQ	66666666 /* DDRCLK on P10xx_p20xx RDB */

/*
 * These can be toggled for performance analysis, otherwise use default.
 */
#define CONFIG_L2_CACHE			/* toggle L2 cache */
#define CONFIG_BTB			/* toggle branch predition */
#define CONFIG_ADDR_STREAMING		/* toggle addr streaming */

#define CONFIG_BOARD_EARLY_INIT_F	1	/* Call board_pre_init */

#define CONFIG_ENABLE_36BIT_PHYS	1

#ifdef CONFIG_PHYS_64BIT
#define CONFIG_ADDR_MAP			1
#define CONFIG_SYS_NUM_ADDR_MAP		16	/* number of TLB1 entries */
#endif

#define CONFIG_SYS_MEMTEST_START	0x00000000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x1fffffff
#define CONFIG_PANIC_HANG	/* do not reset board on panic */

/*
 * Base addresses -- Note these are effective addresses where the
 * actual resources get mapped (not physical addresses)
 */
#define CONFIG_SYS_CCSRBAR_DEFAULT	0xff700000	/* CCSRBAR Default */
#define CONFIG_SYS_CCSRBAR		0xffe00000	/* relocated CCSRBAR */
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_CCSRBAR_PHYS		0xfffe00000ull	/* physical addr of CCSRBAR */
#else
#define CONFIG_SYS_CCSRBAR_PHYS	CONFIG_SYS_CCSRBAR	/* physical addr of CCSRBAR */
#endif
#define CONFIG_SYS_IMMR		CONFIG_SYS_CCSRBAR	/* PQII uses CONFIG_SYS_IMMR */

#define CONFIG_SYS_PCIE2_ADDR		(CONFIG_SYS_CCSRBAR+0x9000)
#define CONFIG_SYS_PCIE1_ADDR		(CONFIG_SYS_CCSRBAR+0xa000)

/* DDR Setup */
#define CONFIG_FSL_DDR2
#undef CONFIG_FSL_DDR_INTERACTIVE
#undef CONFIG_SPD_EEPROM		/* Use SPD EEPROM for DDR setup */
/*							on the board */
#undef CONFIG_DDR_DLL

#define CONFIG_MEM_INIT_VALUE	0xDeadBeef

#define CONFIG_SYS_SDRAM_SIZE		get_board_ddr_size(0)	
#define CONFIG_SYS_DDR_SDRAM_BASE	0x00000000
#define CONFIG_SYS_SDRAM_BASE		CONFIG_SYS_DDR_SDRAM_BASE

#define CONFIG_NUM_DDR_CONTROLLERS	1
#define CONFIG_DIMM_SLOTS_PER_CTLR	1
#define CONFIG_CHIP_SELECTS_PER_CTRL	1

/* These are used when DDR doesn't use SPD.  */
/* DDR2 at 533MHz, 512MB */
#define CONFIG_SYS_DDR_CS0_BNDS		0x0000003F
#define CONFIG_SYS_DDR_CS0_CONFIG	0x80014202	/* Enable, no interleaving */
#define CONFIG_SYS_DDR_CS0_CONFIG_2	0x00000000

#define CONFIG_SYS_DDR_TIMING_3_REVA		0x00020000
#define CONFIG_SYS_DDR_TIMING_0_REVA		0x00260802
#define CONFIG_SYS_DDR_TIMING_1_REVA		0x4c47c432
#define CONFIG_SYS_DDR_TIMING_2_REVA		0x0f9848ce
#define CONFIG_SYS_DDR_TIMING_4_REVA		0x00000000
#define CONFIG_SYS_DDR_TIMING_5_REVA		0x00000000
#define CONFIG_SYS_DDR_CLK_CTRL_REVA		0x02800000
#define CONFIG_SYS_DDR_MODE_1_REVA		0x00040642
#define CONFIG_SYS_DDR_MODE_2_REVA		0x00000000
#define CONFIG_SYS_DDR_INTERVAL_REVA		0x08200100
#define CONFIG_SYS_DDR_CONTROL_REVA		0x43000000	/* Type = DDR2 */
#define CONFIG_SYS_DDR_CONTROL2_REVA		0x24401000

#define CONFIG_SYS_DDR_TIMING_3_533_REVB		0x00020000
#define CONFIG_SYS_DDR_TIMING_0_533_REVB		0x00260802
#define CONFIG_SYS_DDR_TIMING_1_533_REVB		0x4c47c432
#define CONFIG_SYS_DDR_TIMING_2_533_REVB		0x0f9848ce
#define CONFIG_SYS_DDR_TIMING_4_533_REVB		0x00000000
#define CONFIG_SYS_DDR_TIMING_5_533_REVB		0x00000000
#define CONFIG_SYS_DDR_CLK_CTRL_533_REVB		0x02800000
#define CONFIG_SYS_DDR_MODE_1_533_REVB		0x00040642
#define CONFIG_SYS_DDR_MODE_2_533_REVB		0x00000000
#define CONFIG_SYS_DDR_INTERVAL_533_REVB		0x08200100
#define CONFIG_SYS_DDR_CONTROL_533_REVB		0x43000000	/* Type = DDR2 */
#define CONFIG_SYS_DDR_CONTROL2_533_REVB		0x24401000


#define CONFIG_SYS_DDR_TIMING_3_400_REVB		0x00010000
#define CONFIG_SYS_DDR_TIMING_0_400_REVB		0x00260802
#define CONFIG_SYS_DDR_TIMING_1_400_REVB		0x39352322
#define CONFIG_SYS_DDR_TIMING_2_400_REVB		0x1f9048ca
#define CONFIG_SYS_DDR_TIMING_4_400_REVB		0x00000000
#define CONFIG_SYS_DDR_TIMING_5_400_REVB		0x00000000
#define CONFIG_SYS_DDR_CLK_CTRL_400_REVB		0x02800000
#define CONFIG_SYS_DDR_MODE_1_400_REVB		0x00480432
#define CONFIG_SYS_DDR_MODE_2_400_REVB		0x00000000
#define CONFIG_SYS_DDR_INTERVAL_400_REVB		0x06180100
#define CONFIG_SYS_DDR_CONTROL_400_REVB		0x43000000	/* Type = DDR2 */
#define CONFIG_SYS_DDR_CONTROL2_400_REVB		0x24401000


#define CONFIG_SYS_DDR_TIMING_3_667_REVB		0x00030000
#define CONFIG_SYS_DDR_TIMING_0_667_REVB		0x55770802
#define CONFIG_SYS_DDR_TIMING_1_667_REVB		0x5f599543
#define CONFIG_SYS_DDR_TIMING_2_667_REVB		0x0fa074d1
#define CONFIG_SYS_DDR_TIMING_4_667_REVB		0x00000000
#define CONFIG_SYS_DDR_TIMING_5_667_REVB		0x00000000
#define CONFIG_SYS_DDR_CLK_CTRL_667_REVB		0x02800000
#define CONFIG_SYS_DDR_MODE_1_667_REVB		0x00040852
#define CONFIG_SYS_DDR_MODE_2_667_REVB		0x00000000
#define CONFIG_SYS_DDR_INTERVAL_667_REVB		0x0a280100
#define CONFIG_SYS_DDR_CONTROL_667_REVB		0x43000000	/* Type = DDR2 */
#define CONFIG_SYS_DDR_CONTROL2_667_REVB		0x24401000

#define CONFIG_SYS_DDR_TIMING_3_800_REVB		0x00040000
#define CONFIG_SYS_DDR_TIMING_0_800_REVB		0x55770802
#define CONFIG_SYS_DDR_TIMING_1_800_REVB		0x6f6b6543
#define CONFIG_SYS_DDR_TIMING_2_800_REVB		0x0fa074d1
#define CONFIG_SYS_DDR_TIMING_4_800_REVB		0x00000000
#define CONFIG_SYS_DDR_TIMING_5_800_REVB		0x00000000
#define CONFIG_SYS_DDR_CLK_CTRL_800_REVB		0x02000000
#define CONFIG_SYS_DDR_MODE_1_800_REVB			0x00440862
#define CONFIG_SYS_DDR_MODE_2_800_REVB			0x00000000
#define CONFIG_SYS_DDR_INTERVAL_800_REVB		0x0a280100
#define CONFIG_SYS_DDR_CONTROL_800_REVB			0x43000000	/* Type = DDR2 */
#define CONFIG_SYS_DDR_CONTROL2_800_REVB		0x24401000


#define CONFIG_SYS_DDR_DATA_INIT	0xdeadbeef
#define CONFIG_SYS_DDR_OCD_CTRL		0x00000000
#define CONFIG_SYS_DDR_OCD_STATUS	0x00000000

#define CONFIG_SYS_DDR_ERR_INT_EN	0x0000000d
#define CONFIG_SYS_DDR_ERR_DIS		0x00000000
#define CONFIG_SYS_DDR_SBE		0x00FF0000

#define CONFIG_SYS_DDR_TLB_START 9

#undef CONFIG_CLOCKS_IN_MHZ

/*
 * Memory map
 *
 * 0x0000_0000	0x1fff_ffff	DDR			512MB cacheablen
 * 0x2000_0000	0x5fff_ffff	PCI Express Mem		1G non-cacheable
 * 0x6100_0000	0x63ff_ffff	PCI IO range		4M non-cacheable
 *
 * Localbus cacheable (TBD)
 * 0xXXXX_XXXX	0xXXXX_XXXX	SRAM			YZ M Cacheable
 *
 * Localbus non-cacheable
 * 0xef00_0000	0xefff_ffff	FLASH			16M non-cacheable
 * 0xffa0_0000	0xffaf_ffff	NAND			1M non-cacheable
 * 0xffc0_0000	0xffcf_ffff	VSC7385 switch		1M non-cacheable
 * 0xffd0_0000	0xffd0_3fff	L1 for stack		16K Cacheable TLB0
 * 0xffe0_0000	0xffef_ffff	CCSR			1M non-cacheable
 */

/*
 * Local Bus Definitions
 */
#define CONFIG_SYS_FLASH_BASE		0xef000000	/* start of FLASH 16M */

#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_FLASH_BASE_PHYS	0xfef000000ull
#else
#define CONFIG_SYS_FLASH_BASE_PHYS	CONFIG_SYS_FLASH_BASE
#endif

#define CONFIG_FLASH_BR_PRELIM	(BR_PHYS_ADDR(CONFIG_SYS_FLASH_BASE_PHYS) | BR_PS_16 | BR_V)
#define CONFIG_FLASH_OR_PRELIM		0xff000ff7

#define CONFIG_SYS_FLASH_BANKS_LIST	{CONFIG_SYS_FLASH_BASE}
#define CONFIG_SYS_FLASH_QUIET_TEST
#define CONFIG_FLASH_SHOW_PROGRESS 45 /* count down from 45/5: 9..1 */

#define CONFIG_SYS_MAX_FLASH_BANKS	1		/* number of banks */
#define CONFIG_SYS_MAX_FLASH_SECT	128		/* sectors per device */
#undef	CONFIG_SYS_FLASH_CHECKSUM
#define CONFIG_SYS_FLASH_ERASE_TOUT	60000		/* Flash Erase Timeout (ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT	500		/* Flash Write Timeout (ms) */

#define CONFIG_SYS_MONITOR_BASE	TEXT_BASE	/* start of monitor */

#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_SYS_FLASH_EMPTY_INFO
#define CONFIG_SYS_FLASH_AMD_CHECK_DQ7

#define CONFIG_BOARD_EARLY_INIT_R	/* call board_early_init_r function */


#if !defined(CONFIG_SDCARD_U_BOOT)
#define CONFIG_SYS_INIT_RAM_LOCK	1
#define CONFIG_SYS_INIT_RAM_ADDR      0xffd00000	/* stack in RAM */
#define CONFIG_SYS_INIT_RAM_END	0x00004000	/* End of used area in RAM */
#else
#undef  CONFIG_SYS_INIT_RAM_LOCK
#define CONFIG_SYS_INIT_RAM_ADDR	0x07d00000	/* unused memory region */
#define CONFIG_SYS_INIT_RAM_END		0x4000		/* we have SDRAM initialized */
#endif

#define CONFIG_SYS_GBL_DATA_SIZE	128	/* num bytes initial data */
#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_END - CONFIG_SYS_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

#define CONFIG_SYS_MONITOR_LEN		(256 * 1024) /* Reserve 256 kB for Mon */
#define CONFIG_SYS_MALLOC_LEN		(1024 * 1024)	/* Reserved for malloc */

#ifdef CONFIG_SDCARD_U_BOOT
#define CONFIG_SYS_RESERVED_LAW0	0
#endif

#ifdef CONFIG_NAND_SPL
#define CONFIG_SYS_NAND_BASE		0xfff00000
#else
#define CONFIG_SYS_NAND_BASE		0xffa00000
#endif
#define CONFIG_SYS_NAND_BASE_PHYS	CONFIG_SYS_NAND_BASE
#define CONFIG_SYS_NAND_BASE_LIST	{CONFIG_SYS_NAND_BASE}
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define NAND_MAX_CHIPS	1
#define CONFIG_MTD_NAND_VERIFY_WRITE
#define CONFIG_CMD_NAND	1
#define CONFIG_NAND_FSL_ELBC	1
#define CONFIG_SYS_NAND_BLOCK_SIZE	(16 * 1024)
#define CONFIG_SYS_NAND_SPL_SIZE     0x1000
#define CONFIG_SYS_NAND_U_BOOT_SIZE  ((512 << 10) + CONFIG_SYS_NAND_SPL_SIZE)
#define CONFIG_SYS_NAND_U_BOOT_DST   (0x01000000 - CONFIG_SYS_NAND_SPL_SIZE)
#define CONFIG_SYS_NAND_U_BOOT_START 0x01000000
#define CONFIG_SYS_NAND_U_BOOT_OFFS  (0)
#define CONFIG_SYS_NAND_U_BOOT_RELOC 0x00010000

#ifdef CONFIG_NAND_U_BOOT
	#define CONFIG_SYS_RESERVED_LAW1  1
#endif

/* NAND flash config */
#define CONFIG_NAND_BR_PRELIM	(CONFIG_SYS_NAND_BASE_PHYS \
				| (2<<BR_DECC_SHIFT)	/* Use HW ECC */ \
				| BR_PS_8		/* Port Size = 8 bit */ \
				| BR_MS_FCM		/* MSEL = FCM */ \
				| BR_V)			/* valid */

#define CONFIG_NAND_OR_PRELIM	(0xFFF80000		/* length 32K */ \
				| OR_FCM_CSCT \
				| OR_FCM_CST \
				| OR_FCM_CHT \
				| OR_FCM_SCY_1 \
				| OR_FCM_TRLX \
				| OR_FCM_EHTR)

#ifdef CONFIG_NAND_U_BOOT
  #define CONFIG_SYS_BR0_PRELIM  CONFIG_NAND_BR_PRELIM  /* NAND Base Address */
  #define CONFIG_SYS_OR0_PRELIM  CONFIG_NAND_OR_PRELIM  /* NAND Options */
  #define CONFIG_SYS_BR1_PRELIM  CONFIG_FLASH_BR_PRELIM  /* NOR Base Address */
  #define CONFIG_SYS_OR1_PRELIM  CONFIG_FLASH_OR_PRELIM  /* NOR Options */
#else
  #define CONFIG_SYS_BR0_PRELIM  CONFIG_FLASH_BR_PRELIM  /* NOR Base Address */
  #define CONFIG_SYS_OR0_PRELIM  CONFIG_FLASH_OR_PRELIM  /* NOR Options */
  #define CONFIG_SYS_BR1_PRELIM  CONFIG_NAND_BR_PRELIM  /* NAND Base Address */
  #define CONFIG_SYS_OR1_PRELIM  CONFIG_NAND_OR_PRELIM  /* NAND Options */
#endif

#define CONFIG_SYS_VSC7385_BASE	0xffb00000

#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_VSC7385_BASE_PHYS	0xfffb00000ull
#else
#define CONFIG_SYS_VSC7385_BASE_PHYS	CONFIG_SYS_VSC7385_BASE
#endif

#define CONFIG_SYS_BR2_PRELIM	(CONFIG_SYS_VSC7385_BASE | BR_PS_8 | BR_V) /* VSC7385 switch */
#define CONFIG_SYS_OR2_PRELIM	(OR_AM_128KB | OR_GPCM_CSNT | OR_GPCM_XACS | \
				OR_GPCM_SCY_15 | OR_GPCM_SETA | OR_GPCM_TRLX | \
				OR_GPCM_EHTR | OR_GPCM_EAD)

/* Serial Port - controlled on board with jumper J8
 * open - index 2
 * shorted - index 1
 */
#define CONFIG_CONS_INDEX	1
//#define CONFIG_CONS_INDEX	2
#undef	CONFIG_SERIAL_SOFTWARE_FIFO
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	1
#define CONFIG_SYS_NS16550_CLK		get_bus_freq(0)

#define CONFIG_SERIAL_MULTI	1 /* Enable both serial ports */
#define CONFIG_SYS_CONSOLE_IS_IN_ENV	/* determine from environment */

#define CONFIG_SYS_BAUDRATE_TABLE	\
	{300, 600, 1200, 2400, 4800, 9600, 19200, 38400,115200}

#define CONFIG_SYS_NS16550_COM1	(CONFIG_SYS_CCSRBAR+0x4500)
#define CONFIG_SYS_NS16550_COM2	(CONFIG_SYS_CCSRBAR+0x4600)

/* Use the HUSH parser */
#define CONFIG_SYS_HUSH_PARSER
#ifdef	CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2 "> "
#endif

/*
 * Pass open firmware flat tree
 */
#define CONFIG_OF_LIBFDT		1
#define CONFIG_OF_BOARD_SETUP		1
#define CONFIG_OF_STDOUT_VIA_ALIAS	1

#define CONFIG_SYS_64BIT_VSPRINTF	1
#define CONFIG_SYS_64BIT_STRTOUL	1

/* new uImage format support */
#define CONFIG_FIT		1
#define CONFIG_FIT_VERBOSE	1 /* enable fit_format_{error,warning}() */

/* I2C */
#define CONFIG_FSL_I2C		/* Use FSL common I2C driver */
#define CONFIG_HARD_I2C		/* I2C with hardware support */
#undef	CONFIG_SOFT_I2C		/* I2C bit-banged */
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_I2C_CMD_TREE
#define CONFIG_SYS_I2C_SPEED		400000	/* I2C speed and slave address */
#define CONFIG_SYS_I2C_EEPROM_ADDR	0x57
#define CONFIG_SYS_I2C_SLAVE		0x7F
#define CONFIG_SYS_I2C_NOPROBES	{{0,0x29}}/* Don't probe these addrs */
#define CONFIG_SYS_I2C_OFFSET		0x3000
#define CONFIG_SYS_I2C2_OFFSET		0x3100

/*
 * I2C2 EEPROM
 */
#define CONFIG_ID_EEPROM
#ifdef CONFIG_ID_EEPROM
#define CONFIG_SYS_I2C_EEPROM_NXID
#endif
#define CONFIG_SYS_I2C_EEPROM_ADDR	0x57
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN 1
#define CONFIG_SYS_EEPROM_BUS_NUM	1

//#define CONFIG_RTC_DS1337
/*
 * General PCI
 * Memory space is mapped 1-1, but I/O space must start from 0.
 */

/* controller 2, Slot 2, tgtid 2, Base address 9000 */
#define CONFIG_SYS_PCIE2_MEM_VIRT	0xa0000000
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_PCIE2_MEM_BUS	0xc0000000
#define CONFIG_SYS_PCIE2_MEM_PHYS	0xc20000000ull
#else
#define CONFIG_SYS_PCIE2_MEM_BUS	0xa0000000
#define CONFIG_SYS_PCIE2_MEM_PHYS	0xa0000000
#endif
#define CONFIG_SYS_PCIE2_MEM_SIZE	0x20000000	/* 512M */
#define CONFIG_SYS_PCIE2_IO_VIRT	0xffc20000
#define CONFIG_SYS_PCIE2_IO_BUS	0x00000000
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_PCIE2_IO_PHYS	0xfffc20000ull
#else
#define CONFIG_SYS_PCIE2_IO_PHYS	0xffc20000
#endif
#define CONFIG_SYS_PCIE2_IO_SIZE	0x00010000	/* 64k */

/* controller 1, Slot 1, tgtid 1, Base address a000 */
#define CONFIG_SYS_PCIE1_MEM_VIRT	0xc0000000
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_PCIE1_MEM_BUS	0xc0000000
#define CONFIG_SYS_PCIE1_MEM_PHYS	0xc40000000ull
#else
#define CONFIG_SYS_PCIE1_MEM_BUS	0xc0000000
#define CONFIG_SYS_PCIE1_MEM_PHYS	0xc0000000
#endif
#define CONFIG_SYS_PCIE1_MEM_SIZE	0x20000000	/* 512M */
#define CONFIG_SYS_PCIE1_IO_VIRT	0xffc30000
#define CONFIG_SYS_PCIE1_IO_BUS	0x00000000
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_PCIE1_IO_PHYS	0xfffc30000ull
#else
#define CONFIG_SYS_PCIE1_IO_PHYS	0xffc30000
#endif
#define CONFIG_SYS_PCIE1_IO_SIZE	0x00010000	/* 64k */

#if defined(CONFIG_PCI)
#define CONFIG_NET_MULTI
#define CONFIG_PCI_PNP			/* do pci plug-and-play */

#undef CONFIG_EEPRO100
#undef CONFIG_TULIP
#undef CONFIG_RTL8139

#ifdef CONFIG_RTL8139
/* This macro is used by RTL8139 but not defined in PPC architecture */
#define KSEG1ADDR(x)		(x)
#define _IO_BASE	0x00000000
#endif


#define CONFIG_PCI_SCAN_SHOW		/* show pci devices on startup */
#define CONFIG_DOS_PARTITION

#endif	/* CONFIG_PCI */

#define CONFIG_MISC_INIT_R

/************************************************************
 * USB support
 ************************************************************/

#define CONFIG_CMD_FAT
#define CONFIG_CMD_USB
#define CONFIG_USB_STORAGE
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_FSL
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_SYS_USB_EHCI_REGS_BASE		(CONFIG_SYS_CCSRBAR+0x22000)

#if defined(CONFIG_TSEC_ENET)

#ifndef CONFIG_NET_MULTI
#define CONFIG_NET_MULTI	1
#endif

#define CONFIG_MII		1	/* MII PHY management */
#define CONFIG_MII_DEFAULT_TSEC	1	/* Allow unregistered phys */
#define CONFIG_TSEC1	1
#define CONFIG_TSEC1_NAME	"eTSEC1"
#define CONFIG_TSEC2	1
#define CONFIG_TSEC2_NAME	"eTSEC2"
#define CONFIG_TSEC3	1
#define CONFIG_TSEC3_NAME	"eTSEC3"

#define TSEC1_PHY_ADDR		2
#define TSEC2_PHY_ADDR		0
#define TSEC3_PHY_ADDR		1

#define CONFIG_VSC7385_ENET

#define TSEC1_FLAGS		(TSEC_GIGABIT | TSEC_REDUCED)
#define TSEC2_FLAGS		(TSEC_GIGABIT | TSEC_REDUCED)
#define TSEC3_FLAGS		(TSEC_GIGABIT | TSEC_REDUCED)

#define TSEC1_PHYIDX		0
#define TSEC2_PHYIDX		0
#define TSEC3_PHYIDX		0

/* Vitesse 7385 */

#ifdef CONFIG_VSC7385_ENET

/* The flash address and size of the VSC7385 firmware image */
#define CONFIG_VSC7385_IMAGE		0xEF000000
#define CONFIG_VSC7385_IMAGE_SIZE	8192

#endif
#define CONFIG_ETHPRIME		"eTSEC1"

#define CONFIG_PHY_GIGE		1	/* Include GbE speed/duplex detection */

#define CONFIG_HAS_ETH0
#define CONFIG_HAS_ETH1
#define CONFIG_HAS_ETH2

#endif	/* CONFIG_TSEC_ENET */

/*
 * Environment
 */
#if defined(CONFIG_NAND_U_BOOT)
#define CONFIG_ENV_IS_IN_NAND	1
#define CONFIG_ENV_SIZE		CONFIG_SYS_NAND_BLOCK_SIZE
#define CONFIG_ENV_OFFSET	((512 * 1024) + CONFIG_SYS_NAND_BLOCK_SIZE)
#elif !defined(CONFIG_SDCARD_U_BOOT)
#define CONFIG_ENV_IS_IN_FLASH	1
#if CONFIG_SYS_MONITOR_BASE > 0xfff80000
#define CONFIG_ENV_ADDR		0xfff80000
#else
#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE - CONFIG_ENV_SECT_SIZE)
#endif
#define CONFIG_ENV_SIZE		0x2000
#define CONFIG_ENV_SECT_SIZE	0x20000 /* 128K (one sector) */
#else
#define CONFIG_ENV_IS_IN_SDCARD	 1
#define CONFIG_ENV_SIZE		0x2000
/*env located after the u-boot image, size is 8K*/
#define CONFIG_ENV_ADDR_IN_RAM  ((TEXT_BASE - 0x1000) + 0x80000)
#endif

#define CONFIG_LOADS_ECHO	1	/* echo on for serial download */
#define CONFIG_SYS_LOADS_BAUD_CHANGE	1	/* allow baudrate change */

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_IRQ
#define CONFIG_CMD_PING
#define CONFIG_CMD_I2C
#define CONFIG_CMD_MII
#define CONFIG_CMD_ELF
#define CONFIG_CMD_IRQ
#define CONFIG_CMD_SETEXPR

#if defined(CONFIG_PCI)
#define CONFIG_CMD_PCI
#define CONFIG_CMD_BEDBUG
#define CONFIG_CMD_NET
#endif

#undef CONFIG_WATCHDOG			/* watchdog disabled */

#define CONFIG_MMC	1

#ifdef CONFIG_MMC
#define CONFIG_FSL_ESDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR	CONFIG_SYS_MPC85xx_ESDHC_ADDR
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#ifdef CONFIG_P2020
#define CONFIG_SYS_FSL_ESDHC_USE_PIO /* P2020 eSDHC DMA is not functional*/
#endif
#endif

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory	*/
#define CONFIG_CMDLINE_EDITING		/* Command-line editing */
#define CONFIG_SYS_LOAD_ADDR	0x2000000	/* default load address */
#define CONFIG_SYS_PROMPT	"=> "		/* Monitor Command Prompt */
#if defined(CONFIG_CMD_KGDB)
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size */
#else
#define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size */
#endif
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16		/* max number of command args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size */
#define CONFIG_SYS_HZ		1000		/* decrementer freq: 1ms ticks */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ	(8 << 20)	/* Initial Memory map for Linux*/

/*
 * Internal Definitions
 *
 * Boot Flags
 */
#define BOOTFLAG_COLD	0x01		/* Normal Power-On: Boot from FLASH */
#define BOOTFLAG_WARM	0x02		/* Software reboot */

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	230400	/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use */
#endif

/*
 * Environment Configuration
 */

#define CONFIG_HOSTNAME		unknown
#define CONFIG_ROOTPATH		/opt/nfsroot
#define CONFIG_BOOTFILE		uImage
#define CONFIG_UBOOTPATH	u-boot.bin	/* U-Boot image on TFTP server */

/* default location for tftp and bootm */
#define CONFIG_LOADADDR		1000000

#define CONFIG_BOOTDELAY 10	/* -1 disables auto-boot */
#undef  CONFIG_BOOTARGS		/* the boot command will set bootargs */

#define CONFIG_BAUDRATE	115200

#define	CONFIG_EXTRA_ENV_SETTINGS				\
 "netdev=eth0\0"						\
 "uboot=" MK_STR(CONFIG_UBOOTPATH) "\0"				\
 "tftpflash=tftpboot $loadaddr $uboot; "			\
	"protect off " MK_STR(TEXT_BASE) " +$filesize; "	\
	"erase " MK_STR(TEXT_BASE) " +$filesize; "		\
	"cp.b $loadaddr " MK_STR(TEXT_BASE) " $filesize; "	\
	"protect on " MK_STR(TEXT_BASE) " +$filesize; "		\
	"cmp.b $loadaddr " MK_STR(TEXT_BASE) " $filesize\0"	\
 "consoledev=ttyS0\0"				\
 "ramdiskaddr=2000000\0"			\
 "ramdiskfile=p10xx_20xx_rdb/ramdisk.uboot\0"		\
 "fdtaddr=c00000\0"				\
 "fdtfile=p10xx_20xx_rdb/p10xx_20xx_rdb.dtb\0"		\

#define CONFIG_NFSBOOTCOMMAND		\
 "setenv bootargs root=/dev/nfs rw "	\
 "nfsroot=$serverip:$rootpath "		\
 "ip=$ipaddr:$serverip:$gatewayip:$netmask:$hostname:$netdev:off " \
 "console=$consoledev,$baudrate $othbootargs;"	\
 "tftp $loadaddr $bootfile;"		\
 "tftp $fdtaddr $fdtfile;"		\
 "bootm $loadaddr - $fdtaddr"

#define CONFIG_RAMBOOTCOMMAND		\
 "setenv bootargs root=/dev/ram rw "	\
 "console=$consoledev,$baudrate $othbootargs;"	\
 "tftp $ramdiskaddr $ramdiskfile;"	\
 "tftp $loadaddr $bootfile;"		\
 "tftp $fdtaddr $fdtfile;"		\
 "bootm $loadaddr $ramdiskaddr $fdtaddr"

#define CONFIG_BOOTCOMMAND		CONFIG_RAMBOOTCOMMAND

#endif	/* __CONFIG_H */
