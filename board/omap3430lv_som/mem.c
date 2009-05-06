/*
 * (C) Copyright 2004-2006
 * Texas Instruments, <www.ti.com>
 * Richard Woodruff <r-woodruff2@ti.com>
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

#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/io.h>
#include <asm/arch/bits.h>
#include <asm/arch/clocks.h>
#include <asm/arch/mem.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/sys_info.h>
#include <environment.h>
#include <command.h>

/****** DATA STRUCTURES ************/

/* Only One NAND allowed on board at a time. 
 * The GPMC CS Base for the same
 */
unsigned int nand_cs_base = 0;
unsigned int onenand_cs_base = 0;
unsigned int boot_flash_base = 0;
unsigned int boot_flash_off = 0;
unsigned int boot_flash_sec = 0;
volatile unsigned int boot_flash_env_addr = 0;
/* help common/env_flash.c */
#ifdef ENV_IS_VARIABLE

ulong NOR_FLASH_BANKS_LIST[CFG_MAX_FLASH_BANKS];

int NOR_MAX_FLASH_BANKS = 0 ;           /* max number of flash banks */

uchar(*boot_env_get_char_spec) (int index);
int (*boot_env_init) (void);
int (*boot_saveenv) (void);
void (*boot_env_relocate_spec) (void);

/* StrataNor */
extern uchar flash_env_get_char_spec(int index);
extern int flash_env_init(void);
extern int flash_saveenv(void);
extern void flash_env_relocate_spec(void);
extern char *flash_env_name_spec;

/* 16 bit NAND */
extern uchar nand_env_get_char_spec(int index);
extern int nand_env_init(void);
extern int nand_saveenv(void);
extern void nand_env_relocate_spec(void);
extern char *nand_env_name_spec;


#if (CONFIG_COMMANDS & CFG_CMD_FLASH)
u8 is_flash = 0;
#endif

char *env_name_spec = 0;
/* update these elsewhere */
env_t *env_ptr = 0;

#if ((CONFIG_COMMANDS&(CFG_CMD_ENV|CFG_CMD_FLASH)) == (CFG_CMD_ENV|CFG_CMD_FLASH))
extern env_t *flash_addr;
#endif

#endif				/* ENV_IS_VARIABLE */

/* Global fellows */
#if (CONFIG_COMMANDS & CFG_CMD_NAND)
u8 is_nand = 0;
#endif

static u32 gpmc_enet[GPMC_MAX_REG] = {
	OMAP3530LV_SOM_ENET_GPMC_CONFIG1,
	OMAP3530LV_SOM_ENET_GPMC_CONFIG2,
	OMAP3530LV_SOM_ENET_GPMC_CONFIG3,
	OMAP3530LV_SOM_ENET_GPMC_CONFIG4,
	OMAP3530LV_SOM_ENET_GPMC_CONFIG5,
	OMAP3530LV_SOM_ENET_GPMC_CONFIG6, 0
};

static u32 gpmc_m_isp1760[GPMC_MAX_REG] = {
	OMAP3530LV_SOM_ISP1760_GPMC_CONFIG1,
	OMAP3530LV_SOM_ISP1760_GPMC_CONFIG2,
	OMAP3530LV_SOM_ISP1760_GPMC_CONFIG3,
	OMAP3530LV_SOM_ISP1760_GPMC_CONFIG4,
	OMAP3530LV_SOM_ISP1760_GPMC_CONFIG5,
	OMAP3530LV_SOM_ISP1760_GPMC_CONFIG6, 0
};

#ifdef CFG_FIX_FLASH_SYNC
static u32 gpmc_stnor_async[GPMC_MAX_REG] = {
        OMAP3530LV_SOM_STNOR_ASYNC_CONFIG1,
        OMAP3530LV_SOM_STNOR_ASYNC_CONFIG2,
        OMAP3530LV_SOM_STNOR_ASYNC_CONFIG3,
        OMAP3530LV_SOM_STNOR_ASYNC_CONFIG4,
        OMAP3530LV_SOM_STNOR_ASYNC_CONFIG5,
        OMAP3530LV_SOM_STNOR_ASYNC_CONFIG6, 0
};

