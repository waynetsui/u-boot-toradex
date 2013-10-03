/*
 * Copyright 2013 Toradex, Inc.
 *
 * Configuration settings for the Colibri VF50 module booting from NAND flash.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/* We now boot from the gfxRAM area of the OCRAM. */
#define CONFIG_BOARD_SIZE_LIMIT		524288

 /* High Level Configuration Options */

#define CONFIG_VYBRID

#define CONFIG_SYS_VYBRID_HCLK		24000000
#define CONFIG_SYS_VYBRID_CLK32		32768
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_SYS_ICACHE_OFF
#define CONFIG_SYS_CACHELINE_SIZE	64

#include <asm/arch/vybrid-regs.h>

/*
 * Disabled for now due to build problems under Debian and a significant
 * increase in the final file size: 144260 vs. 109536 Bytes.
 */

#define CONFIG_CMDLINE_TAG			/* enable passing of ATAGs */
#undef CONFIG_SETUP_MEMORY_TAGS
#undef CONFIG_INITRD_TAG

#undef CONFIG_OF_LIBFDT

#define CONFIG_MACH_TYPE		MACH_TYPE_COLIBRI_VF50
/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 4 * 1024 * 1024)

#define CONFIG_BOARD_LATE_INIT

#define CONFIG_REVISION_TAG		1
#define CONFIG_SERIAL_TAG		1

#define CONFIG_TRDX_CFG_BLOCK
#define CONFIG_TRDX_CFG_BLOCK_OFFSET	0x40000

/* Hardware drivers */
#define CONFIG_VYBRID_UART
#define CONFIG_VYBRID_UART_BASE		UART0_BASE
#define CONFIG_VYBRID_GPIO

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_SYS_UART_PORT		(0)
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{9600, 19200, 38400, 57600, 115200}

/* Command definition */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_DIAG
#define CONFIG_CMD_BDI		/* bdinfo */
#define CONFIG_CMD_BOOTD
#define CONFIG_CMD_CONSOLE	/* coninfo */
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ELF
#undef CONFIG_CMD_FLASH
#define CONFIG_CMD_SAVEENV
#define CONFIG_CMD_MEMORY	/* md mm nm mw cp cmp crc base loop mtest */
#define CONFIG_CMD_MISC
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#undef CONFIG_CMD_NFS		/* NFS support			*/
#define CONFIG_CMD_PING
#define	CONFIG_CMD_NAND
#define CONFIG_RBTREE
#define CONFIG_LZO
#define CONFIG_CMD_UBI
//#define CONFIG_CMD_UBIFS	/* increases size by almost 60 KB */
#undef CONFIG_CMD_DATE
#define CONFIG_CMD_IMI		/* iminfo */
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_LOADB		/* loadb */
#undef CONFIG_CMD_LOADS		/* loads */

#define CONFIG_BOOTDELAY		1

#define CONFIG_BZIP2
#define CONFIG_CRC32_VERIFY
#define CONFIG_TIMESTAMP

#define CONFIG_AUTO_COMPLETE

/*
 * NAND FLASH
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_JFFS2_NAND
#define CONFIG_NAND_FSL_NFC
#define CONFIG_NAND_FSL_NFC_8BIT
#define CONFIG_SYS_NAND_BASE		0x400E0000
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define NAND_MAX_CHIPS			CONFIG_SYS_MAX_NAND_DEVICE
#define CONFIG_SYS_NAND_SELECT_DEVICE
#define	CONFIG_SYS_64BIT_VSPRINTF	/* needed for nand_util.c */
#endif /* CONFIG_CMD_NAND */

/* Network configuration */
#define CONFIG_MCFFEC
#ifdef CONFIG_MCFFEC
#	define CONFIG_MII		1
#	define CONFIG_MII_INIT		1
#	define CONFIG_SYS_DISCOVER_PHY
#	define CONFIG_SYS_RX_ETH_BUFFER	8
#	define CONFIG_SYS_FAULT_ECHO_LINK_DOWN

#	define CONFIG_SYS_FEC1_PINMUX	0
#	define CONFIG_SYS_FEC1_IOBASE	MACNET1_BASE_ADDR
#	define CONFIG_SYS_FEC1_MIIBASE	MACNET1_BASE_ADDR
#	define MCFFEC_TOUT_LOOP 50000
#	undef CONFIG_HAS_ETH1

#	define CONFIG_ETHPRIME		"FEC1"
#	define CONFIG_IPADDR		192.168.10.2
#	define CONFIG_NETMASK		255.255.255.0
#	define CONFIG_SERVERIP		192.168.10.1
#endif /* CONFIG_MCFFEC */

#define DEFAULT_BOOTCOMMAND					\
	"run ubiboot; run nfsboot"

