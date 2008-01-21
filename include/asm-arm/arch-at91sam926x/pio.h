/*
 * (C) Copyright 2005
 * M Amine SAYA ATEML Rousset, France.
 * include/asm/arch/pio.h contains pio config and management functions
 *
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


#ifndef __ASM_ARCH_PIO_H
#define __ASM_ARCH_PIO_H

/*---------------------------------------------------------------------------- */
/* \fn    AT91F_PIO_CfgPeriph */
/* \brief Enable pins to be drived by peripheral */
/*---------------------------------------------------------------------------- */
void __inline__ AT91F_PIO_CfgPeriph(
				    AT91PS_PIO pPio,             
				    unsigned int periphAEnable,  
				    unsigned int periphBEnable)
     
{
	pPio->PIO_ASR = periphAEnable;
	pPio->PIO_BSR = periphBEnable;
	pPio->PIO_PDR = (periphAEnable | periphBEnable); /* Set in Periph mode */
}

/*----------------------------------------------------------------------------*/
/* \fn    AT91F_PIO_CfgOutput */
/* \brief Enable PIO in output mode */
/*----------------------------------------------------------------------------*/
void __inline__ AT91F_PIO_CfgOutput(
	AT91PS_PIO pPio,             
	unsigned int pioEnable)      
{
	pPio->PIO_PER = pioEnable; /* Set in PIO mode */
	pPio->PIO_OER = pioEnable; /* Configure in Output */
}

/*----------------------------------------------------------------------------*/
/* \fn    AT91F_PIO_CfgInput */
/* \brief Enable PIO in input mode */
/*----------------------------------------------------------------------------*/
void __inline__ AT91F_PIO_CfgInput(
	AT91PS_PIO pPio,             
	unsigned int inputEnable)
{
	/* Disable output */
	pPio->PIO_ODR  = inputEnable;
	pPio->PIO_PER  = inputEnable;
}

/*---------------------------------------------------------------------------- */
/* \fn    AT91F_PIO_GetInput */
/* \brief Return PIO input value */
/*----------------------------------------------------------------------------*/
unsigned int __inline__ AT91F_PIO_GetInput(AT91PS_PIO pPio)
{
	return pPio->PIO_PDSR;
}

/*----------------------------------------------------------------------------*/
/* \fn    AT91F_PIO_SetOutput */
/* \brief Set to 1 output PIO */
/*----------------------------------------------------------------------------*/
void __inline__ AT91F_PIO_SetOutput(
	AT91PS_PIO pPio,
	unsigned int flag)
{
	pPio->PIO_SODR = flag;
}

/*----------------------------------------------------------------------------*/
/* \fn    AT91F_PIO_ClearOutput */
/* \brief Set to 0 output PIO */
/*----------------------------------------------------------------------------*/
void __inline__ AT91F_PIO_ClearOutput(
	AT91PS_PIO pPio,   
	unsigned int flag)
{
	pPio->PIO_CODR = flag;
}

/*----------------------------------------------------------------------------*/
/* \fn    AT91F_PIO_ForceOutput */
/* \brief Force output when Direct drive option is enabled */
/*----------------------------------------------------------------------------*/
void __inline__ AT91F_PIO_ForceOutput(
	AT91PS_PIO pPio,   
	unsigned int flag)
{
	pPio->PIO_ODSR = flag;
}

/*----------------------------------------------------------------------------*/
/* \fn    AT91F_PIO_Enable */
/* \brief Enable PIO */
/*----------------------------------------------------------------------------*/
void __inline__ AT91F_PIO_Enable(
        AT91PS_PIO pPio,
        unsigned int flag) 
{
        pPio->PIO_PER = flag;
}

/*----------------------------------------------------------------------------*/
/* \fn    AT91F_PIO_Disable */
/* \brief Disable PIO */
/*----------------------------------------------------------------------------*/
void __inline__ AT91F_PIO_Disable(
        AT91PS_PIO pPio,
        unsigned int flag) 
{
        pPio->PIO_PDR = flag;
}

