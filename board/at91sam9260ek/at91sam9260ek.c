/*
 * (C) Copyright 2006
 * M. Amine SAYA ATMEL Rousset, France.
 * Added AT91SAM9260EK support.
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

int board_init (void)
{
  DECLARE_GLOBAL_DATA_PTR;
  
  /* Enable Ctrlc */
  console_init_f ();

  /* Enable clocks for SMC */
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_SYS;

  /* memory and cpu-speed are setup before relocation */
  /* so we do _nothing_ here */  

  /* arch number of AT91SAM9260EK-Board */
  gd->bd->bi_arch_number = MACH_TYPE_AT91SAM9260EK;
  /* adress of boot parameters */
  gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;
  
  return 0;
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
  eth_init(gd->bd);
#endif

  return 0;
}



int AT91F_Serial_Hardware_Init(void)
{

/* DBGU */
#ifdef CONFIG_DBGU
  *AT91C_PIOB_PDR = AT91C_PB15_DTXD | AT91C_PB14_DRXD;	/* PB 15 & 15 */
  *AT91C_PMC_PCER = 1 << AT91C_ID_SYS;	                /* enable clock */
#endif

/* USART 0 */  
#ifdef CONFIG_USART0
  *AT91C_PIOB_PDR = AT91C_PB4_TXD0 | AT91C_PB5_RXD0;	/* PB 4 & 5 */
  *AT91C_PMC_PCER |= 1 << AT91C_ID_US0;	/* enable clock */
#endif
  
#ifdef CONFIG_USART1
  *AT91C_PIOB_PDR = AT91C_PB6_TXD1 | AT91C_PB7_RXD1;	/* PB 6 & 7 */
  *AT91C_PMC_PCER |= 1 << AT91C_ID_US1;	/* enable clock */
#endif
  
#ifdef CONFIG_USART2
  *AT91C_PIOB_PDR = AT91C_PB8_TXD2 | AT91C_PB9_RXD2;	/* PB 8 & 9 */
  *AT91C_PMC_PCER |= 1 << AT91C_ID_US2;	/* enable clock */
#endif
}


#ifdef CONFIG_DRIVER_ETHER
#if (CONFIG_COMMANDS & CFG_CMD_NET)

extern AT91PS_EMAC p_mac;

int AT91F_EMAC_Hardware_Init(void)
{
	unsigned int periphAEnable, periphBEnable;
	unsigned int val, i;
	int ret;
	
	p_mac = AT91C_BASE_EMACB;

	/* Disable pull-up on:
		RXDV (PA17) => PHY normal mode (not Test mode)
		
		ERX0 (PA14) => PHY ADDR0
		ERX1 (PA15) => PHY ADDR1
		ERX2 (PA25) => PHY ADDR2
		ERX3 (PA26) => PHY ADDR3				
		ECRS (PA28) => PHY ADDR4  => PHYADDR = 0x0
		
	   PHY has internal pull-down
	*/
	
	AT91C_BASE_PIOA->PIO_PPUDR = AT91C_PA14_ERX0 | AT91C_PA15_ERX1 |
				     AT91C_PA17_ERXDV | AT91C_PA25_ERX2 |
				     AT91C_PA26_ERX3 | AT91C_PA28_ECRS;
	
	/* Need to reset PHY -> 500ms reset */
	AT91C_BASE_RSTC->RSTC_RMR = (AT91C_RSTC_KEY & ((unsigned int)0xA5<<24)) |
				    (AT91C_RSTC_ERSTL & (0x0D << 8)) | AT91C_RSTC_URSTEN;
	AT91C_BASE_RSTC->RSTC_RCR = (AT91C_RSTC_KEY & ((unsigned int)0xA5<<24)) |
				     AT91C_RSTC_EXTRST;

	/* Wait for end hardware reset */
	while (!(AT91C_BASE_RSTC->RSTC_RSR & AT91C_RSTC_NRSTL));

	/* Re-enable pull-up */
	AT91C_BASE_PIOA->PIO_PPUER = AT91C_PA14_ERX0 | AT91C_PA15_ERX1 |
				     AT91C_PA17_ERXDV | AT91C_PA25_ERX2 |
				     AT91C_PA26_ERX3 | AT91C_PA28_ECRS;

#ifdef CONFIG_AT91C_USE_RMII
	periphAEnable = ((unsigned int) AT91C_PA21_EMDIO   ) |
		((unsigned int) AT91C_PA20_EMDC    ) |
		((unsigned int) AT91C_PA19_ETXCK   ) |
		((unsigned int) AT91C_PA18_ERXER   ) |
		((unsigned int) AT91C_PA14_ERX0    ) |
		((unsigned int) AT91C_PA17_ERXDV   ) |
		((unsigned int) AT91C_PA15_ERX1    ) |
		((unsigned int) AT91C_PA16_ETXEN   ) |
		((unsigned int) AT91C_PA12_ETX0    ) |
		((unsigned int) AT91C_PA13_ETX1    );

	periphBEnable = 0;
#else
	periphAEnable = ((unsigned int) AT91C_PA21_EMDIO   ) |
		((unsigned int) AT91C_PA19_ETXCK   ) |
		((unsigned int) AT91C_PA20_EMDC    ) |
		((unsigned int) AT91C_PA18_ERXER   ) |
		((unsigned int) AT91C_PA14_ERX0    ) |
		((unsigned int) AT91C_PA17_ERXDV   ) |
		((unsigned int) AT91C_PA15_ERX1    ) |
		((unsigned int) AT91C_PA16_ETXEN   ) |
		((unsigned int) AT91C_PA12_ETX0    ) |
		((unsigned int) AT91C_PA13_ETX1    );
		
	periphBEnable = ((unsigned int) AT91C_PA27_ERXCK   ) |
		((unsigned int) AT91C_PA29_ECOL    ) |
		((unsigned int) AT91C_PA25_ERX2    ) |
		((unsigned int) AT91C_PA26_ERX3    ) |
		((unsigned int) AT91C_PA22_ETXER   ) |
		((unsigned int) AT91C_PA10_ETX2    ) |		
		((unsigned int) AT91C_PA11_ETX3    ) |
		((unsigned int) AT91C_PA28_ECRS	   );
#endif
	AT91C_BASE_PIOA->PIO_ASR = periphAEnable;
	AT91C_BASE_PIOA->PIO_BSR = periphBEnable;
	AT91C_BASE_PIOA->PIO_PDR = (periphAEnable | periphBEnable);
}

#endif	/* CONFIG_COMMANDS & CFG_CMD_NET */
#endif	/* CONFIG_DRIVER_ETHER */
