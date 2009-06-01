/*
 * Copyright (C) 2009 Freescale Semiconductor, Inc. All rights reserved.
 * Copyright (C) 2003  Motorola,Inc.
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

/* U-Boot Startup Code for Motorola 85xx PowerPC based Embedded Boards
 *
 * The processor starts at 0xfffffffc and the code is first executed in the
 * last 4K page(0xfffff000-0xffffffff) in flash/rom.
 *
 */

#include <config.h>
#include <mpc85xx.h>
#include <version.h>

#define _LINUX_CONFIG_H 1	/* avoid reading Linux autoconf.h file	*/

#include <ppc_asm.tmpl>
#include <ppc_defs.h>

#include <asm/cache.h>
#include <asm/mmu.h>

#ifndef	 CONFIG_IDENT_STRING
#define	 CONFIG_IDENT_STRING ""
#endif

/*
 * Set up GOT: Global Offset Table
 *
 * Use r14 to access the GOT
 */
	START_GOT
	GOT_ENTRY(_GOT_TABLE_)
	END_GOT

/*
 * e500 Startup -- after reset only the last 4KB of the effective
 * address space is mapped in the MMU L2 TLB1 Entry0. The .bootpg
 * section is located at THIS LAST page and basically does three
 * things: clear some registers, set up exception tables and
 * add more TLB entries for 'larger spaces'(e.g. the boot rom) to
 * continue the boot procedure.

 * Once the boot rom is mapped by TLB entries we can proceed
 * with normal startup.
 *
 */

#define	ENTRY_START \
	mflr	r1 	;	\
	bl	0f 	;

#define	ENTRY_END \
0:	mflr	r0	;	\
	mtlr	r1	;	\
	blr		;


	.text
	.globl	_start
_start:
	.long	0x27051956		/* U-BOOT Magic Number */
	.globl	version_string
version_string:
	.ascii U_BOOT_VERSION
	.ascii " (", __DATE__, " - ", __TIME__, ")"
	.ascii CONFIG_IDENT_STRING, "\0"

	.align	4
	.globl _start_e500

_start_e500:

	/* clear registers/arrays not reset by hardware */

	/* L1 */
	li	r0,2
	mtspr	L1CSR0,r0	/* invalidate d-cache */
	mtspr	L1CSR1,r0 	/* invalidate i-cache */

	mfspr	r1,DBSR
	mtspr	DBSR,r1		/* Clear all valid bits */

	/* Enable L1 Caches early */

	lis	r2,L1CSR0_CPE@H	/* enable parity */
	ori	r2,r2,L1CSR0_DCE
	mtspr	L1CSR0,r2	/* enable L1 Dcache */
	isync
	mtspr	L1CSR1,r2	/* enable L1 Icache */
	isync
	msync

	bl	tlb_entry
	mr	r5,r0
	lwzu	r4,0(r5)	/* how many TLB entries we actually use */
	mtctr	r4

0:	lwzu	r6,4(r5)
	lwzu	r7,4(r5)
	lwzu	r8,4(r5)
	lwzu	r9,4(r5)
	mtspr	MAS0,r6
	mtspr	MAS1,r7
	mtspr	MAS2,r8
	mtspr	MAS3,r9
	isync
	msync
	tlbwe
	isync
	bdnz	0b

/* NOTE: if no enough space for code, try to use default CCSR */
#if (CONFIG_SYS_CCSRBAR_DEFAULT != CONFIG_SYS_CCSRBAR)
	/* Special sequence needed to update CCSRBAR itself */
	lis	r4,CONFIG_SYS_CCSRBAR_DEFAULT@h
	ori	r4,r4,CONFIG_SYS_CCSRBAR_DEFAULT@l

	lis	r5,CONFIG_SYS_CCSRBAR@h
	ori	r5,r5,CONFIG_SYS_CCSRBAR@l
	srwi	r6,r5,12
	stw	r6,0(r4)
	isync

	lis	r5, 0xffff
	ori	r5,r5, 0xf000
	lwz	r5,0(r5)
	isync

	lis	r3,CONFIG_SYS_CCSRBAR@h
	lwz	r5,CONFIG_SYS_CCSRBAR@l(r3)
	isync
