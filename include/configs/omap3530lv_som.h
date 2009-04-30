/*
 * (C) Copyright 2006
 * Texas Instruments.
 * Richard Woodruff <r-woodruff2@ti.com>
 * Syed Mohammed Khasim <x0khasim@ti.com>
 *
 * Configuration settings for the 3430 TI SDP3430 board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 */
#define CONFIG_ARMCORTEXA8	1    /* This is an ARM V7 CPU core */
#define CONFIG_OMAP		1    /* in a TI OMAP core */
#define CONFIG_OMAP34XX		1    /* which is a 34XX */
#define CONFIG_OMAP3430		1    /* which is in a 3430 */
#define CONFIG_3430LV_SOM       1    /* working on LV_SOM */
//#define CONFIG_3430_AS_3410	1    /* true for 3430 in 3410 mode */

#include <asm/arch/cpu.h>        /* get chip and board defs */

/* Clock Defines */
#define V_OSCK                   26000000  /* Clock output from T2 */

#if (V_OSCK > 19200000)
#define V_SCLK                   (V_OSCK >> 1)
#else
#define V_SCLK                   V_OSCK
#endif

#define PRCM_CLK_CFG2_266MHZ   1    /* VDD2=1.15v - 133MHz DDR */
//#define PRCM_CLK_CFG2_332MHZ     1    /* VDD2=1.15v - 166MHz DDR */
#define PRCM_PCLK_OPP2           1    /* ARM=500MHz - VDD1=1.20v */

#undef CONFIG_USE_IRQ                 /* no support for IRQs */
#define CONFIG_MISC_INIT_R

#define CONFIG_CMDLINE_TAG       1    /* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS 1
#define CONFIG_INITRD_TAG        1
#define CONFIG_REVISION_TAG      1

/*
 * Size of malloc() pool
 */
#define CFG_ENV_SIZE             SZ_128K    /* Total Size Environment Sector */
#define CFG_MALLOC_LEN           (CFG_ENV_SIZE + SZ_128K)
#define CFG_GBL_DATA_SIZE        128  /* bytes reserved for initial data */

/*
 * Hardware drivers
 */

/*
 * NS16550 Configuration
 */
#define V_NS16550_CLK            (48000000)  /* 48MHz */

#define CFG_NS16550
#define CFG_NS16550_SERIAL
#define CFG_NS16550_REG_SIZE     (-4)
#define CFG_NS16550_CLK          V_NS16550_CLK
#define CFG_NS16550_COM1         OMAP34XX_UART1

/*
 * select serial console configuration
 */
#define CONFIG_SERIAL1           1    /* UART1 on board */
/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX        1
#define CONFIG_BAUDRATE          115200
#define CFG_BAUDRATE_TABLE       {4800, 9600, 19200, 38400, 57600, 115200}

#define NET_CMDS                 (CFG_CMD_DHCP|CFG_CMD_NFS|CFG_CMD_NET)

#ifndef CONFIG_OPTIONAL_NOR_POPULATED
//#define C_MSK (CFG_CMD_FLASH | CFG_CMD_IMLS)
#define C_MSK (CFG_CMD_IMLS | CFG_CMD_FLASH)
#endif

#define CONFIG_COMMANDS          (( CFG_CMD_I2C | CONFIG_CMD_DFL | CFG_CMD_DHCP \
					| CFG_CMD_NAND | CFG_CMD_PING | CFG_CMD_ASKENV ) & ~(C_MSK))

#define CONFIG_BOOTP_MASK        CONFIG_BOOTP_DEFAULT

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#if (CONFIG_COMMANDS & CFG_CMD_NET)
/*
 * SMC91c96 Etherent
 */
#define CONFIG_DRIVER_SMSC9118
#define CONFIG_SMSC9118_BASE	 (DEBUG_BASE)
#endif

#if (CONFIG_COMMANDS & CFG_CMD_I2C)
#define CFG_I2C_SPEED            100
#define CFG_I2C_SLAVE            1
#define CFG_I2C_BUS              0
#define CFG_I2C_BUS_SELECT       1
#define CONFIG_DRIVER_OMAP34XX_I2C 1
#endif

/*
 *  Board NAND Info.
 */
