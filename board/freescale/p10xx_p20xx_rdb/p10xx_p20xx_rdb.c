/*
 * Copyright (C) 2009 Freescale Semiconductor, Inc. All rights reserved.
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

#include <common.h>
#include <command.h>
#include <pci.h>
#include <asm/processor.h>
#include <asm/mmu.h>
#include <asm/cache.h>
#include <asm/immap_85xx.h>
#include <asm/immap_fsl_pci.h>
#include <asm/fsl_ddr_sdram.h>
#include <asm/io.h>
#include <asm/fsl_law.h>
#include <miiphy.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <tsec.h>
#include <vsc7385.h>


#if defined(CONFIG_DDR_ECC) && !defined(CONFIG_ECC_INIT_VIA_DDRCONTROLLER)
extern void ddr_enable_ecc(unsigned int dram_size);
#endif

#define GPIO_DIR	0x060f0000
#define BOARD_PERI_RST	0x020f0000
#define USB_RST		0x08000000
DECLARE_GLOBAL_DATA_PTR;

phys_size_t  fixed_sdram(void);

int board_early_init_f (void)
{
#ifdef CONFIG_MMC
	volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);

	setbits_be32(&gur->pmuxcr,
			(MPC85xx_PMUXCR_SD_DATA |
			 MPC85xx_PMUXCR_SDHC_CD |
			 MPC85xx_PMUXCR_SDHC_WP));
#endif
	return 0;
}

int checkboard (void)
{
	u32 val;
	volatile ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);
	printf ("Board: %sRDB, System ID: 0x%02x, "
		"System Version: 0x%02x\n", gd->cpu->name, 0, 0);

/* Bringing the following peripherals out of reset via GPIOs
 * 0= reset and 1= out of reset
 * GPIO12 - Reset to Ethernet Switch
 * GPIO13 - Reset to SLIC/SLAC devices
 * GPIO14 - Reset to SGMII_PHY_N
 * GPIO15 - Reset to PCIe slots
 * GPIO6  - Reset to RGMII PHY
 * GPIO5 - Reset to USB3300 devices 1= reset and 0= out of reset
 */
	val = pgpio->gpdir;
	val |=  GPIO_DIR;
	setbits_be32(&pgpio->gpdir, GPIO_DIR);

	val = pgpio->gpdat;
	clrsetbits_be32(&pgpio->gpdat, USB_RST, BOARD_PERI_RST);

	return 0;
}

phys_size_t initdram(int board_type)
{
	phys_size_t dram_size = 0;

	puts("Initializing....");

#ifdef CONFIG_DDR_SPD
	dram_size = fsl_ddr_sdram();
#else
	dram_size = fixed_sdram();
	set_ddr_laws(0, dram_size, LAW_TRGT_IF_DDR_1);
#endif
	dram_size = setup_ddr_tlbs(dram_size / 0x100000);
	dram_size *= 0x100000;

#if defined(CONFIG_DDR_ECC) && !defined(CONFIG_ECC_INIT_VIA_DDRCONTROLLER)
	/*
	 * Initialize and enable DDR ECC.
	 */
	ddr_enable_ecc(dram_size);
#endif
	puts("    DDR: ");
	return dram_size;
}

#if !defined(CONFIG_SPD)
/*
 * Fixed sdram init -- doesn't use serial presence detect.
 */

phys_size_t fixed_sdram (void)
{

	volatile ccsr_ddr_t *ddr= (ccsr_ddr_t *)CONFIG_SYS_MPC85xx_DDR_ADDR;
	int d_init, dbw;

	ddr->cs0_bnds = CONFIG_SYS_DDR_CS0_BNDS;
	ddr->cs0_config = CONFIG_SYS_DDR_CS0_CONFIG;
	ddr->cs0_config_2 = CONFIG_SYS_DDR_CS0_CONFIG_2;
	ddr->timing_cfg_3 = CONFIG_SYS_DDR_TIMING_3;
	ddr->timing_cfg_0 = CONFIG_SYS_DDR_TIMING_0;
	ddr->timing_cfg_1 = CONFIG_SYS_DDR_TIMING_1;
	ddr->timing_cfg_2 = CONFIG_SYS_DDR_TIMING_2;
	ddr->timing_cfg_4 = CONFIG_SYS_DDR_TIMING_4;
	ddr->timing_cfg_5 = CONFIG_SYS_DDR_TIMING_5;
	ddr->sdram_mode = CONFIG_SYS_DDR_MODE_1;
	ddr->sdram_mode_2 = CONFIG_SYS_DDR_MODE_2;
	ddr->sdram_interval = CONFIG_SYS_DDR_INTERVAL;
	ddr->sdram_data_init = CONFIG_SYS_DDR_DATA_INIT;
	ddr->sdram_clk_cntl = CONFIG_SYS_DDR_CLK_CTRL;
	ddr->sdram_cfg_2 = CONFIG_SYS_DDR_CONTROL2;

	dbw = gd->cpu->ddr_data_width;
	if(dbw == 32) {
		/* need to check if this is required for P1020/P2020 also */
		printf("fixing DDR2 errata for 32bit\n");
		u32 temp = in_be32(&ddr->debug_1);
		out_be32(&ddr->debug_1, temp | 1);
	}

#if defined (CONFIG_DDR_ECC)
	ddr->err_int_en = CONFIG_SYS_DDR_ERR_INT_EN;
	ddr->err_disable = CONFIG_SYS_DDR_ERR_DIS;
	ddr->err_sbe = CONFIG_SYS_DDR_SBE;
#endif
	asm("sync;isync");

	udelay(500);

	ddr->sdram_cfg = CONFIG_SYS_DDR_CONTROL;
	if(dbw == 32)
		ddr->sdram_cfg |= 0x00080000;

#if defined(CONFIG_ECC_INIT_VIA_DDRCONTROLLER)
	d_init = 1;
	debug("DDR - 1st controller: memory initializing\n");
	/*
	 * Poll until memory is initialized.
	 * 512 Meg at 400 might hit this 200 times or so.
	 */
	while ((ddr->sdram_cfg_2 & (d_init << 4)) != 0) {
		udelay(1000);
	}
	debug("DDR: memory initialized\n\n");
	asm("sync; isync");
	udelay(500);
#endif

	return CONFIG_SYS_SDRAM_SIZE * 1024 * 1024;
}

