/*
 * (C) Copyright 2005
 * M. Amine SAYA ATMEL Rousset, France.
 * Added AT91SAM9261EK support.
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

#include <net.h>

/* ------------------------------------------------------------------------- */
/*
 * Miscelaneous platform dependent initialisations
 */
#ifdef  CONFIG_DRIVER_DM9000

void at91sam9261ek_dm9000_init(void) {
  
  /* Configure SMC CS2 for DM9000 */
  AT91C_BASE_SMC->SMC_SETUP2 = (AT91C_DM9000_NWE_SETUP | AT91C_DM9000_NCS_WR_SETUP |
				AT91C_DM9000_NRD_SETUP | AT91C_DM9000_NCS_RD_SETUP);
  
  AT91C_BASE_SMC->SMC_PULSE2 = (AT91C_DM9000_NWE_PULSE | AT91C_DM9000_NCS_WR_PULSE | AT91C_DM9000_NRD_PULSE | AT91C_DM9000_NCS_RD_PULSE);
  
  AT91C_BASE_SMC->SMC_CYCLE2 = (AT91C_DM9000_NWE_CYCLE | AT91C_DM9000_NRD_CYCLE);
  
#ifdef CONFIG_DM9000_USE_8BIT
  AT91C_BASE_SMC->SMC_CTRL2 = (AT91C_SMC_READMODE | AT91C_SMC_WRITEMODE | AT91C_SMC_NWAITM_NWAIT_ENABLE_READY |
			       AT91C_SMC_DBW_WIDTH_EIGTH_BITS | AT91C_DM9000_TDF);
#endif
  
#ifdef CONFIG_DM9000_USE_16BIT
  AT91C_BASE_SMC->SMC_CTRL2 = (AT91C_SMC_READMODE | AT91C_SMC_WRITEMODE | AT91C_SMC_NWAITM_NWAIT_DISABLE |
			       AT91C_SMC_BAT_BYTE_WRITE | AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS | AT91C_DM9000_TDF);
#endif
  
  /* Configure NWAIT signal */
  /* AT91F_PIO_CfgPeriph(AT91C_BASE_PIOC,AT91C_PC2_NWAIT,0); */
  
  /* Configure Reset signal in output */
  AT91F_PIO_CfgOutput(AT91C_BASE_PIOC,AT91C_PIO_PC10);
  AT91F_PIO_ClearOutput(AT91C_BASE_PIOC,AT91C_PIO_PC10);
}
#endif

int board_init (void)
{
  DECLARE_GLOBAL_DATA_PTR;
  
  /* Enable Ctrlc */
  console_init_f ();

  /* Enable clocks for SMC and PIOC */
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_SYS;
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOC;

  /* memory and cpu-speed are setup before relocation */
  /* so we do _nothing_ here */
  
#ifdef CONFIG_LCD
  gd->fb_base = (unsigned long) AT91C_IRAM;
#endif

  /* arch number of AT91SAM9261EK-Board */
  gd->bd->bi_arch_number = MACH_TYPE_AT91SAM9261EK;
  /* adress of boot parameters */
  gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;
  
  return 0;
}

int AT91F_Serial_Hardware_Init(void)
{
#ifdef CONFIG_DBGU
  *AT91C_PIOA_PDR = AT91C_PA10_DTXD | AT91C_PA9_DRXD;	/* PA 10 & 9 */
  *AT91C_PMC_PCER = 1 << AT91C_ID_SYS;	/* enable clock */
#endif
  
#ifdef CONFIG_USART0
  *AT91C_PIOA_PDR = AT91C_PC8_TXD0 | AT91C_PC9_RXD0;
  *AT91C_PMC_PCER |= 1 << AT91C_ID_US0;	/* enable clock */
#endif
  
#ifdef CONFIG_USART1
  *AT91C_PIOB_PDR = AT91C_PC12_TXD1 | AT91C_PC13_RXD1;
  *AT91C_PMC_PCER |= 1 << AT91C_ID_US1;	/* enable clock */
#endif
  
#ifdef CONFIG_USART2
  *AT91C_PIOB_PDR = AT91C_PC14_TXD2 | AT91C_PC15_RXD2;
  *AT91C_PMC_PCER |= 1 << AT91C_ID_US2;	/* enable clock */
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
  DECLARE_GLOBAL_DATA_PTR;
  
  /* Fix Ethernet Initialization Bug when starting Linux from U-Boot */
#if (CONFIG_COMMANDS & CFG_CMD_NET)
#ifdef  CONFIG_DRIVER_DM9000
  /* configure SMC's NCS2 to use it with DM9000 chip */
  at91sam9261ek_dm9000_init();
#endif
  eth_init(gd->bd);
#endif

  return 0;
}
