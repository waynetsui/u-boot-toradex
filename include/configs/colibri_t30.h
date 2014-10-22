/*
 * Copyright (c) 2012-2014 Toradex, Inc.
 *
 * Configuration settings for the Toradex Colibri T30 modules.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/sizes.h>

#include "tegra30-common.h"

#define CONFIG_ARCH_MISC_INIT

/* High-level configuration options */
#define V_PROMPT			"Colibri T30 # "
#define CONFIG_TEGRA_BOARD_STRING	"Toradex Colibri T30"

/* Board-specific serial config */
#define CONFIG_SERIAL_MULTI
#define CONFIG_TEGRA_ENABLE_UARTA
#define CONFIG_SYS_NS16550_COM1		NV_PA_APB_UARTA_BASE

#define CONFIG_MACH_TYPE		MACH_TYPE_COLIBRI_T30

#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG
#define CONFIG_SERIAL_TAG

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT		/* board/nvidia/common/board.c */

/* Make the HW version stuff available in u-boot env */
#define CONFIG_VERSION_VARIABLE		/* ver environment variable */
#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
#define CONFIG_CMD_ASKENV

/* I2C */
#define CONFIG_SYS_I2C_TEGRA
#define CONFIG_SYS_I2C_INIT_BOARD
#define CONFIG_SYS_I2C_SPEED		100000
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C

/* SD/MMC */
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_TEGRA_MMC
#define CONFIG_CMD_MMC
#define CONFIG_CMD_EXT3
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION

/* Environment in eMMC, at the end of 2nd "boot sector" */
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_ENV_OFFSET		(-CONFIG_ENV_SIZE)
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_MMC_ENV_PART		2

/* USB Host support */
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_TEGRA
#define CONFIG_USB_MAX_CONTROLLER_COUNT 3
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_USB

/* USB networking support */
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX

#define CONFIG_USE_ARCH_MEMCPY

/* General networking support */
#define CONFIG_CMD_NET
#define CONFIG_CMD_DHCP

/* Miscellaneous commands */
#define CONFIG_CMD_SETEXPR
#define CONFIG_FAT_WRITE

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	1
#undef CONFIG_IPADDR
#define CONFIG_IPADDR		192.168.10.2
#define CONFIG_NETMASK		255.255.255.0
#undef CONFIG_SERVERIP
#define CONFIG_SERVERIP		192.168.10.1

#define CONFIG_BOOTCOMMAND \
	"run emmcboot; echo; echo emmcboot failed; " \
	"run nfsboot; echo; echo nfsboot failed"

#define EMMC_BOOTCMD \
	"emmcargs=ip=off root=/dev/mmcblk0p2 rw,noatime rootfstype=ext3 " \
		"rootwait\0" \
	"emmcboot=run setup; setenv bootargs ${defargs} ${emmcargs} " \
		"${setupargs} ${vidargs}; echo Booting from internal eMMC " \
		"chip...; run emmcdtbload; load mmc 0:1 ${kernel_addr_r} " \
		"${boot_file} && bootm ${kernel_addr_r} - ${dtbparam}\0" \
	"emmcdtbload=setenv dtbparam; load mmc 0:1 ${fdt_addr_r} " \
		"${soc}-colibri-${fdt_board}.dtb && " \
		"setenv dtbparam ${fdt_addr_r}\0"

#define NFS_BOOTCMD \
	"nfsargs=ip=:::::eth0:on root=/dev/nfs rw netdevwait\0" \
	"nfsboot=run setup; setenv bootargs ${defargs} ${nfsargs} " \
		"${setupargs} ${vidargs}; echo Booting via DHCP/TFTP/NFS...; " \
		"usb start; run nfsdtbload; dhcp ${kernel_addr_r} " \
		"&& bootm ${kernel_addr_r} - ${dtbparam}\0" \
	"nfsdtbload=setenv dtbparam; tftp ${fdt_addr_r} " \
		"${soc}-colibri-${fdt_board}.dtb " \
		"&& setenv dtbparam ${fdt_addr_r}\0"