static u32 gpmc_stnor_sync[GPMC_MAX_REG] = {
        OMAP3530LV_SOM_STNOR_SYNC_CONFIG1,
        OMAP3530LV_SOM_STNOR_SYNC_CONFIG2,
        OMAP3530LV_SOM_STNOR_SYNC_CONFIG3,
        OMAP3530LV_SOM_STNOR_SYNC_CONFIG4,
        OMAP3530LV_SOM_STNOR_SYNC_CONFIG5,
        OMAP3530LV_SOM_STNOR_SYNC_CONFIG6, 0
};
#endif

static u32 gpmc_m_nand[GPMC_MAX_REG] = {
	M_NAND_GPMC_CONFIG1,
	M_NAND_GPMC_CONFIG2,
	M_NAND_GPMC_CONFIG3,
	M_NAND_GPMC_CONFIG4,
	M_NAND_GPMC_CONFIG5,
	M_NAND_GPMC_CONFIG6, 0
};

// Compact Flash setup
static u32 gpmc_m_cf[GPMC_MAX_REG] = {
	M_CF_GPMC_CONFIG1,
	M_CF_GPMC_CONFIG2,
	M_CF_GPMC_CONFIG3,
	M_CF_GPMC_CONFIG4,
	M_CF_GPMC_CONFIG5,
	M_CF_GPMC_CONFIG6, 0
};

/* These are bit definitions for the RCR register of the NOR flash       */
/* 28FxxxP30 device.  This register sets the bus configration for reads. */
/* settings, located on address pins A[15:0].                            */
#define FLASH_28FxxxP30_RCR_RM    0x8000
#define FLASH_28FxxxP30_RCR_R     0x4000
#define FLASH_28FxxxP30_RCR_LC(x) ((x & 0x7) << 11)
#define FLASH_28FxxxP30_RCR_WP    0x0400
#define FLASH_28FxxxP30_RCR_DH    0x0200
#define FLASH_28FxxxP30_RCR_WD    0x0100
#define FLASH_28FxxxP30_RCR_BS    0x0080
#define FLASH_28FxxxP30_RCR_CE    0x0040
#define FLASH_28FxxxP30_RCR_BW    0x0008
#define FLASH_28FxxxP30_RCR_BL(x) ((x & 0x7) << 0)
#define FLASH_28FxxxP30_BL_4      0x1
#define FLASH_28FxxxP30_BL_8      0x2
#define FLASH_28FxxxP30_BL_16     0x3
#define FLASH_28FxxxP30_BL_CONT   0x7

/********** Functions ****/

/* ENV Functions */
#ifdef ENV_IS_VARIABLE
uchar env_get_char_spec(int index)
{
	if (!boot_env_get_char_spec) {
		puts("ERROR!! env_get_char_spec not available\n");
	} else
		return boot_env_get_char_spec(index);
	return 0;
}
int env_init(void)
{
	if (!boot_env_init) {
		puts("ERROR!! boot_env_init not available\n");
	} else
		return boot_env_init();
	return -1;
}
int saveenv(void)
{
	if (!boot_saveenv) {
		puts("ERROR!! boot_saveenv not available\n");
	} else
		return boot_saveenv();
	return -1;
}
void env_relocate_spec(void)
{
	if (!boot_env_relocate_spec) {
		puts("ERROR!! boot_env_relocate_spec not available\n");
	} else
		boot_env_relocate_spec();
}
#endif


/**************************************************************************
 * make_cs1_contiguous() - for es2 and above remap cs1 behind cs0 to allow
 *  command line mem=xyz use all memory with out discontinuous support
 *  compiled in.  Could do it at the ATAG, but there really is two banks...
 * Called as part of 2nd phase DDR init.
 **************************************************************************/
void make_cs1_contiguous(void)
{
	u32 size, a_add_low, a_add_high;

	size = get_sdr_cs_size(SDRC_CS0_OSET);
	size /= SZ_32M;		/* find size to offset CS1 */
	a_add_high = (size & 3) << 8;	/* set up low field */
	a_add_low = (size & 0x3C) >> 2;	/* set up high field */
	__raw_writel((a_add_high | a_add_low), SDRC_CS_CFG);

}

/********************************************************
 *  mem_ok() - test used to see if timings are correct
 *             for a part. Helps in guessing which part
 *             we are currently using.
 *******************************************************/