#endif

#ifdef CONFIG_PCIE1
static struct pci_controller pcie1_hose;
#endif

#ifdef CONFIG_PCIE2
static struct pci_controller pcie2_hose;
#endif

extern int fsl_pci_setup_inbound_windows(struct pci_region *r);
extern void fsl_pci_init(struct pci_controller *hose);

int first_free_busno=0;
#ifdef CONFIG_PCI
void pci_init_board(void)
{
	volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
	uint devdisr = gur->devdisr;
	uint io_sel = (gur->pordevsr & MPC85xx_PORDEVSR_IO_SEL) >> 19;
	uint host_agent = (gur->porbmsr & MPC85xx_PORBMSR_HA) >> 16;

	debug ("   pci_init_board: devdisr=%x, io_sel=%x, host_agent=%x\n",
			devdisr, io_sel, host_agent);

	if (!(gur->pordevsr & MPC85xx_PORDEVSR_SGMII2_DIS))
		printf ("    eTSEC2 is in sgmii mode.\n");

#ifdef CONFIG_PCIE2
	{
		volatile ccsr_fsl_pci_t *pci = (ccsr_fsl_pci_t *) CONFIG_SYS_PCIE2_ADDR;
		struct pci_controller *hose = &pcie2_hose;
		int pcie_ep = (host_agent == 2) || (host_agent == 4) ||
			(host_agent == 6) || (host_agent == 0);
		int pcie_configured  = (io_sel == 0xE);
		struct pci_region *r = hose->regions;

		if (pcie_configured && !(devdisr & MPC85xx_DEVDISR_PCIE)){
			printf ("\n    PCIE2 connected to Slot 1 as %s (base address %x)",
					pcie_ep ? "End Point" : "Root Complex",
					(uint)pci);
			if (pci->pme_msg_det) {
				pci->pme_msg_det = 0xffffffff;
				debug (" with errors.  Clearing.  Now 0x%08x",pci->pme_msg_det);
			}
			printf ("\n");

			/* inbound */
			r += fsl_pci_setup_inbound_windows(r);

			/* outbound memory */
			pci_set_region(r++,
					CONFIG_SYS_PCIE2_MEM_BUS,
					CONFIG_SYS_PCIE2_MEM_PHYS,
					CONFIG_SYS_PCIE2_MEM_SIZE,
					PCI_REGION_MEM);

			/* outbound io */
			pci_set_region(r++,
					CONFIG_SYS_PCIE2_IO_BUS,
					CONFIG_SYS_PCIE2_IO_PHYS,
					CONFIG_SYS_PCIE2_IO_SIZE,
					PCI_REGION_IO);

			hose->region_count = r - hose->regions;
			hose->first_busno=first_free_busno;
			pci_setup_indirect(hose, (int) &pci->cfg_addr, (int) &pci->cfg_data);

			fsl_pci_init(hose);
			first_free_busno=hose->last_busno+1;
			printf ("    PCIE2 on bus %02x - %02x\n",
					hose->first_busno,hose->last_busno);

		} else {
			printf ("    PCIE2: disabled\n");
		}

	}
#else
	gur->devdisr |= MPC85xx_DEVDISR_PCIE2; /* disable */
#endif
#ifdef CONFIG_PCIE1
	{
		volatile ccsr_fsl_pci_t *pci = (ccsr_fsl_pci_t *) CONFIG_SYS_PCIE1_ADDR;
		struct pci_controller *hose = &pcie1_hose;
		int pcie_ep = (host_agent <= 1) || (host_agent == 4) ||
			(host_agent == 5);
		int pcie_configured  = (io_sel == 0xE);
		struct pci_region *r = hose->regions;

		if (pcie_configured && !(devdisr & MPC85xx_DEVDISR_PCIE)){
			printf ("\n    PCIE1 connected to Slot 2 as %s (base address %x)",
					pcie_ep ? "End Point" : "Root Complex",
					(uint)pci);
			if (pci->pme_msg_det) {
				pci->pme_msg_det = 0xffffffff;
				debug (" with errors.  Clearing.  Now 0x%08x",pci->pme_msg_det);
			}
			printf ("\n");

			/* inbound */
			r += fsl_pci_setup_inbound_windows(r);

			/* outbound memory */
			pci_set_region(r++,
					CONFIG_SYS_PCIE1_MEM_BUS,
					CONFIG_SYS_PCIE1_MEM_PHYS,
					CONFIG_SYS_PCIE1_MEM_SIZE,
					PCI_REGION_MEM);

			/* outbound io */
			pci_set_region(r++,
					CONFIG_SYS_PCIE1_IO_BUS,
					CONFIG_SYS_PCIE1_IO_PHYS,
					CONFIG_SYS_PCIE1_IO_SIZE,
					PCI_REGION_IO);

			hose->region_count = r - hose->regions;
			hose->first_busno=first_free_busno;

			pci_setup_indirect(hose, (int) &pci->cfg_addr, (int) &pci->cfg_data);

			fsl_pci_init(hose);

			first_free_busno=hose->last_busno+1;
			printf("    PCIE1 on bus %02x - %02x\n",
					hose->first_busno,hose->last_busno);

		} else {
			printf ("    PCIE1: disabled\n");
		}

	}
#else
	gur->devdisr |= MPC85xx_DEVDISR_PCIE; /* disable */
#endif
}
#endif