#endif

	lis     r7,CONFIG_SYS_CCSRBAR@h
	ori     r7,r7,CONFIG_SYS_CCSRBAR@l
	bl	law_entry
	mr	r6,r0
	lwzu	r5,0(r6)	/* how many windows we actually use */
	mtctr	r5
	li	r2,0x0c28	/* the first pair is reserved for */
	li	r1,0x0c30	/* boot-over-rio-or-pci */
0:	lwzu	r4,4(r6)
	lwzu	r3,4(r6)
	stwx	r4,r7,r2
	stwx	r3,r7,r1
	addi	r2,r2,0x0020
	addi	r1,r1,0x0020
	bdnz	0b

	/* Clear and set up some registers. */
	li	r0,0x0000
	lis	r1,0xffff
	mtspr	DEC,r0			/* prevent dec exceptions */
	mttbl	r0			/* prevent fit & wdt exceptions */
	mttbu	r0
	mtspr	TSR,r1			/* clear all timer exception status */
	mtspr	TCR,r0			/* disable all */
	mtspr	ESR,r0			/* clear exception syndrome register */
	mtspr	MCSR,r0			/* machine check syndrome register */
	mtxer	r0			/* clear integer exception register */
	isync

	/* Lock L1 DCache as used for initial RAM */

	/* Allocate Initial RAM in data cache.
	 */
	lis	r3,CONFIG_SYS_INIT_RAM_ADDR@h
	ori	r3,r3,CONFIG_SYS_INIT_RAM_ADDR@l
	li	r2,512 /* 512*32=16K */
	mtctr	r2
	li	r0,0
1:
	dcbz	r0,r3
	dcbtls	0,r0,r3
	addi	r3,r3,CONFIG_SYS_CACHELINE_SIZE
	bdnz	1b

	/* keep current running address = link address */
	lis	r3,CONFIG_SYS_NAND_BASE@h
	ori	r3,r3,CONFIG_SYS_NAND_BASE@l
	addi	r3,r3,_start_cont - _start + _START_OFFSET
	mtlr	r3

	li	r0,0
	mtspr	SRR1,r0		/* Keep things disabled for now */
	mflr	r1
	mtspr	SRR0,r1
	rfi
	isync

	.globl	_start_cont
_start_cont:
	/* Setup the stack in initial RAM,could be L2-as-SRAM or L1 dcache*/
	lis	r1,CONFIG_SYS_INIT_RAM_ADDR@h
	ori	r1,r1,CONFIG_SYS_INIT_SP_OFFSET@l

	li	r0,0
	stwu	r0,-4(r1)
	stwu	r0,-4(r1)		/* Terminate call chain */

	stwu	r1,-8(r1)		/* Save back chain and move SP */
	lis	r0,RESET_VECTOR@h	/* Address of reset vector */
	ori	r0,r0,RESET_VECTOR@l
	stwu	r1,-8(r1)		/* Save back chain and move SP */
	stw	r0,+12(r1)		/* Save return addr (underflow vect) */

	GET_GOT
	bl	cpu_init_f
	bl	board_init_f_nand
	isync

	.globl	tlb_entry
tlb_entry:
	ENTRY_START

	/*
	 * Number of TLB0 and TLB1 entries in the following table
	 */
	.long (tlb_end-tlb_start)/16