u32 mem_ok(void)
{
	u32 val1, val2, orig1, orig2, addr;
	u32 pattern = 0x12345678;

	addr = OMAP34XX_SDRC_CS0;

	orig1 = __raw_readl(addr + 0x400);	/* try to save original value */
	orig2 = __raw_readl(addr);
	__raw_writel(0x0, addr + 0x400);	/* clear pos A */
	__raw_writel(pattern, addr);	/* pattern to pos B */
	__raw_writel(0x0, addr + 4);	/* remove pattern off the bus */
	val1 = __raw_readl(addr + 0x400);	/* get pos A value */
	val2 = __raw_readl(addr);	/* get val2 */

	if ((val1 != 0) || (val2 != pattern))	/* see if pos A value changed */
		return (0);
	else {
		/* restore original values and return pass */
		__raw_writel(orig1, addr + 0x400);
		__raw_writel(orig2, addr);
		return (1);
	}
}

/********************************************************
 *  sdrc_init() - init the sdrc chip selects CS0 and CS1
 *  - early init routines, called from flash or
 *  SRAM.
 *******************************************************/
void sdrc_init(void)
{
#define EARLY_INIT 1
	do_sdrc_init(SDRC_CS0_OSET, EARLY_INIT); /* only init up first bank here */
}

/*************************************************************************
 * do_sdrc_init(): initialize the SDRAM for use.
 *  -code sets up SDRAM basic SDRC timings for CS0
 *  -optimal settings can be placed here, or redone after i2c
 *      inspection of board info
 *
 *  - code called ones in C-Stack only context for CS0 and a possible 2nd
 *      time depending on memory configuration from stack+global context
 **************************************************************************/