#define CFG_NAND_ADDR NAND_BASE  /* physical address to access nand*/
#define CFG_NAND_BASE NAND_BASE  /* physical address to access nand at CS0*/
#define CFG_NAND_WIDTH_16

#define CFG_MAX_NAND_DEVICE      1 /* Max number of NAND devices */
#define SECTORSIZE               2048

#undef NAND_ALLOW_ERASE_ALL
#define ADDR_COLUMN              1
#define ADDR_PAGE                2
#define ADDR_COLUMN_PAGE         3

#define NAND_ChipID_UNKNOWN      0x00
#define NAND_MAX_FLOORS          1
#define NAND_MAX_CHIPS           1
#define NAND_NO_RB               1
#define CFG_NAND_WP

#define CONFIG_BOOTDELAY         6

#if 1
#if 1
#define CONFIG_BOOTARGS "display=15 console=ttyS0,115200n8 root=/dev/ram rw ramdisk_size=22629"
#if 0
#define CONFIG_BOOTCOMMAND "tftpboot 0x81000000 uImage;tftpboot 0x81200000 rootfs.ext2.gz.uboot;bootm 0x81000000 0x81200000"
#else
#define CONFIG_EXTRA_ENV_SETTINGS \
  "display=15\0" \
  "loadaddr=0x81000000\0"\
  "rootfsaddr=0x81200000\0" \
  "consoledev=ttyS0\0" \
  "rootpath=/opt/nfs-exports/ltib-omap\0" \
  "ramdisksize=89000\0" \
  "nfsoptions=,wsize=1500,rsize=1500\0" \
  "nfsboot=setenv bootargs display=${display} console=${consoledev},${baudrate} root=/dev/nfs rw nfsroot=${serverip}:${rootpath}${nfsoptions} ip=dhcp;tftpboot ${loadaddr} uImage;bootm ${loadaddr}\0" \
  "ramboot=setenv bootargs display=${display} console=${consoledev},${baudrate} root=/dev/ram rw ramdisk_size=${ramdisksize};tftpboot ${loadaddr} uImage;tftpboot ${rootfsaddr} rootfs.ext2.gz.uboot;bootm ${loadaddr} ${rootfsaddr}\0" \
  "xipboot=setenv bootargs display=${display} console=${consoledev},${baudrate} root=/dev/ram rw ramdisk_size=${ramdisksize};bootm ${loadaddr} ${rootfsaddr}\0"
#endif
#else
#define CONFIG_BOOTARGS "console=ttyS0,115200n8 root=/dev/nfs rw nfsroot=192.168.3.5:/opt/nfs-exports/ltib-omap ip=dhcp"
#define CONFIG_BOOTCOMMAND "tftpboot 0x81000000 uImage;bootm 0x81000000"
#endif
#else
#ifdef NFS_BOOT_DEFAULTS
#define CONFIG_BOOTARGS "mem=64M console=ttyS2,115200n8 noinitrd root=/dev/nfs rw nfsroot=192.168.3.5:/opt/nfs-exports/omap-ltib ip=dhcp"
#else
#define CONFIG_BOOTARGS "root=/dev/ram0 rw mem=64M console=ttyS2,115200n8 initrd=0x80600000,8M ramdisk_size=8192"
#endif
#endif

#define CONFIG_PREBOOT \
   "echo ======================NOTICE============================;"    \
   "echo This is the first time that you boot up this board. You are;"  \
   "echo required to set a valid display for your LCD panel.;" \
   "echo Enter the display number of the LCD panel(none for no LCD panel);" \
   "echo Pick one of:;" \
   "echo   2 == LQ121S1DG31     TFT SVGA    (12.1)  Sharp;" \
   "echo   3 == LQ036Q1DA01     TFT QVGA    (3.6)   Sharp w/ASIC;" \
   "echo   5 == LQ064D343       TFT VGA     (6.4)   Sharp;" \
   "echo   7 == LQ10D368        TFT VGA     (10.4)  Sharp;" \
   "echo  15 == LQ043T1DG01     TFT WQVGA   (4.3)   Sharp;" \
   "echo MAKE SURE YOUR DISPLAY IS CORRECTLY ENTERED!;" \
   "askenv display 'Please enter your LCD display number:' 2;"               \
   "printenv display;" \
   "setenv preboot;"                                                   \
   "saveenv;"

