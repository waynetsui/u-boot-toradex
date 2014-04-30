/*
 * Copyright 2013 Toradex, Inc.
 *
 * Configuration settings for the Toradex VF61 module.
 *
 * Based on vf610twr.h:
 * Copyright 2013 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/arch/imx-regs.h>
#include <config_cmd_default.h>

#define CONFIG_VF610

#define CONFIG_SYS_GENERIC_BOARD
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_SKIP_LOWLEVEL_INIT

/* Enable passing of ATAGs */
#define CONFIG_CMDLINE_TAG

#define CONFIG_CMD_FUSE
#ifdef CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP
#endif

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 2 * 1024 * 1024)

#define CONFIG_BOARD_EARLY_INIT_F

#define CONFIG_FSL_LPUART
#define LPUART_BASE			UART0_BASE

/* Allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_BAUDRATE			115200

/* NAND support */
#define CONFIG_CMD_NAND
#define CONFIG_CMD_WRITEBCB
#define CONFIG_NAND_FSL_NFC
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		NFC_BASE_ADDR

/* Dynamic MTD partition support */
#define CONFIG_CMD_MTDPARTS	/* Enable 'mtdparts' command line support */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE	/* needed for mtdparts commands */
#define MTDIDS_DEFAULT		"nand0=fsl_nfc"
#define MTDPARTS_DEFAULT	"mtdparts=fsl_nfc:"		\
				"128k(vf-bcb)ro,"		\
				"1408k(u-boot)ro,"		\
				"512k(u-boot-env)ro,"		\
				"8m(kernel-ubi),"		\
				"-(rootfs-ubi)"

#undef CONFIG_CMD_IMLS

#define CONFIG_MMC
#define CONFIG_FSL_ESDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR	0
#define CONFIG_SYS_FSL_ESDHC_NUM	1

#define CONFIG_SYS_FSL_ERRATUM_ESDHC111

#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION

#define CONFIG_RBTREE
#define CONFIG_LZO
#define CONFIG_CMD_FS_GENERIC
#define CONFIG_CMD_BOOTZ
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS	/* increases size by almost 60 KB */

#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_FEC_MXC
#define CONFIG_MII
#define IMX_FEC_BASE			ENET1_BASE_ADDR
#define CONFIG_FEC_XCV_TYPE		RMII
#define CONFIG_FEC_MXC_PHYADDR          0
#define CONFIG_PHYLIB
#define CONFIG_PHY_MICREL

#define CONFIG_IPADDR		192.168.10.2
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_SERVERIP		192.168.10.1

#define CONFIG_BOOTDELAY		1

#define CONFIG_BOARD_LATE_INIT
#define CONFIG_TRDX_CFG_BLOCK_OFFSET	0x800
#define CONFIG_REVISION_TAG
#define CONFIG_SERIAL_TAG

#define CONFIG_LOADADDR			0x80010000
#define CONFIG_SYS_TEXT_BASE		0x3f408000

#define DEFAULT_BOOTCOMMAND					\
	"run ubiboot; run nfsboot"

#define MMC_BOOTCMD						\
	"run setup; "						\
	"setenv bootargs ${defargs} ${mmcargs} ${mtdparts} ${setupargs}; " \
	"echo Booting from MMC/SD card...; "			\
	"mmc part 0; fatload mmc 0:1 ${loadaddr} uImage && bootm"

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
	"defargs=vmalloc=64M usb_high_speed=1\0" \
	"mmcargs=root=/dev/mmcblk0p2 rw rootwait\0" \
	"sdboot=" MMC_BOOTCMD "\0" \
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	"nfsargs=ip=:::::eth0: root=/dev/nfs\0" \
	"setup=setenv setupargs " \
	"fec_mac=${ethaddr} no_console_suspend=1 console=tty1 console=ttymxc0" \
		",${baudrate}n8 ${memargs}\0" \
	"ubiargs=ubi.mtd=rootfs-ubi root=ubi0:rootfs rootfstype=ubifs\0" \
	"ubiboot=" UBI_BOOTCMD "\0" \
	""

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser */
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_PROMPT		"Colibri VFxx # "
#undef CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		\
			(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_MEMTEST_START	0x80010000
#define CONFIG_SYS_MEMTEST_END		0x87C00000

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR
#define CONFIG_SYS_HZ			1000
#define CONFIG_CMDLINE_EDITING

/*
 * Stack sizes
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE		(128 * 1024)	/* regular stack */

/* Physical memory map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM			(0x80000000)
#define PHYS_SDRAM_SIZE			(256 * 1024 * 1024)

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* Environment organization */
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SIZE			(64 * 2048)
#define CONFIG_ENV_OFFSET		(6 * 64 * 2048)

#define CONFIG_OF_LIBFDT
#define CONFIG_CMD_BOOTZ

#define CONFIG_SYS_NO_FLASH

#endif