void do_sdrc_init(u32 offset, u32 early)
{
	u32 common = 0, cs0 = 0, pmask = 0, pass_type, mtype, mono = 0;

	if (offset == SDRC_CS0_OSET)
		cs0 = common = 1;	/* int regs shared between both chip select */

    pass_type = IP_DDR;

    /* If this is a 2nd pass init of a CS1, make it contiguous with CS0 */
    if (!early && (((mtype = get_mem_type()) == DDR_COMBO)
		       || (mtype == DDR_STACKED))) {
		if (mtype == DDR_COMBO) {
			pmask = BIT2;	/* if shared CKE don't use */
			pass_type = COMBO_DDR;	/* CS1 config */
			__raw_writel((__raw_readl(SDRC_POWER)) & ~pmask,
				     SDRC_POWER);
		}
		make_cs1_contiguous();
	}

next_mem_type:
	if (common) { /* do a SDRC reset between types to clear regs */
		__raw_writel(SOFTRESET, SDRC_SYSCONFIG);	/* reset sdrc */
		wait_on_value(BIT0, BIT0, SDRC_STATUS, 12000000);	/* wait on reset */
		__raw_writel(0, SDRC_SYSCONFIG);	/* clear soft reset */
		__raw_writel(SDP_SDRC_SHARING, SDRC_SHARING);
		/* If its a 3430 ES1.0 silicon, configure WAKEUPPROC to 1 as
		per Errata 1.22 */
		/* Need to change the condition to silicon and rev check */
		if(1)
			__raw_writel((__raw_readl(SDRC_POWER)) | WAKEUPPROC
							, SDRC_POWER);
#ifdef POWER_SAVE
		__raw_writel(__raw_readl(SMS_SYSCONFIG) | SMART_IDLE,
			     SMS_SYSCONFIG);
		__raw_writel(SDP_SDRC_SHARING | SMART_IDLE, SDRC_SHARING);
		__raw_writel((__raw_readl(SDRC_POWER) | BIT6), SDRC_POWER);
#endif
	}

    /* set MDCFG_0 values */
    if ((pass_type == IP_DDR) || (pass_type == STACKED)) {
		__raw_writel(SDP_SDRC_MDCFG_0_DDR, SDRC_MCFG_0 + offset);
		if (mono) /* Stacked with memory on CS1 only */
			__raw_writel(SDP_SDRC_MDCFG_MONO_DDR, SDRC_MCFG_0 + offset);
	} else if (pass_type == COMBO_DDR) {	/* (combo-CS0/CS1) */
		__raw_writel(SDP_COMBO_MDCFG_0_DDR, SDRC_MCFG_0 + offset);
	} else if (pass_type == IP_SDR) {	/* ip sdr-CS0 */
		__raw_writel(SDP_SDRC_MDCFG_0_SDR, SDRC_MCFG_0 + offset);
	}

    /* Set ACTIM values */
    if (cs0) {
		__raw_writel(SDP_SDRC_ACTIM_CTRLA_0, SDRC_ACTIM_CTRLA_0);
		__raw_writel(SDP_SDRC_ACTIM_CTRLB_0, SDRC_ACTIM_CTRLB_0);
	} else {
		__raw_writel(SDP_SDRC_ACTIM_CTRLA_0, SDRC_ACTIM_CTRLA_1);
		__raw_writel(SDP_SDRC_ACTIM_CTRLB_0, SDRC_ACTIM_CTRLB_1);
	}
	__raw_writel(SDP_SDRC_RFR_CTRL, SDRC_RFR_CTRL + offset);

	/* init sequence for mDDR/mSDR using manual commands (DDR is different) */
	__raw_writel(CMD_NOP, SDRC_MANUAL_0 + offset);
	sdelay(5000);	/* supposed to be 100us per design spec for mddr/msdr */
	__raw_writel(CMD_PRECHARGE, SDRC_MANUAL_0 + offset);
	__raw_writel(CMD_AUTOREFRESH, SDRC_MANUAL_0 + offset);
	__raw_writel(CMD_AUTOREFRESH, SDRC_MANUAL_0 + offset);

    /* Set MR0 values */
    if (pass_type == IP_SDR)
		__raw_writel(SDP_SDRC_MR_0_SDR, SDRC_MR_0 + offset);
	else
		__raw_writel(SDP_SDRC_MR_0_DDR, SDRC_MR_0 + offset);

    /* setup 343x DLL values (DDR only) */
    if (common && (pass_type != IP_SDR)) {
		__raw_writel(SDP_SDRC_DLLAB_CTRL, SDRC_DLLA_CTRL);
		sdelay(0x2000);	/* give time to lock, at least 1000 L3 */
	}
	sdelay(0x1000);

	if (mono)		/* Used if Stacked memory is on CS1 only */
		make_cs1_contiguous();	/* make CS1 appear at CS0 */

	if (mem_ok())
		return;		/* STACKED, other configured type */
	++pass_type;		/* IPDDR->COMBODDR->IPSDR for CS0 */
	goto next_mem_type;
}

void enable_gpmc_config(u32 * gpmc_config, u32 gpmc_base, u32 base, u32 size)
{
	__raw_writel(0, GPMC_CONFIG7 + gpmc_base);
	sdelay(1000);
	/* Delay for settling */
	__raw_writel(gpmc_config[0], GPMC_CONFIG1 + gpmc_base);
	__raw_writel(gpmc_config[1], GPMC_CONFIG2 + gpmc_base);
	__raw_writel(gpmc_config[2], GPMC_CONFIG3 + gpmc_base);
	__raw_writel(gpmc_config[3], GPMC_CONFIG4 + gpmc_base);
	__raw_writel(gpmc_config[4], GPMC_CONFIG5 + gpmc_base);
	__raw_writel(gpmc_config[5], GPMC_CONFIG6 + gpmc_base);
	/* Enable the config */
	__raw_writel((((size & 0xF) << 8) | ((base >> 24) & 0x3F) |
		      (1 << 6)), GPMC_CONFIG7 + gpmc_base);
	sdelay(2000);
}

void gpmc_dump_config(int cs)
{
  u32 gpmc_base = GPMC_CONFIG_CS0 + (cs * GPMC_CONFIG_WIDTH);
  u32 config7;
  u32 base, len;

  config7 = __raw_readl(GPMC_CONFIG7 + gpmc_base);
  if (config7 & (1<<6)) {
    len = (((config7 >> 7 & 0xf) ^ 0xf) + 1) << 24;
    base = (config7 & 0x3F) << 24;
  printf("%d:%08x %08x %08x %08x %08x %08x %03x %08x-%08x\n",
	 cs, 
	 __raw_readl(GPMC_CONFIG1 + gpmc_base), __raw_readl(GPMC_CONFIG2 + gpmc_base), 
	 __raw_readl(GPMC_CONFIG3 + gpmc_base), __raw_readl(GPMC_CONFIG4 + gpmc_base), 
	 __raw_readl(GPMC_CONFIG5 + gpmc_base), __raw_readl(GPMC_CONFIG6 + gpmc_base), 
	 config7, base, base+len-1);
  }
}