#define CONFIG_BOOTCOMMAND "run xipboot"

// #define CONFIG_NETMASK           255.255.255.0
// #define CONFIG_IPADDR            192.168.3.18
// #define CONFIG_SERVERIP          192.168.3.5
// #define CONFIG_ETHADDR           00:08:ee:01:f6:98
// #define CONFIG_BOOTFILE          "uImage"
#define CONFIG_AUTO_COMPLETE     1
/*
 * Miscellaneous configurable options
 */
#define V_PROMPT                 "=> "

#define CFG_LONGHELP             /* undef to save memory */
#define CFG_PROMPT               V_PROMPT
#define CFG_CBSIZE               1024  /* Console I/O Buffer Size */
/* Print Buffer Size */
#define CFG_PBSIZE               (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)
#define CFG_MAXARGS              16          /* max number of command args */
#define CFG_BARGSIZE             CFG_CBSIZE  /* Boot Argument Buffer Size */

#define CFG_MEMTEST_START        (OMAP34XX_SDRC_CS0)  /* memtest works on */
#define CFG_MEMTEST_END          (OMAP34XX_SDRC_CS0+SZ_31M)

#undef	CFG_CLKS_IN_HZ           /* everything, incl board info, in Hz */

#define CFG_LOAD_ADDR            (OMAP34XX_SDRC_CS0) /* default load address */

/* 2430 has 12 GP timers, they can be driven by the SysClk (12/13/19.2) or by
 * 32KHz clk, or from external sig. This rate is divided by a local divisor.
 */
#define V_PVT                    7

#define CFG_TIMERBASE            OMAP34XX_GPT2
#define CFG_PVT                  V_PVT  /* 2^(pvt+1) */
#define CFG_HZ                   ((V_SCLK)/(2 << CFG_PVT))

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	SZ_128K /* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	SZ_4K   /* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	SZ_4K   /* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	2       /* CS1 may or may not be populated */
#define PHYS_SDRAM_1		OMAP34XX_SDRC_CS0
#define PHYS_SDRAM_1_SIZE	SZ_32M            /* at least 32 meg */
#define PHYS_SDRAM_2		OMAP34XX_SDRC_CS1

/* SDRAM Bank Allocation method */
/*#define SDRC_B_R_C		1 */
/*#define SDRC_B1_R_B0_C	1 */
#define SDRC_R_B_C		1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */

/* **** PISMO SUPPORT *** */

/* Configure the PISMO */
/** REMOVE ME ***/
#define PISMO1_NOR_SIZE_SDPV2	GPMC_SIZE_128M
#define PISMO1_NOR_SIZE		GPMC_SIZE_64M

#define PISMO1_NAND_SIZE	GPMC_SIZE_128M
#define PISMO1_ONEN_SIZE	GPMC_SIZE_128M
#define DBG_MPDB_SIZE		GPMC_SIZE_16M
#define PISMO2_SIZE		0

#define CFG_MAX_FLASH_SECT	(520)		/* max number of sectors on one chip */
#define CFG_MAX_FLASH_BANKS      2		/* max number of flash banks */
#define CFG_MONITOR_LEN		SZ_256K 	/* Reserve 2 sectors */

#define PHYS_FLASH_SIZE_SDPV2	SZ_128M
#define PHYS_FLASH_SIZE		SZ_32M

#define CFG_FLASH_BASE		boot_flash_base
#define PHYS_FLASH_SECT_SIZE	boot_flash_sec
/* Dummy declaration of flash banks to get compilation right */
#define CFG_FLASH_BANKS_LIST	{0, 0}

#define CFG_MONITOR_BASE	CFG_FLASH_BASE /* Monitor at start of flash */

#if 1 //PWB!!! was 1
#define CFG_ENV_IS_IN_NAND	1
#define ENV_IS_VARIABLE		1
#else
#define CFG_ENV_IS_NOWHERE	1
#endif

#ifdef CONFIG_OPTIONAL_NOR_POPULATED
# define CFG_ENV_IS_IN_FLASH	1
#endif

#define SMNAND_ENV_OFFSET	(8<<20) /* environment starts here
					    (64MB point) */

#define CFG_ENV_SECT_SIZE	boot_flash_sec
#define CFG_ENV_OFFSET		boot_flash_off
#define CFG_ENV_ADDR		boot_flash_env_addr

