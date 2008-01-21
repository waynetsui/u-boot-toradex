/*
 * (C) Copyright 2007
 * ATMEL Rousset, France.
 * Added AT91SAM9RLEK support.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
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
#include <asm/arch/hardware.h>

/* ------------------------------------------------------------------------- */
/*
 * Miscelaneous platform dependent initialisations
 */

int board_init (void)
{
  DECLARE_GLOBAL_DATA_PTR;
  
  /* Enable Ctrlc */
  console_init_f ();

  /* Enable clocks for SMC and PIOC */
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_SYS;

  /* memory and cpu-speed are setup before relocation */
  /* so we do _nothing_ here */
  
#ifdef CONFIG_LCD
//  gd->fb_base = (unsigned long) AT91C_IRAM;
  gd->fb_base = (unsigned long) 0x23000000;
#endif

  /* arch number of AT91SAM9RLEK-Board */
  gd->bd->bi_arch_number = MACH_TYPE_AT91SAM9RLEK;
  /* adress of boot parameters */
  gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;
  
  return 0;
}

int AT91F_Serial_Hardware_Init(void)
{
#ifdef CONFIG_DBGU
  *AT91C_PIOA_PDR = AT91C_PA21_DRXD | AT91C_PA22_DTXD;	/* PA 21 & 22 */
  *AT91C_PMC_PCER = 1 << AT91C_ID_SYS;	/* enable clock */
#endif
  
#ifdef CONFIG_USART0
  *AT91C_PIOA_PDR = AT91C_PA6_TXD0 | AT91C_PA7_RXD0;
  *AT91C_PMC_PCER |= 1 << AT91C_ID_US0;	/* enable clock */
#endif
}

int dram_init (void)
{
  DECLARE_GLOBAL_DATA_PTR;
  
  gd->bd->bi_dram[0].start = PHYS_SDRAM;
  gd->bd->bi_dram[0].size = PHYS_SDRAM_SIZE;
  return 0;
}


int board_late_init(void)
{
  return 0;
}