/*----------------------------------------------------------------------------*/
/* \fn    AT91F_PIO_GetStatus */
/* \brief Return PIO Status */
/*----------------------------------------------------------------------------*/
unsigned int __inline__ AT91F_PIO_GetStatus(AT91PS_PIO pPio)
{
        return pPio->PIO_PSR;
}

/*----------------------------------------------------------------------------*/
/* \fn    AT91F_PIO_OutputEnable */
/* \brief Output Enable PIO */
/*----------------------------------------------------------------------------*/
void __inline__ AT91F_PIO_OutputEnable(
        AT91PS_PIO pPio,
        unsigned int flag)
{
        pPio->PIO_OER = flag;
}

/*----------------------------------------------------------------------------*/
/* \fn    AT91F_PIO_OutputDisable */
/* \brief Output Enable PIO */
/*----------------------------------------------------------------------------*/
void __inline__ AT91F_PIO_OutputDisable(
        AT91PS_PIO pPio,
        unsigned int flag)
{
        pPio->PIO_ODR = flag;
}

/*----------------------------------------------------------------------------*/
/* \fn    AT91F_PIO_GetOutputStatus */
/* \brief Return PIO Output Status */
/*----------------------------------------------------------------------------*/
unsigned int __inline__ AT91F_PIO_GetOutputStatus(AT91PS_PIO pPio)
{
        return pPio->PIO_OSR;
}

/*----------------------------------------------------------------------------*/
/* \fn    AT91F_PIO_CfgPullup */
/* \brief Enable pullup on PIO */
/*----------------------------------------------------------------------------*/
void __inline__ AT91F_PIO_CfgPullup(
	AT91PS_PIO pPio,
	unsigned int pullupEnable)
{
	/* Connect or not Pullup */
	pPio->PIO_PPUDR = ~pullupEnable;
	pPio->PIO_PPUER = pullupEnable;
}

#ifdef CONFIG_AT91SAM9261EK
void __inline__ AT91F_LCDC_TFT_CfgPIO (void)
{
  *AT91C_PIOB_PDR = (AT91C_PB1_LCDHSYNC | AT91C_PB2_LCDDOTCK | AT91C_PB3_LCDDEN | AT91C_PB4_LCDCC |
			AT91C_PB7_LCDD2 | AT91C_PB8_LCDD3 | AT91C_PB9_LCDD4 |
			AT91C_PB10_LCDD5 | AT91C_PB11_LCDD6 | AT91C_PB12_LCDD7 |
			AT91C_PB15_LCDD10 | AT91C_PB16_LCDD11 | AT91C_PB17_LCDD12 |
			AT91C_PB18_LCDD13 | AT91C_PB19_LCDD14 | AT91C_PB20_LCDD15 |
			AT91C_PB23_LCDD18 | AT91C_PB24_LCDD19 | AT91C_PB25_LCDD20 |
			AT91C_PB26_LCDD21 | AT91C_PB27_LCDD22 | AT91C_PB28_LCDD23);

  *AT91C_PIOB_BSR = (AT91C_PB23_LCDD18 | AT91C_PB24_LCDD19 | AT91C_PB25_LCDD20 |
			AT91C_PB26_LCDD21 | AT91C_PB27_LCDD22 | AT91C_PB28_LCDD23);

  /* Configure PA12 in pio to enable LCD */
  *AT91C_PIOA_PER = AT91C_PIO_PA12;
  
  *AT91C_PIOA_OER = AT91C_PIO_PA12;
  *AT91C_PIOA_CODR = AT91C_PIO_PA12;
}
#endif

#ifdef CONFIG_AT91SAM9263EK
void __inline__ AT91F_PIOC_CfgPMC(void){
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOCDE;
}

void __inline__ AT91F_EMAC_CfgPIO(void)
{
  *AT91C_PIOE_PDR = (AT91C_PE21_E_TXCK | AT91C_PE22_E_CRS | AT91C_PE23_E_TX0 |
		     AT91C_PE24_E_TX1 | AT91C_PE25_E_RX0 | AT91C_PE26_E_RX1 |
		     AT91C_PE27_E_RXER | AT91C_PE28_E_TXEN | AT91C_PE29_E_MDC |
		     AT91C_PE30_E_MDIO | AT91C_PE31_E_F100);

  *AT91C_PIOC_PDR = AT91C_PC25_E_RXDV ;
  *AT91C_PIOC_BSR = AT91C_PC25_E_RXDV ;
}

