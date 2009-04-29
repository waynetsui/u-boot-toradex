/*
 * (C) Copyright 2004-2006
 * Texas Instruments, <www.ti.com>
 * Rohit Choraria <rohitkc@ti.com>
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

#include <asm/io.h>

#include <asm/arch/cpu.h>
#include <asm/arch/mem.h>

#if (CONFIG_COMMANDS & CFG_CMD_NAND) && !defined(CFG_NAND_LEGACY)

#include <nand.h>

unsigned char cs;
volatile unsigned long gpmc_cs_base_add;

/*
 * omap_nand_hwcontrol - Set the address pointers corretly for the 
 *			following address/data/command operation
 * @mtd:        MTD device structure
 * @ctrl:	Says whether Address or Command or Data is following.
 */

static void omap_nand_hwcontrol(struct mtd_info *mtd, int ctrl)
{
	register struct nand_chip *this = mtd->priv;


/*
 * Point the IO_ADDR to DATA and ADDRESS registers instead of chip address
 */
	switch (ctrl) {
	 case NAND_CTL_SETCLE:
		this->IO_ADDR_W = (void *) gpmc_cs_base_add + GPMC_NAND_CMD;
		this->IO_ADDR_R = (void *) gpmc_cs_base_add + GPMC_NAND_DAT;
		break;
	 case NAND_CTL_SETALE:
		this->IO_ADDR_W = (void *) gpmc_cs_base_add + GPMC_NAND_ADR;
		this->IO_ADDR_R = (void *) gpmc_cs_base_add + GPMC_NAND_DAT;
		break;
	 case NAND_CTL_CLRCLE:
		this->IO_ADDR_W = (void *) gpmc_cs_base_add + GPMC_NAND_DAT;
		this->IO_ADDR_R = (void *) gpmc_cs_base_add + GPMC_NAND_DAT;
		break;
	 case NAND_CTL_CLRALE:
		this->IO_ADDR_W = (void *) gpmc_cs_base_add + GPMC_NAND_DAT;
		this->IO_ADDR_R = (void *) gpmc_cs_base_add + GPMC_NAND_DAT;
		break;
	}
}

/*
 * omap_nand_wait - called primarily after a program/erase operation
 *			so that we access NAND again only after the device
 *			is ready again. 
 * @mtd:        MTD device structure
 * @chip:	nand_chip structure
 * @state:	State from which wait function is being called i.e write/erase.
 */
static int omap_nand_wait(struct mtd_info *mtd, struct nand_chip *chip, int state)
{
	register struct nand_chip *this = mtd->priv;
	int status = 0;

	this->IO_ADDR_W = (void *) gpmc_cs_base_add + GPMC_NAND_CMD;
	this->IO_ADDR_R = (void *) gpmc_cs_base_add + GPMC_NAND_DAT;
	/* Send the status command and loop until the device is free */
	while(!(status & 0x40)){
		__raw_writeb(NAND_CMD_STATUS & 0xFF, this->IO_ADDR_W);
		status = __raw_readb(this->IO_ADDR_R);
	}
	return status;	
}

/*
 * omap_nand_dev_ready - Wait for the NAND device to exit busy state
 *			by polling on RDY/BSY signal
 * @mtd:        MTD device structure
 */ 
static int omap_nand_dev_ready(struct mtd_info *mtd)
{
	unsigned int wait_status = 0;
	
	/* busy loop until NAND device is RDY again */
	while(!(wait_status & (1 << (cs + 8))))
		wait_status = __raw_readl(GPMC_IRQSTATUS);
	/* clear the status register for further usage */
	__raw_writel(1 << (cs + 8), GPMC_IRQSTATUS);
	return 1;
}



/*
 * omap_nand_write_buf -  write buffer to NAND controller
 * @mtd:        MTD device structure
 * @buf:        data buffer
 * @len:        number of bytes to write
 *
 */

