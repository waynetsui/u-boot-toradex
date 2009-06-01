/*
 * Copyright (C) 2007-2009 Freescale Semiconductor, Inc. All rights reserved.
 *
 * Modified by Jason Jin, Jason.jin@freescale.com,
 * 	       Mingkai hu, Mingkai.hu@freescale.com
 *
 * (C) Copyright 2003 Motorola Inc.
 * Modified by Xianghua Xiao, X.Xiao@motorola.com
 *
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

#include <common.h>
#include <watchdog.h>
#include <asm/processor.h>
#include <ioports.h>
#include <asm/io.h>
#include <asm/mmu.h>
#include <asm/fsl_law.h>
#include "mp.h"

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_MPC8536
extern void fsl_serdes_init(void);
#endif

#ifdef CONFIG_QE
extern qe_iop_conf_t qe_iop_conf_tab[];
extern void qe_config_iopin(u8 port, u8 pin, int dir,
				int open_drain, int assign);
extern void qe_init(uint qe_base);
extern void qe_reset(void);

static void config_qe_ioports(void)
{
	u8      port, pin;
	int     dir, open_drain, assign;
	int     i;

	for (i = 0; qe_iop_conf_tab[i].assign != QE_IOP_TAB_END; i++) {
		port		= qe_iop_conf_tab[i].port;
		pin		= qe_iop_conf_tab[i].pin;
		dir		= qe_iop_conf_tab[i].dir;
		open_drain	= qe_iop_conf_tab[i].open_drain;
		assign		= qe_iop_conf_tab[i].assign;
		qe_config_iopin(port, pin, dir, open_drain, assign);
	}
}
#endif

#ifdef CONFIG_CPM2
void config_8560_ioports (volatile ccsr_cpm_t * cpm)
{
	int portnum;

	for (portnum = 0; portnum < 4; portnum++) {
		uint pmsk = 0,
		     ppar = 0,
		     psor = 0,
		     pdir = 0,
		     podr = 0,
		     pdat = 0;
		iop_conf_t *iopc = (iop_conf_t *) & iop_conf_tab[portnum][0];
		iop_conf_t *eiopc = iopc + 32;
		uint msk = 1;

		/*
		 * NOTE:
		 * index 0 refers to pin 31,
		 * index 31 refers to pin 0
		 */
		while (iopc < eiopc) {
			if (iopc->conf) {
				pmsk |= msk;
				if (iopc->ppar)
					ppar |= msk;
				if (iopc->psor)
					psor |= msk;
				if (iopc->pdir)
					pdir |= msk;
				if (iopc->podr)
					podr |= msk;
				if (iopc->pdat)
					pdat |= msk;
			}

			msk <<= 1;
			iopc++;
		}

		if (pmsk != 0) {
			volatile ioport_t *iop = ioport_addr (cpm, portnum);
			uint tpmsk = ~pmsk;

			/*
			 * the (somewhat confused) paragraph at the
			 * bottom of page 35-5 warns that there might
			 * be "unknown behaviour" when programming
			 * PSORx and PDIRx, if PPARx = 1, so I
			 * decided this meant I had to disable the
			 * dedicated function first, and enable it
			 * last.
			 */
			iop->ppar &= tpmsk;
			iop->psor = (iop->psor & tpmsk) | psor;
			iop->podr = (iop->podr & tpmsk) | podr;
			iop->pdat = (iop->pdat & tpmsk) | pdat;
			iop->pdir = (iop->pdir & tpmsk) | pdir;
			iop->ppar |= ppar;
		}
	}
}
#endif