tlb_start:
	/* TLB 0 */
	/* *I*G* - Initial CCSRBAR mapping */
	.long FSL_BOOKE_MAS0(0, 1, 0)
	.long FSL_BOOKE_MAS1(1, 0, 0, 0, BOOKE_PAGESZ_4K)
	.long FSL_BOOKE_MAS2(CONFIG_SYS_CCSRBAR_DEFAULT,
			MAS2_I | MAS2_G)
	.long FSL_BOOKE_MAS3(CONFIG_SYS_CCSRBAR_DEFAULT,
			0, MAS3_SX | MAS3_SW | MAS3_SR)

	/*
	 * TLB0		16K	Cacheable, guarded
	 * Temporary Global data for initialization
	 *
	 * Use four 4K TLB0 entries.  These entries must be cacheable
	 * as they provide the bootstrap memory before the memory
	 * controler and real memory have been configured.
	 *
	 * These entries end up at TLB0 Indicies 0x10, 0x14, 0x18 and 0x1c,
	 * and must not collide with other TLB0 entries.
	 */
	.long FSL_BOOKE_MAS0(0, 0, 0)
	.long FSL_BOOKE_MAS1(1, 0, 0, 0, BOOKE_PAGESZ_4K)
	.long FSL_BOOKE_MAS2(CONFIG_SYS_INIT_RAM_ADDR, MAS2_G)
	.long FSL_BOOKE_MAS3(CONFIG_SYS_INIT_RAM_ADDR,
			0, MAS3_SX | MAS3_SW | MAS3_SR)

	.long FSL_BOOKE_MAS0(0, 0, 0)
	.long FSL_BOOKE_MAS1(1, 0, 0, 0, BOOKE_PAGESZ_4K)
	.long FSL_BOOKE_MAS2(CONFIG_SYS_INIT_RAM_ADDR + 4 * 1024,
			MAS2_G)
	.long FSL_BOOKE_MAS3(CONFIG_SYS_INIT_RAM_ADDR + 4 * 1024,
			0, MAS3_SX | MAS3_SW | MAS3_SR)

	.long FSL_BOOKE_MAS0(0, 0, 0)
	.long FSL_BOOKE_MAS1(1, 0, 0, 0, BOOKE_PAGESZ_4K)
	.long FSL_BOOKE_MAS2(CONFIG_SYS_INIT_RAM_ADDR + 8 * 1024,
			MAS2_G)
	.long FSL_BOOKE_MAS3(CONFIG_SYS_INIT_RAM_ADDR + 8 * 1024,
			0, MAS3_SX | MAS3_SW | MAS3_SR)

	.long FSL_BOOKE_MAS0(0, 0, 0)
	.long FSL_BOOKE_MAS1(1, 0, 0, 0, BOOKE_PAGESZ_4K)
	.long FSL_BOOKE_MAS2(CONFIG_SYS_INIT_RAM_ADDR + 12 * 1024,
			MAS2_G)
	.long FSL_BOOKE_MAS3(CONFIG_SYS_INIT_RAM_ADDR + 12 * 1024,
			0, MAS3_SX | MAS3_SW | MAS3_SR)
/*
	.long FSL_BOOKE_MAS0(0, 0, 0)
	.long FSL_BOOKE_MAS1(1, 0, 0, 0, BOOKE_PAGESZ_4K)
	.long FSL_BOOKE_MAS2(PIXIS_BASE, MAS2_I | MAS2_G)
	.long FSL_BOOKE_MAS3(PIXIS_BASE, 0, MAS3_SX | MAS3_SW | MAS3_SR)
*/
	/* TLB 1 */
	/* *I*** - Covers boot page */
	.long FSL_BOOKE_MAS0(1, 0, 0)
	.long FSL_BOOKE_MAS1(1, 1, 0, 0, BOOKE_PAGESZ_4K)
	.long FSL_BOOKE_MAS2(0xfffff000, MAS2_I | MAS2_G)
	.long FSL_BOOKE_MAS3(0xfffff000, 0, MAS3_SX | MAS3_SW | MAS3_SR)

	/* *I*G* - CCSRBAR */
	.long FSL_BOOKE_MAS0(1, 1, 0)
	.long FSL_BOOKE_MAS1(1, 1, 0, 0, BOOKE_PAGESZ_1M)
	.long FSL_BOOKE_MAS2(CONFIG_SYS_CCSRBAR, MAS2_I | MAS2_G)
	.long FSL_BOOKE_MAS3(CONFIG_SYS_CCSRBAR, 0, MAS3_SX | MAS3_SW | MAS3_SR)

	/* *I*G* - NAND Flash, localbus */
	.long FSL_BOOKE_MAS0(1, 8, 0)
	.long FSL_BOOKE_MAS1(1, 1, 0, 0, BOOKE_PAGESZ_256K)
	.long FSL_BOOKE_MAS2(CONFIG_SYS_NAND_BASE, MAS2_I | MAS2_G)
	.long FSL_BOOKE_MAS3(CONFIG_SYS_NAND_BASE, 0,
			MAS3_SX | MAS3_SW | MAS3_SR)

	/* DDR SDRAM */
	.long FSL_BOOKE_MAS0(1, 9, 0)
	.long FSL_BOOKE_MAS1(1, 1, 0, 0, BOOKE_PAGESZ_1G)
	.long FSL_BOOKE_MAS2(0, 0)
	.long FSL_BOOKE_MAS3(0, 0, MAS3_SX | MAS3_SW | MAS3_SR)