#if defined (CFG_OMAP_DUMP_GPMC)
void do_dump_cs (cmd_tbl_t *cmdtp, int flah, int argc, char *argv[])
{
	int i;

	printf("CS: GPMC_CONFIG [%08x]\n", __raw_readl(GPMC_CONFIG));
	printf("CS: GPMC_IRQENABLE [%08x]\n", __raw_readl(GPMC_IRQENABLE));
	printf("CS: GPMC_TIMEOUT_CONTROL [%08x]\n", __raw_readl(GPMC_TIMEOUT_CONTROL));
	for (i=0; i<8; ++i)
		gpmc_dump_config(i);

	return 0;
}

U_BOOT_CMD(
	dump_cs,	1,	1,	do_dump_cs,
	"dump_cs - display OMAP Chip Select registers\n",
	"\n    - display active OMAP Chip Select registers\n"
);
#endif
/*****************************************************
 * gpmc_init(): init gpmc bus
 * Init GPMC for x16, MuxMode (SDRAM in x32).
 * This code can only be executed from SRAM or SDRAM.
 *****************************************************/
void gpmc_init(void)
{
/* putting a blanket check on GPMC based on ZeBu for now */
	u32 mux = 0, mwidth;
	u32 *gpmc_config = NULL;
	u32 gpmc_base = 0;
	u32 base = 0;
	u32 size = 0;
	u32 f_off = CFG_MONITOR_LEN;
	u32 f_sec = 0;
	u32 config = 0;
	u16 rcrval;
	unsigned char *config_sel = NULL;
	u32 i=0;
	
	mux = BIT9;
	mwidth = get_gpmc0_width();

	/* global settings */
	__raw_writel(0x10, GPMC_SYSCONFIG);	/* smart idle */
	__raw_writel(0x0, GPMC_IRQENABLE);	/* isr's sources masked */
	__raw_writel(0, GPMC_TIMEOUT_CONTROL);	/* timeout disable */

	config = __raw_readl(GPMC_CONFIG);
#if 1
	config &= (~0xd00); // clear wait lines for all but WAIT1 as
			    // used by NOR flash
#else
	config &= (~0xf00);
#endif
	__raw_writel(config, GPMC_CONFIG);

	/* Disable the GPMC0 config set by ROM code
	 * It conflicts with our MPDB (both at 0x08000000)
	 */
	__raw_writel(0 , GPMC_CONFIG7 + GPMC_CONFIG_CS0);
	sdelay(1000);
	
	/* CS 0 */
	gpmc_config = gpmc_m_nand;
	gpmc_base = GPMC_CONFIG_CS0 + (0 * GPMC_CONFIG_WIDTH);
	base = PISMO1_NAND_BASE;
	size = PISMO1_NAND_SIZE;
	enable_gpmc_config(gpmc_config, gpmc_base, base, size);

	f_off = SMNAND_ENV_OFFSET;
	f_sec = SZ_128K;
#if (CONFIG_COMMANDS & CFG_CMD_NAND)
	is_nand = 1;
	nand_cs_base = gpmc_base;
#endif
	/* env setup */
	boot_flash_base = base;
	boot_flash_off = f_off;
	boot_flash_sec = f_sec;
	boot_flash_env_addr = f_off;

#ifdef ENV_IS_VARIABLE
	boot_env_get_char_spec = nand_env_get_char_spec;
	boot_env_init = nand_env_init;
	boot_saveenv = nand_saveenv;
	boot_env_relocate_spec = nand_env_relocate_spec;
	env_ptr = 0;	/* This gets filled elsewhere!! */
	env_name_spec = nand_env_name_spec;
#endif
	/* CS 1 */
	gpmc_config = gpmc_enet;
	gpmc_base = GPMC_CONFIG_CS0 + (1 * GPMC_CONFIG_WIDTH);	
	enable_gpmc_config(gpmc_config, gpmc_base, DEBUG_BASE, DBG_MPDB_SIZE);

#ifdef OPTIONAL_NOR
	/* CS 2 (fixme -- sizes for optional s-nor)*/
	gpmc_config = gpmc_stnor;
	gpmc_config[0] |= mux | TYPE_NOR | mwidth;
	gpmc_base = GPMC_CONFIG_CS0 + (2 * GPMC_CONFIG_WIDTH);	
	enable_gpmc_config(gpmc_config, gpmc_base, DEBUG_BASE, DBG_MPDB_SIZE);
	
	/* handle flash probe setup */
	f_sec = SZ_128K;
	NOR_MAX_FLASH_BANKS = 2;
	size = PISMO1_NOR_SIZE;
	for(i=0; i < NOR_MAX_FLASH_BANKS; i++) {
		NOR_FLASH_BANKS_LIST[i] = 
		FLASH_BASE_SDPV1 + PHYS_FLASH_SIZE*i;
	}
#endif


	/* CS 3 (CompactFlash)*/
	gpmc_config = gpmc_m_cf;
	gpmc_base = GPMC_CONFIG_CS0 + (3 * GPMC_CONFIG_WIDTH);	
	enable_gpmc_config(gpmc_config, gpmc_base, 0x18000000, GPMC_SIZE_16M);

	/* CS 6 (ISP1760)*/
	gpmc_config = gpmc_m_isp1760;
	gpmc_base = GPMC_CONFIG_CS0 + (6 * GPMC_CONFIG_WIDTH);	
	enable_gpmc_config(gpmc_config, gpmc_base, 0x1c000000, GPMC_SIZE_16M);
}