/* We run cpu_init_early_f in AS = 1 */
void cpu_init_early_f(void)
{
	/* Pointer is writable since we allocated a register for it */
	gd = (gd_t *) (CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_GBL_DATA_OFFSET);

	/* Clear initial global data */
	memset ((void *) gd, 0, sizeof (gd_t));

#ifndef CONFIG_NAND_U_BOOT
	set_tlb(0, CONFIG_SYS_CCSRBAR, CONFIG_SYS_CCSRBAR_PHYS,
		MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		1, 0, BOOKE_PAGESZ_4K, 0);

#endif
	/* set up CCSR if we want it moved */
#if (CONFIG_SYS_CCSRBAR_DEFAULT != CONFIG_SYS_CCSRBAR_PHYS)
	{
		u32 temp;
		volatile u32 *ccsr_virt =
			(volatile u32 *)(CONFIG_SYS_CCSRBAR + 0x1000);

		set_tlb(0, (u32)ccsr_virt, CONFIG_SYS_CCSRBAR_DEFAULT,
			MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
			1, 1, BOOKE_PAGESZ_4K, 0);

		temp = in_be32(ccsr_virt);
		out_be32(ccsr_virt, CONFIG_SYS_CCSRBAR_PHYS >> 12);
		temp = in_be32((volatile u32 *)CONFIG_SYS_CCSRBAR);
	}
#endif

	init_laws();
#ifndef CONFIG_NAND_U_BOOT
/* To protect DDR TLB from getting invalid */
	invalidate_tlb(0);
#endif
	init_tlbs();
}

#ifdef CONFIG_SDCARD_U_BOOT
/*
 * Used for booting from SD.
 * The ROM code has set a 4G space in tlb0, and on the sd data structure,
 * there also has a law0 setting for DDR(usually 1G, can be changed).
 * we try to use those space as much as we can and will change those
 * setting later.
 * Change the CCSRBAR firstly as needed.
*/
void cpu_init_early_f_sd(void)
{
	unsigned int law_size;

#if !defined(CONFIG_SYS_RAMBOOT)
	/* set up CCSR if we want it moved */
#if (CONFIG_SYS_CCSRBAR_DEFAULT != CONFIG_SYS_CCSRBAR_PHYS)
	{
		u32 temp;
		temp = in_be32((volatile u32 *)CONFIG_SYS_CCSRBAR_DEFAULT);
		out_be32((volatile u32 *)CONFIG_SYS_CCSRBAR_DEFAULT, CONFIG_SYS_CCSRBAR_PHYS >> 12);

		temp = in_be32((volatile u32 *)CONFIG_SYS_CCSRBAR);
	}
#endif
#endif

	/* Pointer is writable since we allocated a register for it.
	 * the gd->used_law will be used in the init_laws(),
	 * we prepared it here.
	 */
	gd = (gd_t *) (CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_GBL_DATA_OFFSET);

	/* Clear initial global data */
	memset((void *)gd, 0, sizeof(gd_t));

	/* law0 has been set for DDR in the SD data structure, we defined a
	 * CFG_RESERVED_LAW macro in the header file, try to avoid it being
	 * overlaped in the init_laws().
	 */
	init_laws();

	/*set a temp tlb15 in address space 1 for DDR*/
	set_tlb(1, CONFIG_SYS_DDR_SDRAM_BASE, CONFIG_SYS_DDR_SDRAM_BASE,
		MAS3_SX|MAS3_SW|MAS3_SR, 0,
		1, 15, BOOKE_PAGESZ_1G, 1);

	/*And tlb14 for CCSR registers*/
	set_tlb(1, CONFIG_SYS_CCSRBAR, CONFIG_SYS_CCSRBAR_PHYS,
		MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		1, 14, BOOKE_PAGESZ_1M, 1);
}

/*This run in space 1 */
void cpu_init_early_f_sd_continue(void)
{
	/* Disable tlb0 which intialized in the ROM
	 * Normal init for space 0
	 */
	disable_tlb(0);
	init_tlbs();

	/*Reinitialize the tlb for DDR in space 0*/
	setup_ddr_tlbs(CONFIG_SYS_SDRAM_SIZE);
}
#endif

/*
 * Breathe some life into the CPU...
 *
 * Set up the memory map
 * initialize a bunch of registers
 */