void __inline__ AT91F_LCDC_TFT_CfgPIO (void)
{
  *AT91C_PIOC_PDR = (AT91C_PC1_LCDHSYNC | AT91C_PC2_LCDDOTCK | AT91C_PC3_LCDEN   |
		     AT91C_PC6_LCDD2    | AT91C_PC7_LCDD3    | AT91C_PC8_LCDD4   |
		     AT91C_PC9_LCDD5    | AT91C_PC10_LCDD6   | AT91C_PC11_LCDD7  | 
		     AT91C_PC12_LCDD13B | AT91C_PC14_LCDD10  | AT91C_PC15_LCDD11 | 
		     AT91C_PC16_LCDD12  | AT91C_PC18_LCDD14  | AT91C_PC19_LCDD15 | 
		     AT91C_PC17_LCDD21B | AT91C_PC22_LCDD18  | AT91C_PC23_LCDD19 | 
		     AT91C_PC24_LCDD20  | AT91C_PC26_LCDD22  | AT91C_PC27_LCDD23);
  
  *AT91C_PIOC_BSR =  AT91C_PC12_LCDD13B | AT91C_PC17_LCDD21B;

  *AT91C_PIOB_PDR = AT91C_PB9_LCDCC;
  *AT91C_PIOB_BSR = AT91C_PB9_LCDCC;

  /* Configure PD12 in pio to enable LCD
   PD12 is set to 1 in pio mode by default
   after reset. This is just a redo in case
   something else changes PD12. */
  *AT91C_PIOD_PER = AT91C_PIO_PD12;
  *AT91C_PIOD_OER = AT91C_PIO_PD12;
}
#endif

#ifdef CONFIG_AT91SAM9RLEK
void __inline__ AT91F_LCDC_TFT_CfgPIO (void)
{
  *AT91C_PIOC_PDR = (AT91C_PC3_LCDCC   | AT91C_PC5_LCDHSYNC | AT91C_PC6_LCDDOTCK | AT91C_PC7_LCDDEN  |
		     AT91C_PC9_LCDD3   | AT91C_PC10_LCDD4   | AT91C_PC11_LCDD5  |
		     AT91C_PC12_LCDD6  | AT91C_PC13_LCDD7   | 
		     AT91C_PC15_LCDD11 | AT91C_PC16_LCDD12  | AT91C_PC17_LCDD13 |
		     AT91C_PC18_LCDD14 | AT91C_PC19_LCDD15  |
		     AT91C_PC20_LCDD18 | AT91C_PC21_LCDD19  | AT91C_PC22_LCDD20 |
		     AT91C_PC23_LCDD21 | AT91C_PC24_LCDD22  | AT91C_PC25_LCDD23
		);
  
  *AT91C_PIOC_BSR = (AT91C_PC9_LCDD3    | AT91C_PC10_LCDD4   | AT91C_PC11_LCDD5  |
		     AT91C_PC12_LCDD6   | AT91C_PC13_LCDD7   | 
		     AT91C_PC15_LCDD11  | AT91C_PC16_LCDD12  | AT91C_PC17_LCDD13 |
		     AT91C_PC18_LCDD14  | AT91C_PC19_LCDD15  |
		     AT91C_PC20_LCDD18  | AT91C_PC21_LCDD19  | AT91C_PC22_LCDD20 |
		     AT91C_PC23_LCDD21  | AT91C_PC24_LCDD22  | AT91C_PC25_LCDD23
		);

#if 0
  /* Configure PD12 in pio to enable LCD
   PD12 is set to 1 in pio mode by default
   after reset. This is just a redo in case
   something else changes PD12. */
  *AT91C_PIOD_PER = AT91C_PIO_PD12;
  *AT91C_PIOD_OER = AT91C_PIO_PD12;
#endif
}
#endif

#endif