static void omap_nand_write_buf(struct mtd_info *mtd, const uint8_t * buf,
				int len)
{
	int i;
	int j=0;
	struct nand_chip *chip = mtd->priv;

	for (i = 0; i < len; i++) {
		writeb(buf[i], chip->IO_ADDR_W);
		for(j=0;j<10;j++);
        }

}

/*
 * omap_nand_read_buf - read data from NAND controller into buffer
 * @mtd:        MTD device structure
 * @buf:        buffer to store date
 * @len:        number of bytes to read
 *
 */

static void omap_nand_read_buf(struct mtd_info *mtd, uint8_t * buf, int len)
{
	int i;
	int j=0;
	struct nand_chip *chip = mtd->priv;

	for (i = 0; i < len; i++) {
		buf[i] = readb(chip->IO_ADDR_R);
		for(j=0;j<10;j++);
	}

}

/*
 * Board-specific NAND initialization. The following members of the
 * argument are board-specific (per include/linux/mtd/nand_new.h):
 * - IO_ADDR_R?: address to read the 8 I/O lines of the flash device
 * - IO_ADDR_W?: address to write the 8 I/O lines of the flash device
 * - hwcontrol: hardwarespecific function for accesing control-lines
 * - dev_ready: hardwarespecific function for  accesing device ready/busy line
 * - enable_hwecc?: function to enable (reset)  hardware ecc generator. Must
 *   only be provided if a hardware ECC is available
 * - eccmode: mode of ecc, see defines
 * - chip_delay: chip dependent delay for transfering data from array to
 *   read regs (tR)
 * - options: various chip options. They can partly be set to inform
 *   nand_scan about special functionality. See the defines for further
 *   explanation
 * Members with a "?" were not set in the merged testing-NAND branch,
 * so they are not set here either.
 */
void board_nand_init(struct nand_chip *nand)
{
	int gpmc_config=0;
        cs = 0;
	while (cs <= GPMC_MAX_CS) {
		/* Each GPMC set for a single CS is at offset 0x30 */
		/* already remapped for us */
		gpmc_cs_base_add = (GPMC_CONFIG_CS0 + (cs*0x30));
		/* xloader/Uboot would have written the NAND type for us
		 * -NOTE This is a temporary measure and cannot handle ONENAND.
		 * The proper way of doing this is to pass the setup of u-boot up to kernel
		 * using kernel params - something on the lines of machineID
		 */
		/* Check if NAND type is set */
		if ((__raw_readl(gpmc_cs_base_add + GPMC_CONFIG1) & 0xC00)==0x800) {
		/* Found it!! */
			break;
		}
		cs++;
	}
	if (cs > GPMC_MAX_CS) {
		printk ("NAND: Unable to find NAND settings in GPMC Configuration - quitting\n");
	}
       
	gpmc_config = __raw_readl(GPMC_CONFIG);
	/* Disable Write protect */
	gpmc_config |= 0x10;
	__raw_writel(gpmc_config, GPMC_CONFIG);


	nand->IO_ADDR_R		= gpmc_cs_base_add + GPMC_NAND_DAT;
	nand->IO_ADDR_W		= gpmc_cs_base_add + GPMC_NAND_CMD;

	nand->hwcontrol         = omap_nand_hwcontrol;
	nand->options           = NAND_SAMSUNG_LP_OPTIONS;
	nand->read_buf          = omap_nand_read_buf;
	nand->write_buf         = omap_nand_write_buf;
	nand->eccmode           = NAND_ECC_SOFT;
/* if RDY/BSY line is connected to OMAP then use the omap ready funcrtion
 * and the generic nand_wait function which reads the status register after
 * monitoring the RDY/BSY line. Otherwise use a standard chip delay which
 * is slightly more than tR (AC Timing) of the NAND device and read the
 * status register until you get a failure or success
 */

#if 0
	nand->dev_ready         = omap_nand_dev_ready;
#else
	nand->waitfunc		= omap_nand_wait;
	nand->chip_delay        = 50;
#endif
}


#endif /* (CONFIG_COMMANDS & CFG_CMD_NAND) */