int board_early_init_r(void)
{
	const unsigned int flashbase = CONFIG_SYS_FLASH_BASE;
	const u8 flash_esel = 2;

	/*
	 * Remap Boot flash region to caching-inhibited
	 * so that flash can be erased properly.
	 */

	/* Flush d-cache and invalidate i-cache of any FLASH data */
	flush_dcache();
	invalidate_icache();

	/* invalidate existing TLB entry for flash */
	disable_tlb(flash_esel);

	set_tlb(1, flashbase, CONFIG_SYS_FLASH_BASE_PHYS,	/* tlb, epn, rpn */
			MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,	/* perms, wimge */
			0, flash_esel, BOOKE_PAGESZ_16M, 1);	/* ts, esel, tsize, iprot */
	return 0;
}


#ifdef CONFIG_TSEC_ENET
int board_eth_init(bd_t *bis)
{
	struct tsec_info_struct tsec_info[4];
	volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
	int num = 0;

#ifdef CONFIG_TSEC1
	SET_STD_TSEC_INFO(tsec_info[num], 1);
	num++;
#endif
#ifdef CONFIG_TSEC2
	SET_STD_TSEC_INFO(tsec_info[num], 2);
	num++;
#endif
#ifdef CONFIG_TSEC3
	SET_STD_TSEC_INFO(tsec_info[num], 3);
	if (!(gur->pordevsr & MPC85xx_PORDEVSR_SGMII3_DIS))
		tsec_info[num].flags |= TSEC_SGMII;
	num++;
#endif

	if (!num) {
		printf("No TSECs initialized\n");

		return 0;
	}

	tsec_eth_init(bis, tsec_info, num);

	return 0;
}
#endif

/*
 * Miscellaneous late-boot configurations
 *
 * If a VSC7385 microcode image is present, then upload it.
*/
int misc_init_r(void)
{
	int rc = 0;


#ifdef CONFIG_VSC7385_IMAGE
	printf(" uploading VSC7385 microcode.from %x\n", CONFIG_VSC7385_IMAGE);
	if (vsc7385_upload_firmware((void *) CONFIG_VSC7385_IMAGE,
					CONFIG_VSC7385_IMAGE_SIZE)) {
		puts("Failure uploading VSC7385 microcode.\n");
		rc = 1;
	}
#endif

	return rc;
}

#if defined(CONFIG_OF_BOARD_SETUP)
extern void ft_fsl_pci_setup(void *blob, const char *pci_alias,
				struct pci_controller *hose);

void ft_board_setup(void *blob, bd_t *bd)
{
	ulong base, size;

	ft_cpu_setup(blob, bd);

	base = getenv_bootm_low();
	size = getenv_bootm_size();

	fdt_fixup_memory(blob, (u64)base, (u64)size);

#ifdef CONFIG_PCIE2
	ft_fsl_pci_setup(blob, "pci1", &pcie2_hose);
#endif
#ifdef CONFIG_PCIE1
	ft_fsl_pci_setup(blob, "pci2", &pcie1_hose);
#endif
}
#endif

#ifdef CONFIG_MP
extern void cpu_mp_lmb_reserve(struct lmb *lmb);

void board_lmb_reserve(struct lmb *lmb)
{
	cpu_mp_lmb_reserve(lmb);
}
#endif
