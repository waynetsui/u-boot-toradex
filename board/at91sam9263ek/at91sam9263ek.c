/*
 * (C) Copyright 2006
 * M. Amine SAYA ATEML Rousset, France.
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
  
#ifdef CONFIG_LCD
  gd->fb_base = (unsigned long) AT91C_IRAM;
#endif

  /* arch number of AT91SAM9263EK-Board */
  gd->bd->bi_arch_number = MACH_TYPE_AT91SAM9263EK;
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
#ifdef CONFIG_CMD_NET
  eth_init(gd->bd);
#endif

  return 0;
}


void hw_flash_init(void){
    
  /* Configure SMC CS0 for parallel flash */
  AT91C_BASE_SMC0->SMC_SETUP0 = (AT91C_FLASH_NWE_SETUP | AT91C_FLASH_NCS_WR_SETUP |
				AT91C_FLASH_NRD_SETUP | AT91C_FLASH_NCS_RD_SETUP);
  
  AT91C_BASE_SMC0->SMC_PULSE0 = (AT91C_FLASH_NWE_PULSE | AT91C_FLASH_NCS_WR_PULSE | 
				AT91C_FLASH_NRD_PULSE | AT91C_FLASH_NCS_RD_PULSE);
  
  AT91C_BASE_SMC0->SMC_CYCLE0 = AT91C_FLASH_NWE_CYCLE | AT91C_FLASH_NRD_CYCLE;
  
  AT91C_BASE_SMC0->SMC_CTRL0 = (AT91C_SMC_READMODE | AT91C_SMC_WRITEMODE | 
			       AT91C_SMC_NWAITM_NWAIT_DISABLE | AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS | 
			       AT91C_FLASH_TDF);
}


int AT91F_Serial_Hardware_Init(void)
{

/* DBGU */
#ifdef CONFIG_DBGU
  *AT91C_PIOC_PDR = AT91C_PC31_DTXD | AT91C_PC30_DRXD;	/* PA 31 & 30 */
  *AT91C_PMC_PCER = 1 << AT91C_ID_SYS;	                /* enable clock */
#endif

/* USART 0 */  
#ifdef CONFIG_USART0
  *AT91C_PIOA_PDR = AT91C_PA26_TXD0 | AT91C_PA27_RXD0;
  *AT91C_PMC_PCER |= 1 << AT91C_ID_US0;	/* enable clock */
#endif
  
#ifdef CONFIG_USART1
  *AT91C_PIOD_PDR = AT91C_PD0_TXD1 | AT91C_PD1_RXD1;
  *AT91C_PMC_PCER |= 1 << AT91C_ID_US1;	/* enable clock */
#endif
  
#ifdef CONFIG_USART2
  *AT91C_PIOD_PDR = AT91C_PD2_TXD2 | AT91C_PD3_RXD2;
  *AT91C_PMC_PCER |= 1 << AT91C_ID_US2;	/* enable clock */
#endif

}


#ifdef CONFIG_DRIVER_ETHER
#ifdef CONFIG_CMD_NET

extern AT91PS_EMAC p_mac;