void cpu_init_f (void)
{
	volatile ccsr_lbc_t *memctl = (void *)(CONFIG_SYS_MPC85xx_LBC_ADDR);
	extern void m8560_cpm_reset (void);
#ifdef CONFIG_MPC8548
	ccsr_local_ecm_t *ecm = (void *)(CONFIG_SYS_MPC85xx_ECM_ADDR);
	uint svr = get_svr();

	/*
	 * CPU2 errata workaround: A core hang possible while executing
	 * a msync instruction and a snoopable transaction from an I/O
	 * master tagged to make quick forward progress is present.
	 * Fixed in silicon rev 2.1.
	 */
	if ((SVR_MAJ(svr) == 1) || ((SVR_MAJ(svr) == 2 && SVR_MIN(svr) == 0x0)))
		out_be32(&ecm->eebpcr, in_be32(&ecm->eebpcr) | (1 << 16));
#endif

#ifndef CONFIG_NAND_U_BOOT
	disable_tlb(14);
	disable_tlb(15);
#endif

#ifdef CONFIG_CPM2
	config_8560_ioports((ccsr_cpm_t *)CONFIG_SYS_MPC85xx_CPM_ADDR);
#endif

	/* Map banks 0 and 1 to the FLASH banks 0 and 1 at preliminary
	 * addresses - these have to be modified later when FLASH size
	 * has been determined
	 */
#if defined(CONFIG_SYS_OR0_REMAP)
	memctl->or0 = CONFIG_SYS_OR0_REMAP;
#endif
#if defined(CONFIG_SYS_OR1_REMAP)
	memctl->or1 = CONFIG_SYS_OR1_REMAP;
#endif

	/* now restrict to preliminary range */
	/* if cs1 is already set via debugger, leave cs0/cs1 alone */
	if (! memctl->br1 & 1) {
#if defined(CONFIG_SYS_BR0_PRELIM) && defined(CONFIG_SYS_OR0_PRELIM)
		memctl->br0 = CONFIG_SYS_BR0_PRELIM;
		memctl->or0 = CONFIG_SYS_OR0_PRELIM;
#endif

#if defined(CONFIG_SYS_BR1_PRELIM) && defined(CONFIG_SYS_OR1_PRELIM)
		memctl->or1 = CONFIG_SYS_OR1_PRELIM;
		memctl->br1 = CONFIG_SYS_BR1_PRELIM;
#endif
	}

#if defined(CONFIG_SYS_BR2_PRELIM) && defined(CONFIG_SYS_OR2_PRELIM)
	memctl->or2 = CONFIG_SYS_OR2_PRELIM;
	memctl->br2 = CONFIG_SYS_BR2_PRELIM;
#endif

#if defined(CONFIG_SYS_BR3_PRELIM) && defined(CONFIG_SYS_OR3_PRELIM)
	memctl->or3 = CONFIG_SYS_OR3_PRELIM;
	memctl->br3 = CONFIG_SYS_BR3_PRELIM;
#endif

#if defined(CONFIG_SYS_BR4_PRELIM) && defined(CONFIG_SYS_OR4_PRELIM)
	memctl->or4 = CONFIG_SYS_OR4_PRELIM;
	memctl->br4 = CONFIG_SYS_BR4_PRELIM;
#endif

#if defined(CONFIG_SYS_BR5_PRELIM) && defined(CONFIG_SYS_OR5_PRELIM)
	memctl->or5 = CONFIG_SYS_OR5_PRELIM;
	memctl->br5 = CONFIG_SYS_BR5_PRELIM;
#endif

#if defined(CONFIG_SYS_BR6_PRELIM) && defined(CONFIG_SYS_OR6_PRELIM)
	memctl->or6 = CONFIG_SYS_OR6_PRELIM;
	memctl->br6 = CONFIG_SYS_BR6_PRELIM;
#endif

#if defined(CONFIG_SYS_BR7_PRELIM) && defined(CONFIG_SYS_OR7_PRELIM)
	memctl->or7 = CONFIG_SYS_OR7_PRELIM;
	memctl->br7 = CONFIG_SYS_BR7_PRELIM;
#endif

#if defined(CONFIG_CPM2)
	m8560_cpm_reset();
#endif
#ifdef CONFIG_QE
	/* Config QE ioports */
	config_qe_ioports();
#endif
#if defined(CONFIG_MPC8536)
	fsl_serdes_init();
#endif

}