tlb_end:
	ENTRY_END

	.globl	law_entry
law_entry:
	ENTRY_START

	.long (law_end-law_start)/8
law_start:
	.long	0
	.long	LAWAR_EN | LAWAR_TRGT_IF_DDR1 | (LAWAR_SIZE & LAWAR_SIZE_512M)

	/* placeholder for LAWAR_TRGT_IF_DDR2 */
	.long	0
	.long	0

//	.long	(PIXIS_BASE>>12) & 0xfffff
//	.long	LAWAR_EN | LAWAR_TRGT_IF_LBC | (LAWAR_SIZE & LAWAR_SIZE_4K)

	.long	(CONFIG_SYS_NAND_BASE>>12) & 0xfffff
	.long	LAWAR_EN | LAWAR_TRGT_IF_LBC | (LAWAR_SIZE & LAWAR_SIZE_256K)

law_end:
	ENTRY_END
/*
 * void relocate_code (addr_sp, gd, addr_moni)
 *
 * This "function" does not return, instead it continues in RAM
 * after relocating the monitor code.
 *
 * r3 = dest
 * r4 = src
 * r5 = length in bytes
 * r6 = cachelinesize
 */
	.globl	relocate_code
relocate_code:

	mr	r1,r3		/* Set new stack pointer		*/
	mr	r9,r4		/* Save copy of Init Data pointer	*/
	mr	r10,r5		/* Save copy of Destination Address	*/

	mr	r3,r5				/* Destination Address	*/

        lis     r4, CONFIG_SYS_NAND_BASE@h      /* Source      Address */
        ori     r4, r4, CONFIG_SYS_NAND_BASE@l
        li      r5, 0x1000                      /* 4 kbyte bootloader */
        li      r6, CONFIG_SYS_CACHELINE_SIZE   /* Cache Line Size      */

	/*
	 * Fix GOT pointer:
	 *
	 * New GOT-PTR = (old GOT-PTR - CONFIG_SYS_MONITOR_BASE) + Destination Address
	 *
	 * Offset:
	 */
	sub	r15,r10,r4

	/* First our own GOT */
	add	r14,r14,r15
	/* the the one used by the C code */
	add	r30,r30,r15

	/*
	 * Now relocate code
	 */

	cmplw	cr1,r3,r4
	addi	r0,r5,3
	srwi.	r0,r0,2
	beq	cr1,4f		/* In place copy is not necessary	*/
	beq	7f		/* Protect against 0 count		*/
	mtctr	r0
	bge	cr1,2f

	la	r8,-4(r4)
	la	r7,-4(r3)
1:	lwzu	r0,4(r8)
	stwu	r0,4(r7)
	bdnz	1b
	b	4f

2:	slwi	r0,r0,2
	add	r8,r4,r0
	add	r7,r3,r0
3:	lwzu	r0,-4(r8)
	stwu	r0,-4(r7)
	bdnz	3b

/*
 * Now flush the cache: note that we must start from a cache aligned
 * address. Otherwise we might miss one cache line.
 */
4:	cmpwi	r6,0
	add	r5,r3,r5
	beq	7f		/* Always flush prefetch queue in any case */
	subi	r0,r6,1
	andc	r3,r3,r0
	mr	r4,r3
5:	dcbst	0,r4
	add	r4,r4,r6
	cmplw	r4,r5
	blt	5b
	sync			/* Wait for all dcbst to complete on bus */
	mr	r4,r3
6:	icbi	0,r4
	add	r4,r4,r6
	cmplw	r4,r5
	blt	6b
7:	sync			/* Wait for all icbi to complete on bus */
	isync

/*
 * We are done. Do not return, instead branch to second part of board
 * initialization, now running from RAM.
 */

	addi	r0,r10,in_ram - _start + _START_OFFSET
	mtlr	r0
	blr				/* NEVER RETURNS! */
	.globl	in_ram
in_ram:

        /*
         * Adjust got table entries to fix pointers to strings
         */
        li      r0,__got_entries@sectoff@l
        la      r4,GOT(_GOT_TABLE_)
        cmpwi   r0,0
        mtctr   r0
        addi    r4,r4,-4
        beq     4f
3:      lwzu    r3,4(r4)
        add     r3,r3,r15
        stw     r3,0(r4)
        bdnz    3b
4:
	bl	board_init_r