void AT91F_EMAC_Hardware_Init(void)
{
	unsigned int periphAEnable, periphBEnable;
	
	p_mac = AT91C_BASE_EMAC;

	/* Disable pull-up on:
		RXDV (PA17) => PHY normal mode (not Test mode)
		
		ERX0 (PE25) => PHY ADDR0
		ERX1 (PE26) => PHY ADDR1
		ERX2 (PA25) => PHY ADDR2
		ERX3 (PA26) => PHY ADDR3				
		ECRS (PA28) => PHY ADDR4  => PHYADDR = 0x0
		
	   PHY has internal pull-down
	*/
	AT91C_BASE_PIOC->PIO_PPUDR = AT91C_PC25_E_RXDV;
	AT91C_BASE_PIOE->PIO_PPUDR = AT91C_PE25_E_RX0 | AT91C_PE26_E_RX1;
	
	/* Need to reset PHY -> 500ms reset */
	AT91C_BASE_RSTC->RSTC_RMR = (AT91C_RSTC_KEY & ((unsigned int)0xA5<<24)) |
				    (AT91C_RSTC_ERSTL & (0x0D << 8)) | AT91C_RSTC_URSTEN;
	AT91C_BASE_RSTC->RSTC_RCR = (AT91C_RSTC_KEY & ((unsigned int)0xA5<<24)) |
				     AT91C_RSTC_EXTRST;

	/* Wait for end hardware reset */
	while (!(AT91C_BASE_RSTC->RSTC_RSR & AT91C_RSTC_NRSTL));

	/* Re-enable pull-up */
	AT91C_BASE_PIOC->PIO_PPUER = AT91C_PC25_E_RXDV;
	AT91C_BASE_PIOE->PIO_PPUER = AT91C_PE25_E_RX0 | AT91C_PE26_E_RX1;
	
#ifdef CONFIG_AT91C_USE_RMII

	periphAEnable =	0;
	periphBEnable = ((unsigned int) AT91C_PC25_E_RXDV);

	AT91C_BASE_PIOC->PIO_ASR = periphAEnable;
	AT91C_BASE_PIOC->PIO_BSR = periphBEnable;
	AT91C_BASE_PIOC->PIO_PDR = (periphAEnable | periphBEnable);

	periphAEnable =	((unsigned int) AT91C_PE28_E_TXEN ) |
			((unsigned int) AT91C_PE26_E_RX1 ) |
			((unsigned int) AT91C_PE24_E_TX1 ) |
			((unsigned int) AT91C_PE29_E_MDC ) |
			((unsigned int) AT91C_PE27_E_RXER ) |
			((unsigned int) AT91C_PE23_E_TX0 ) |
			((unsigned int) AT91C_PE25_E_RX0 ) |
			((unsigned int) AT91C_PE21_E_TXCK ) |
			((unsigned int) AT91C_PE30_E_MDIO );
	periphBEnable =	0;
	AT91C_BASE_PIOE->PIO_ASR = periphAEnable;
	AT91C_BASE_PIOE->PIO_BSR = periphBEnable;
	AT91C_BASE_PIOE->PIO_PDR = (periphAEnable | periphBEnable);

#else
	periphAEnable =	0;
	periphBEnable = ((unsigned int) AT91C_PC25_E_RXDV) |
			((unsigned int) AT91C_PC20_E_TX2 ) |
			((unsigned int) AT91C_PC24_E_TXER ) |
			((unsigned int) AT91C_PC27_E_RXCK ) |
			((unsigned int) AT91C_PC26_E_COL ) |
			((unsigned int) AT91C_PC21_E_TX3 ) |
			((unsigned int) AT91C_PC22_E_RX2 ) |
			((unsigned int) AT91C_PC23_E_RX3 );

	AT91C_BASE_PIOC->PIO_ASR = periphAEnable;
	AT91C_BASE_PIOC->PIO_BSR = periphBEnable;
	AT91C_BASE_PIOC->PIO_PDR = (periphAEnable | periphBEnable);

	periphAEnable =	((unsigned int) AT91C_PE28_E_TXEN ) |
			((unsigned int) AT91C_PE26_E_RX1 ) |
			((unsigned int) AT91C_PE24_E_TX1 ) |
			((unsigned int) AT91C_PE29_E_MDC ) |
			((unsigned int) AT91C_PE27_E_RXER ) |
			((unsigned int) AT91C_PE23_E_TX0 ) |
			((unsigned int) AT91C_PE25_E_RX0 ) |
			((unsigned int) AT91C_PE22_E_CRS ) |
			((unsigned int) AT91C_PE21_E_TXCK ) |
			((unsigned int) AT91C_PE30_E_MDIO );
	periphBEnable =	0;
	AT91C_BASE_PIOE->PIO_ASR = periphAEnable;
	AT91C_BASE_PIOE->PIO_BSR = periphBEnable;
	AT91C_BASE_PIOE->PIO_PDR = (periphAEnable | periphBEnable);
#endif
}

#endif	/* CONFIG_CMD_NET */
#endif	/* CONFIG_DRIVER_ETHER */