#define CONFIG_CMDLINE_EDITING

#ifdef CONFIG_CMDLINE_EDITING
#undef CONFIG_AUTO_COMPLETE
#else
#define CONFIG_AUTO_COMPLETE
#endif

// #define CONFIG_STATUS_LED	1	// Status LED enabled
// #define CONFIG_BOARD_SPECIFIC_LED       /* version has board specific leds */

#ifdef CONFIG_STATUS_LED
#define	STATUS_LED_PAR		1	// makes status_led.h happy

#define STATUS_LED_BIT		0x00000001	// bit 31 (led is logical 1/0)

#define STATUS_LED_PERIOD	(CFG_HZ / 4)
#define STATUS_LED_STATE	STATUS_LED_BLINKING
#define STATUS_LED_ACTIVE       1       /* LED on for bit == 0  */
#define STATUS_LED_BOOT         0       /* LED 0 used for boot status */
#endif

/*-----------------------------------------------------------------------
 * CFI FLASH driver setup
 */
#ifndef CONFIG_OPTIONAL_NOR_POPULATED
#define CFG_NO_FLASH	1            /* Disable NOR Flash support */
#else
#define CFG_FLASH_CFI		1    /* Flash memory is CFI compliant */
#define CFG_FLASH_CFI_DRIVER	1    /* Use drivers/cfi_flash.c */
#define CFG_FLASH_USE_BUFFER_WRITE 1    /* Use buffered writes (~10x faster) */
#define CFG_FLASH_PROTECTION	1    /* Use hardware sector protection */
#define CFG_FLASH_QUIET_TEST	1    /* Dont crib abt missing chips */
#define CFG_FLASH_CFI_WIDTH	0x02
/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(100*CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(100*CFG_HZ) /* Timeout for Flash Write */

/* Flash banks JFFS2 should use */
#define CFG_MAX_MTD_BANKS	(CFG_MAX_FLASH_BANKS+CFG_MAX_NAND_DEVICE)
#define CFG_JFFS2_MEM_NAND
#define CFG_JFFS2_FIRST_BANK	CFG_MAX_FLASH_BANKS /* use flash_info[2] */
#define CFG_JFFS2_NUM_BANKS	1
#define CONFIG_LED_INFOnand_read_buf16
#define CONFIG_LED_LEN		16
#endif  /* optional NOR flash */

#ifndef __ASSEMBLY__
extern unsigned int nand_cs_base;
extern unsigned int boot_flash_base;
extern volatile unsigned int boot_flash_env_addr;
extern unsigned int boot_flash_off;
extern unsigned int boot_flash_sec;
extern unsigned int boot_flash_type;

// GPIO
extern unsigned int pin_get_gpio_input(unsigned int pin);
extern unsigned int pin_set_gpio_dataout(unsigned int pin, unsigned int set);
extern unsigned int pin_init_gpio(unsigned int pin_num, unsigned int in_out);

// Turn on VAUX1 voltage for Product ID
extern void init_vaux1_voltage(void);

// Set the enetaddr environment variable from production data
extern int fetch_production_data(void);
extern void board_get_nth_enetaddr (unsigned char *enetaddr, int which);

#endif

#define WRITE_NAND_COMMAND(d, adr) __raw_writew(d, (nand_cs_base + GPMC_NAND_CMD))
#define WRITE_NAND_ADDRESS(d, adr) __raw_writew(d, (nand_cs_base + GPMC_NAND_ADR))
#define WRITE_NAND(d, adr) __raw_writew(d, (nand_cs_base + GPMC_NAND_DAT))
#define READ_NAND(adr) __raw_readw((nand_cs_base + GPMC_NAND_DAT))

/* Other NAND Access APIs */
#define NAND_WP_OFF()  do {*(volatile u32 *)(GPMC_CONFIG) |= 0x00000010;} while(0)
#define NAND_WP_ON()  do {*(volatile u32 *)(GPMC_CONFIG) &= ~0x00000010;} while(0)
#define NAND_DISABLE_CE(nand)
#define NAND_ENABLE_CE(nand)
#define NAND_WAIT_READY(nand)	udelay(10)


#endif                           /* __CONFIG_H */
