/*
 * (C) Copyright 2007 ATMEL Rousset, Nicolas Ferre
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

#if (CONFIG_COMMANDS & CFG_CMD_NAND)

#include <nand.h>

/*
 *	hardware specific access to control-lines
 */
#define	MASK_ALE	(1 << 21)	/* our ALE is AD21 */
#define	MASK_CLE	(1 << 22)	/* our CLE is AD22 */

/*
 * Disk On Chip (NAND) Millenium initialization.
 * The NAND lives in the CS3 space
 */
void at91sam9RLek_nand_init (struct nand_chip *nand)
{
  /* Setup nand flash, first enable the address range of CS3 in user interface */
  AT91C_BASE_CCFG->CCFG_EBICSA |= AT91C_EBI_CS3A_SM;
    
  /* Configure SMC CS3 */
  AT91C_BASE_SMC->SMC_SETUP3 = (AT91C_SM_NWE_SETUP | AT91C_SM_NCS_WR_SETUP |
				AT91C_SM_NRD_SETUP | AT91C_SM_NCS_RD_SETUP);
  
  AT91C_BASE_SMC->SMC_PULSE3 = (AT91C_SM_NWE_PULSE | AT91C_SM_NCS_WR_PULSE | AT91C_SM_NRD_PULSE | AT91C_SM_NCS_RD_PULSE);
  
  AT91C_BASE_SMC->SMC_CYCLE3 = (AT91C_SM_NWE_CYCLE | AT91C_SM_NRD_CYCLE);
  
  AT91C_BASE_SMC->SMC_CTRL3 = (AT91C_SMC_READMODE | AT91C_SMC_WRITEMODE | AT91C_SMC_NWAITM_NWAIT_DISABLE |
                                AT91C_SMC_DBW_WIDTH_EIGTH_BITS | AT91C_SM_TDF);
  
  /* Clock PIOD */
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOD;

  /* Configure Ready/Busy signal */
  AT91C_BASE_PIOD->PIO_ODR  = AT91C_PIO_PD17;
  AT91C_BASE_PIOD->PIO_PER  = AT91C_PIO_PD17;
  /* Configure pull-up */
  AT91C_BASE_PIOD->PIO_PPUER = AT91C_PIO_PD17;
  
  /* Enable PB4=SMOE line PB5=SMWE, A22=CLE, A21=ALE */
  AT91C_BASE_PIOB->PIO_ASR = (AT91C_PB4_NANDOE | AT91C_PB5_NANDWE);
  AT91C_BASE_PIOB->PIO_PDR = (AT91C_PB4_NANDOE | AT91C_PB5_NANDWE); /* Set in Periph mode */
  
  /* Enable NandFlash */
  AT91C_BASE_PIOB->PIO_PER = AT91C_PIO_PB6;
  AT91C_BASE_PIOB->PIO_OER = AT91C_PIO_PB6;
}

static void at91sam9RLek_nand_hwcontrol(struct mtd_info *mtd, int cmd)
{
	struct nand_chip *this = mtd->priv;
	ulong IO_ADDR_W = (ulong) this->IO_ADDR_W;

	IO_ADDR_W &= ~(MASK_ALE|MASK_CLE);
	switch (cmd) {
		case NAND_CTL_SETCLE: IO_ADDR_W |= MASK_CLE; break;
		case NAND_CTL_SETALE: IO_ADDR_W |= MASK_ALE; break;
		case NAND_CTL_CLRNCE: *AT91C_PIOC_SODR = AT91C_PIO_PC14; break;
		case NAND_CTL_SETNCE: *AT91C_PIOC_CODR = AT91C_PIO_PC14; break;
	}
	this->IO_ADDR_W = (void *) IO_ADDR_W;
}

/*
 *	chip R/B detection
 */
static int at91sam9RLek_nand_ready(struct mtd_info *mtd)
{
	return (*AT91C_PIOD_PDSR & AT91C_PIO_PD17);
}

void board_nand_init(struct nand_chip *nand)
{
	/* Init due to switch 8/16 bits mode */
	if (nand->write_byte)
		nand->write_byte = NULL;
	if (nand->read_byte)
		nand->read_byte = NULL;
	if (nand->write_buf)
		nand->write_buf = NULL;
	if (nand->read_buf)
		nand->read_buf = NULL;
	if (nand->verify_buf)
		nand->verify_buf = NULL;

	nand->eccmode = NAND_ECC_SOFT;
	nand->hwcontrol = at91sam9RLek_nand_hwcontrol;
	nand->dev_ready = at91sam9RLek_nand_ready;
	nand->chip_delay = 18;
	
	at91sam9RLek_nand_init(nand);
}
#endif