/*
 * Initialize L2 as cache.
 *
 * The newer 8548, etc, parts have twice as much cache, but
 * use the same bit-encoding as the older 8555, etc, parts.
 *
 */

int cpu_init_r(void)
{
	puts ("L2:    ");

#if defined(CONFIG_L2_CACHE)
	volatile ccsr_l2cache_t *l2cache = (void *)CONFIG_SYS_MPC85xx_L2_ADDR;
	volatile uint cache_ctl;
	uint svr, ver;
	uint l2srbar;
	u32 l2siz_field;

	svr = get_svr();
	ver = SVR_SOC_VER(svr);

	asm("msync;isync");
	cache_ctl = l2cache->l2ctl;
	l2siz_field = (cache_ctl >> 28) & 0x3;

	switch (l2siz_field) {
	case 0x0:
		printf(" unknown size (0x%08x)\n", cache_ctl);
		return -1;
		break;
	case 0x1:
		if (ver == SVR_8540 || ver == SVR_8560   ||
		    ver == SVR_8541 || ver == SVR_8541_E ||
		    ver == SVR_8555 || ver == SVR_8555_E) {
			puts("128 KB ");
			/* set L2E=1, L2I=1, & L2BLKSZ=1 (128 Kbyte) */
			cache_ctl = 0xc4000000;
		} else {
			puts("256 KB ");
			cache_ctl = 0xc0000000; /* set L2E=1, L2I=1, & L2SRAM=0 */
		}
		break;
	case 0x2:
		if (ver == SVR_8540 || ver == SVR_8560   ||
		    ver == SVR_8541 || ver == SVR_8541_E ||
		    ver == SVR_8555 || ver == SVR_8555_E) {
			puts("256 KB ");
			/* set L2E=1, L2I=1, & L2BLKSZ=2 (256 Kbyte) */
			cache_ctl = 0xc8000000;
		} else {
			puts ("512 KB ");
			/* set L2E=1, L2I=1, & L2SRAM=0 */
			cache_ctl = 0xc0000000;
		}
		break;
	case 0x3:
		puts("1024 KB ");
		/* set L2E=1, L2I=1, & L2SRAM=0 */
		cache_ctl = 0xc0000000;
		break;
	}

	if (l2cache->l2ctl & 0x80000000) {
		puts("already enabled");
		l2srbar = l2cache->l2srbar0;
#ifdef CONFIG_SYS_INIT_L2_ADDR
		if (l2cache->l2ctl & 0x00010000 && l2srbar >= CONFIG_SYS_FLASH_BASE) {
			l2srbar = CONFIG_SYS_INIT_L2_ADDR;
			l2cache->l2srbar0 = l2srbar;
			printf("moving to 0x%08x", CONFIG_SYS_INIT_L2_ADDR);
		}
#endif /* CONFIG_SYS_INIT_L2_ADDR */
		puts("\n");
	} else {
		asm("msync;isync");
		l2cache->l2ctl = cache_ctl; /* invalidate & enable */
		asm("msync;isync");
		puts("enabled\n");
	}
#else
	puts("disabled\n");
#endif
#ifdef CONFIG_QE
	uint qe_base = CONFIG_SYS_IMMR + 0x00080000; /* QE immr base */
	qe_init(qe_base);
	qe_reset();
#endif

#if defined(CONFIG_MP)
	setup_mp();
#endif
	return 0;
}