#define MMC_BOOTCMD						\
	"run setup; "						\
	"setenv bootargs ${defargs} ${mmcargs} ${mtdparts} ${setupargs}; " \
	"echo Booting from MMC/SD card...; "			\
	"fatload mmc 0:1 ${loadaddr} uImage && bootm"

#define NFS_BOOTCMD						\
	"run setup; "						\
	"setenv bootargs ${defargs} ${nfsargs} ${mtdparts} ${setupargs}; " \
	"echo Booting from NFS...; "				\
	"dhcp && bootm"

#define UBI_BOOTCMD						\
	"run setup; "						\
	"setenv bootargs ${defargs} ${ubiargs} ${mtdparts} ${setupargs}; " \
	"echo Booting from NAND...; "				\
	"ubi part kernel-ubi && ubi read ${loadaddr} kernel && bootm"

#define CONFIG_BOOTCOMMAND	DEFAULT_BOOTCOMMAND
#define CONFIG_NFSBOOTCOMMAND	NFS_BOOTCMD

#define CONFIG_EXTRA_ENV_SETTINGS \
	"defargs=vmalloc=64M mem=128M usb_high_speed=1\0" \
	"mmcargs=root=/dev/mmcblk0p2 rw rootwait\0" \
	"sdboot=" MMC_BOOTCMD "\0" \
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	"nfsargs=ip=:::::eth0: root=/dev/nfs\0" \
	"setup=setenv setupargs " \
	"fec_mac=${ethaddr} no_console_suspend=1 console=tty1 console=ttymxc0" \
		",${baudrate}n8\0" \
	"ubiargs=ubi.mtd=5 root=ubi0:rootfs rootfstype=ubifs\0" \
	"ubiboot=" UBI_BOOTCMD "\0" \
	""

/* Dynamic MTD partition support */
#define CONFIG_CMD_MTDPARTS	/* Enable 'mtdparts' command line support */
#define CONFIG_MTD_PARTITIONS	/* ??? */
#define CONFIG_MTD_DEVICE	/* needed for mtdparts commands */
#define MTDIDS_DEFAULT		"nand0=NAND"
#define MTDPARTS_DEFAULT	"mtdparts=NAND:"		\
				"256k(fcb_)ro,"			\
				"256k@256k(config-block)ro,"	\
				"512k@512k(u-boot)ro,"		\
				"1m(u-boot-env)ro,"		\
				"8m(kernel-ubi),"		\
				"-(rootfs-ubi)"

/* SD/MMC */
#define CONFIG_MMC
#ifdef CONFIG_MMC
#define CONFIG_SYS_ESDHC1_BASE		ESDHC2_BASE_ADDR
#define CONFIG_FSL_ESDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR	0
#define CONFIG_SYS_FSL_ESDHC_NUM	1
#define CONFIG_ESDHC_NO_SNOOP		1
//#define CONFIG_MMC_TRACE

#define CONFIG_SYS_FSL_ERRATUM_ESDHC135
#define CONFIG_SYS_FSL_ERRATUM_ESDHC111
#define CONFIG_SYS_FSL_ERRATUM_ESDHC_A001

#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#endif /* CONFIG_MMC */

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH

/* Environment not stored */
//#define CONFIG_ENV_IS_NOWHERE
#ifndef CONFIG_ENV_IS_NOWHERE
/* Environment stored in NAND flash */
#define CONFIG_ENV_IS_IN_NAND		1 /* use NAND for environment vars */
#if defined(CONFIG_ENV_IS_IN_NAND)
#define CONFIG_ENV_OFFSET		0x100000
#define CONFIG_ENV_RANGE		0x100000
#endif /* CONFIG_ENV_IS_IN_NAND */
#endif /* !CONFIG_ENV_IS_NOWHERE */
#define CONFIG_ENV_SIZE			(8 * 1024)