#define SD_BOOTCMD \
	"sdargs=ip=off root=/dev/mmcblk1p2 rw,noatime rootfstype=ext3 " \
		"rootwait\0" \
	"sdboot=run setup; setenv bootargs ${defargs} ${sdargs} ${setupargs} " \
		"${vidargs}; echo Booting from SD card in 8bit slot...; " \
		"run sddtbload; load mmc 1:1 ${kernel_addr_r} " \
		"${boot_file} && bootm ${kernel_addr_r} - ${dtbparam}\0" \
	"sddtbload=setenv dtbparam; load mmc 1:1 ${fdt_addr_r} " \
		"${soc}-colibri-${fdt_board}.dtb " \
		"&& setenv dtbparam ${fdt_addr_r}\0"

#define USB_BOOTCMD \
	"usbargs=ip=off root=/dev/sda2 rw,noatime rootfstype=ext3 " \
		"rootwait\0" \
	"usbboot=run setup; setenv bootargs ${defargs} ${setupargs} " \
		"${usbargs} ${vidargs}; echo Booting from USB stick...; " \
		"usb start && run usbdtbload; load usb 0:1 ${kernel_addr_r} " \
		"${boot_file} && bootm ${kernel_addr_r} - ${dtbparam}\0" \
	"usbdtbload=setenv dtbparam; load usb 0:1 ${fdt_addr_r} " \
		"${soc}-colibri-${fdt_board}.dtb " \
		"&& setenv dtbparam ${fdt_addr_r}\0"

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_AUTO_COMPLETE
#undef CONFIG_SYS_CBSIZE
#define CONFIG_SYS_CBSIZE		1024

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#undef CONFIG_SYS_MAXARGS
#define CONFIG_SYS_MAXARGS		32
#undef CONFIG_SYS_BARGSIZE
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE

#define CONFIG_CMDLINE_EDITING

/* Toradex Configblock in eMMC, at the end of 1st "boot sector" */
#define CONFIG_TRDX_CFG_BLOCK
#define CONFIG_TRDX_CFG_BLOCK_DEV	0
#define CONFIG_TRDX_CFG_BLOCK_IS_IN_MMC
#define CONFIG_TRDX_CFG_BLOCK_OFFSET	(-512)
#define CONFIG_TRDX_CFG_BLOCK_PART	1
#define CONFIG_TRDX_CFG_BLOCK_SIZE	4096

#define CONFIG_OF_LIBFDT
#define CONFIG_CMD_BOOTZ

#define CONFIG_CMD_TIME
#define CONFIG_CMD_MEMTEST
#define CONFIG_SYS_ALT_MEMTEST

#define CONFIG_CMD_FS_GENERIC

#include "tegra-common-usb-gadget.h"
#include "tegra-common-post.h"

#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"boot_file=uImage\0" \
	"console=ttyS0\0" \
	"defargs=core_edp_mv=1300 usb_high_speed=1\0" \
	EMMC_BOOTCMD \
	"fdt_board=eval-v3\0" \
	MEM_LAYOUT_ENV_SETTINGS \
	"memargs=vmalloc=128M mem=1012M@2048M fbmem=12M@3060M\0" \
	NFS_BOOTCMD \
	SD_BOOTCMD \
	"setup=setenv setupargs asix_mac=${ethaddr} " \
		"consoleblank=0  no_console_suspend=1 console=tty1 " \
		"console=${console},${baudrate}n8 debug_uartport=lsport,0 " \
		"${memargs}\0" \
	"setupdate=load mmc 1:1 ${kernel_addr_r} flash_mmc.img " \
		"&& source ${kernel_addr_r}\0" \
	USB_BOOTCMD \
	"vidargs=video=tegrafb0:640x480-16@60\0"

#endif /* __CONFIG_H */