void fix_flash_sync()
{
#ifdef CFG_FIX_FLASH_SYNC
	u32 *gpmc_config = NULL;
	u32 gpmc_base;
	u32 config;
	u16 rcrval;

	/* CS 2 - Check if NOR is in sync, and if not, then put flash into
	   sync mode, and GPMC into sync */
	// gpmc_dump_config(2);
	gpmc_base = GPMC_CONFIG_CS0 + (2 * GPMC_CONFIG_WIDTH);	
	config = __raw_readl(GPMC_CONFIG1 + gpmc_base);
	if (!(config & TYPE_READTYPE)) {
		// Its in async mode, we have to put it into sync.

		puts ("FLASH: initialize in sync mode\n");

		// clear WAIT1 polarity
		__raw_writel(__raw_readl(GPMC_CONFIG) & ~0x200, GPMC_CONFIG);

		// clear GPMC_TIMEOUT
		__raw_writel(0x0, GPMC_TIMEOUT_CONTROL);

		// First set the GPMC for async.
		gpmc_config = gpmc_stnor_async;
		enable_gpmc_config(gpmc_config, gpmc_base, FLASH_BASE, GPMC_SIZE_64M);
		// Second, tell flash to go into sync mode.

		// gpmc_dump_config(2);

		// 1st NOR cycle, send read config register setup 0x60
		*(volatile u16 *)FLASH_BASE = 0x0060;

		// 2nd NOR cycle, send 0x03 to latch in read
		// configuration register setttings, located on A[15:0]
		rcrval = FLASH_28FxxxP30_RCR_LC(4) | FLASH_28FxxxP30_RCR_WP |
		  FLASH_28FxxxP30_RCR_BS | FLASH_28FxxxP30_RCR_CE |
		  FLASH_28FxxxP30_RCR_BW | FLASH_28FxxxP30_RCR_BL(FLASH_28FxxxP30_BL_4);
		*(volatile u16 *)(FLASH_BASE | (rcrval << 1)) = 0x0003;

		// Third, set GPMC for sync.
		gpmc_config = gpmc_stnor_sync;
		enable_gpmc_config(gpmc_config, gpmc_base, FLASH_BASE, GPMC_SIZE_64M);
		// And lastly, set the WAIT1 polarity high
		__raw_writel(__raw_readl(GPMC_CONFIG) | 0x200, GPMC_CONFIG);

		// gpmc_dump_config(2);

	}
#endif
}