#define CONFIG_LOADADDR			0x80010000	/* loadaddr env var */
#define CONFIG_ARP_TIMEOUT		200UL

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser */
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_PROMPT		"Colibri VF50 # "
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size */
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE		\
			(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#define CONFIG_SYS_MEMTEST_START	0x80010000
#define CONFIG_SYS_MEMTEST_END		0x87C00000

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR

#define CONFIG_SYS_HZ			1000
#define CONFIG_CMDLINE_EDITING

#define CONFIG_PRAM			2048

/*
 * Stack sizes
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE		(128 * 1024)	/* regular stack */

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1_SIZE		(128 * 1024 * 1024)

#define CONFIG_SYS_SDRAM_BASE		(0x80000000)
#define CONFIG_SYS_INIT_RAM_ADDR	(IRAM_BASE_ADDR)
#define CONFIG_SYS_INIT_RAM_SIZE	(IRAM_SIZE)

#define CONFIG_BOARD_EARLY_INIT_F

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* timer */
#define FTM_BASE_ADDR			FTM0_BASE_ADDR
#define CONFIG_TMR_USEPIT

/* clock/PLL configuration */
#define CONFIG_SYS_CLKCTL_CCGR0		0xFFFFFFFF
#define CONFIG_SYS_CLKCTL_CCGR1		0xFFFFFFFF
#define CONFIG_SYS_CLKCTL_CCGR2		0xFFFFFFFF
#define CONFIG_SYS_CLKCTL_CCGR3		0xFFFFFFFF
#define CONFIG_SYS_CLKCTL_CCGR4		0xFFFFFFFF
#define CONFIG_SYS_CLKCTL_CCGR5		0xFFFFFFFF
#define CONFIG_SYS_CLKCTL_CCGR6		0xFFFFFFFF
#define CONFIG_SYS_CLKCTL_CCGR7		0xFFFFFFFF
#define CONFIG_SYS_CLKCTL_CCGR8		0xFFFFFFFF
#define CONFIG_SYS_CLKCTL_CCGR9		0xFFFFFFFF
#define CONFIG_SYS_CLKCTL_CCGR10	0xFFFFFFFF
#define CONFIG_SYS_CLKCTL_CCGR11	0xFFFFFFFF

#define CONFIG_SYS_CLKCTRL_CCR		0x00010005
/* 10.2.3 CCM Clock Switcher Register (CCM_CCSR) */
//DDRC_CLK_SEL: PLL2 PFD2 clk
//PLL2 (PLL 528 MHz)
//PFD2 396 MHz
#define CONFIG_SYS_CLKCTRL_CCSR		0x0003FF24
#define CONFIG_SYS_CLKCTRL_CACRR	0x00000810
#define CONFIG_SYS_CLKCTRL_CSCMR1	0x03CA0000
#define CONFIG_SYS_CLKCTRL_CSCDR1	0x01000000
#define CONFIG_SYS_CLKCTRL_CSCDR2	0x30114240
#define CONFIG_SYS_CLKCTRL_CSCDR3	0x00003F1F
/* 9.10.6 Ethernet RMII/MII Clocking
   10.2.9 CCM Serial Clock Multiplexer Register 2 (CCM_CSCMR2) */
#define CONFIG_SYS_CLKCTRL_CSCMR2	0x00000020	/* RMII_CLK_SEL:
							   PLL5 main clock */
#define CONFIG_SYS_CLKCTRL_CSCDR4	0x00000000
#define CONFIG_SYS_CLKCTRL_CLPCR	0x00000078

#define CONFIG_SYS_ANADIG_USB1_CTRL	0x00012000
#define CONFIG_SYS_ANADIG_USB2_CTRL	0x00012000
#define CONFIG_SYS_ANADIG_528_CTRL	0x00002001
#define CONFIG_SYS_ANADIG_528_SS	0x00000000
#define CONFIG_SYS_ANADIG_528_NUM	0x00000000
#define CONFIG_SYS_ANADIG_528_DENOM	0x00000012
#define CONFIG_SYS_ANADIG_VID_CTRL	0x00011028
#define CONFIG_SYS_ANADIG_VID_NUM	0x00000000
#define CONFIG_SYS_ANADIG_VID_DENOM	0x00000012
/* 11.21.13 PLL5 Control register (ANADIG_PLL5_CTRL) */
#define CONFIG_SYS_ANADIG_ENET_CTRL	0x00002001	/* !BYPASS, !POWERDOWN,
							   ENABLE */
#define CONFIG_SYS_ANADIG_PFD_USB1	0x1B1D1A1C
#define CONFIG_SYS_ANADIG_PFD_528	0x171C1813
#define CONFIG_SYS_ANADIG_USB1_MISC	0x00000002
#define CONFIG_SYS_ANADIG_USB2_VBUS	0x00100004
#define CONFIG_SYS_ANADIG_USB2_CHRG	0x00000000
#define CONFIG_SYS_ANADIG_USB2_MISC	0x00000002
#define CONFIG_SYS_ANADIG_SYS_CTRL	0x00002001
#define CONFIG_SYS_ANADIG_SYS_SS	0x00000000
#define CONFIG_SYS_ANADIG_SYS_NUM	0x00000000
#define CONFIG_SYS_ANADIG_SYS_DENOM	0x00000012
#define CONFIG_SYS_ANADIG_SYS_PFD_528	0x00000000
#define CONFIG_SYS_ANADIG_SYS_PLL_LOCK	0x00000000

#endif /* __CONFIG_H */
