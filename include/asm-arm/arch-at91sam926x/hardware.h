/*
 * linux/include/asm-arm/arch-at91sam926x/hardware.h
 *
 *  Copyright (C) 2005 M. Amine SAYA ATMEL Rousset, FRANCE.
 *  Copyright (C) 2003 SAN People
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#include <asm/sizes.h>

#ifdef CONFIG_AT91SAM9260EK
#ifndef __ASSEMBLY__
#include <asm/arch/at91sam9260.h>
#else
#include <asm/arch/at91sam9260_inc.h>
#endif
#endif

#ifdef CONFIG_AT91SAM9261EK
#ifndef __ASSEMBLY__
#include <asm/arch/at91sam9261.h>
#else
#include <asm/arch/at91sam9261_inc.h>
#endif
#endif

#ifdef CONFIG_AT91SAM9263EK
#ifndef __ASSEMBLY__
#include <asm/arch/at91sam9263.h>
#else
#include <asm/arch/at91sam9263_inc.h>
#endif
#endif

#ifdef CONFIG_AT91SAM9RLEK
#ifndef __ASSEMBLY__
#include <asm/arch/at91sam9rl.h>
#else
#include <asm/arch/at91sam9rl_inc.h>
#endif
#endif


/*
 * Where in virtual memory the IO devices (timers, system controllers
 * and so on)
 */
#define AT91_IO_BASE		0xF0000000	/* Virt/Phys Address of IO */

/* SDRAM */
#define AT91_SDRAM_BASE		0x20000000	/* NCS1 */

/* SmartMedia */
#define AT91_SMARTMEDIA_BASE	0x40000000	/* NCS3 */

/* Definition of interrupt priority levels */
#define AT91C_AIC_PRIOR_0 AT91C_AIC_PRIOR_LOWEST
#define AT91C_AIC_PRIOR_1 ((unsigned int) 0x1)
#define AT91C_AIC_PRIOR_2 ((unsigned int) 0x2)
#define AT91C_AIC_PRIOR_3 ((unsigned int) 0x3)
#define AT91C_AIC_PRIOR_4 ((unsigned int) 0x4)
#define AT91C_AIC_PRIOR_5 ((unsigned int) 0x5)
#define AT91C_AIC_PRIOR_6 ((unsigned int) 0x6)
#define AT91C_AIC_PRIOR_7 AT91C_AIC_PRIOR_HIGEST

#endif
