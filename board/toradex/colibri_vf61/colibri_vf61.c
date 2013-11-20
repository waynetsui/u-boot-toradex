/*
 * Copyright 2013 Toradex, Inc.
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

#include <common.h> /* do not change order of include file */

#include <asm/arch/crm_regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/scsc_regs.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/vybrid-pins.h>
#include <asm/arch/vybrid-regs.h>
#include <asm/errno.h>
#include <asm/fec.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <fsl_esdhc.h>
#include <i2c.h>
#include <malloc.h>
#include <mmc.h>
#include <nand.h>
#include <usb/ehci-fsl.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_BOARD_LATE_INIT) && (defined(CONFIG_TRDX_CFG_BLOCK) || \
		defined(CONFIG_REVISION_TAG) || defined(CONFIG_SERIAL_TAG))
static unsigned char *config_block;
#endif

#ifdef CONFIG_FSL_ESDHC
struct fsl_esdhc_cfg esdhc_cfg[] = {
	{CONFIG_SYS_ESDHC1_BASE, 1},
};
#endif /* CONFIG_FSL_ESDHC */

void ddr_phy_init(void);
void setup_iomux_ddr(void);
void setup_iomux_nfc(void);
void setup_iomux_uart(void);

int board_early_init_f(void)
{
	setup_iomux_uart();

#ifdef CONFIG_NAND_FSL_NFC
	setup_iomux_nfc();
#endif

	return 0;
}

int board_init(void)
{
	u32 temp;
	struct vybrid_scsc_reg *scsc =
			(struct vybrid_scsc_reg *)SCSCM_BASE_ADDR;

	/* address of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

	/*
	 * Enable external 32K Oscillator
	 *
	 * The internal clock experiences significant drift
	 * so we must use the external oscillator in order
	 * to maintain correct time in the hwclock
	 */
	temp = __raw_readl(&scsc->sosc_ctr);
	temp |= VYBRID_SCSC_SICR_CTR_SOSC_EN;
	__raw_writel(temp, &scsc->sosc_ctr);

	return 0;
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	char env_str[256];

	int i;

	char *addr_str, *end;
	unsigned char bi_enetaddr[6]	= {0, 0, 0, 0, 0, 0};	/* Ethernet
								   address */
	unsigned char *mac_addr;
	unsigned char mac_addr00[6]	= {0, 0, 0, 0, 0, 0};

	size_t size			= 4096;
	unsigned char toradex_oui[3]	= { 0x00, 0x14, 0x2d };
	int valid			= 0;

	int ret;

	/* Allocate RAM area for config block */
	config_block = malloc(size);
	if (!config_block) {
		printf("Not enough malloc space available!\n");
		return -1;
	}

	/* Clear it */
	memset((void *)config_block, 0, size);

	/* Read production parameter config block from NAND */
	ret = nand_read_skip_bad(&nand_info[0], CONFIG_TRDX_CFG_BLOCK_OFFSET,
			&size, (unsigned char *)config_block);

	/* Check validity */
	if ((ret == 0) && (size > 0)) {
		mac_addr = config_block + 8;
		if (!(memcmp(mac_addr, toradex_oui, 3)))
			valid = 1;
	}

	if (!valid) {
		printf("Missing Colibri config block\n");
		memset((void *)config_block, 0, size);
	} else {
		/* Get MAC address from environment */
		addr_str = getenv("ethaddr");
		if (addr_str != NULL) {
			for (i = 0; i < 6; i++) {
				bi_enetaddr[i] = addr_str ? simple_strtoul(
						addr_str, &end, 16) : 0;
				if (addr_str)
					addr_str = (*end) ? end + 1 : end;
			}
		}

		/* Set Ethernet MAC address from config block if not already set
		*/
		if (memcmp(mac_addr00, bi_enetaddr, 6) == 0) {
			sprintf(env_str, "%02x:%02x:%02x:%02x:%02x:%02x",
				mac_addr[0], mac_addr[1], mac_addr[2],
				mac_addr[3], mac_addr[4], mac_addr[5]);
			setenv("ethaddr", env_str);
#ifndef CONFIG_ENV_IS_NOWHERE
			saveenv();
#endif
		}
	}

	/* Default memory arguments */
	if (!getenv("memargs")) {
		switch (gd->ram_size) {
		case 0x08000000:
			/* 128 MB */
			setenv("memargs", "mem=128M");
			break;
		case 0x10000000:
			/* 256 MB */
			setenv("memargs", "mem=256M");
			break;
		default:
			printf("Failed detecting RAM size.\n");
		}
	}

#ifdef CONFIG_MXC_SPI
	setup_iomux_spi();
#endif

	return 0;
}
#endif /* CONFIG_BOARD_LATE_INIT */

#ifdef CONFIG_FSL_ESDHC
int board_mmc_getcd(struct mmc *mmc)
{
	/*struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;*/
	int ret;

	__raw_writel(0x005031ef, IOMUXC_PAD_014);	/* clk */
	__raw_writel(0x005031ef, IOMUXC_PAD_015);	/* cmd */
	__raw_writel(0x005031ef, IOMUXC_PAD_016);	/* dat0 */
	__raw_writel(0x005031ef, IOMUXC_PAD_017);	/* dat1 */
	__raw_writel(0x005031ef, IOMUXC_PAD_018);	/* dat2 */
	__raw_writel(0x005031ef, IOMUXC_PAD_019);	/* dat3 */

	ret = 1;
	return ret;
}

int board_mmc_init(bd_t *bis)
{
	u32 index = 0;
	s32 status = 0;

	status |= fsl_esdhc_initialize(bis, &esdhc_cfg[index]);
	return status;
}
#endif /* CONFIG_FSL_ESDHC */

int checkboard(void)
{
	if (__raw_readl(MSCM_CP0CFG1))
		puts("Board: Colibri VF61\n");
	else
		puts("Board: Colibri VF50\n");

	return 0;
}

unsigned long ddr_ctrl_init(void)
{
	volatile u32 *pMem = 0x80000000;
	u32 temp = 0;

	int dram_size, rows, cols, banks, port;

	__raw_writel(0x00000600, DDR_CR000);	/* DDR3 */
	__raw_writel(0x00000020, DDR_CR002);	/* TINIT */
	__raw_writel(0x0000007c, DDR_CR010);	/* reset during power on */
						/* warm boot - 200ns */
	__raw_writel(0x00013880, DDR_CR011);	/* 500us - 10ns */
	__raw_writel(0x0000050c, DDR_CR012);	/* CASLAT_LIN, WRLAT */
	__raw_writel(0x15040404, DDR_CR013);	/* trc, trrd, tccd
						   tbst_int_interval */
	__raw_writel(0x1406040F, DDR_CR014);	/* tfaw, tfp, twtr, tras_min */
	__raw_writel(0x04040000, DDR_CR016);	/* tmrd, trtp */
	__raw_writel(0x006DB00C, DDR_CR017);	/* tras_max, tmod */
	__raw_writel(0x00000403, DDR_CR018);	/* tckesr, tcke */

	__raw_writel(0x01000403, DDR_CR020);	/* ap, writrp */
	__raw_writel(0x00060101, DDR_CR021);	/* trcd_int, tras_lockout
						   ccAP */
	__raw_writel(0x000B0000, DDR_CR022);	/* tdal */
	__raw_writel(0x03000200, DDR_CR023);	/* bstlen, tmrr, tdll */
	__raw_writel(0x00000006, DDR_CR024);	/* addr_mirror, reg_dimm
						   trp_ab */
	__raw_writel(0x00010000, DDR_CR025);	/* tref_enable, auto_refresh
						   arefresh */
	__raw_writel(0x0C280040, DDR_CR026);	/* tref, trfc */
	__raw_writel(0x00000005, DDR_CR028);	/* tref_interval fixed at 5 */
	__raw_writel(0x00000003, DDR_CR029);	/* tpdex */

	__raw_writel(0x0000000A, DDR_CR030);	/* txpdll */
	__raw_writel(0x00440200, DDR_CR031);	/* txsnr, txsr */
	__raw_writel(0x00010000, DDR_CR033);	/* cke_dly, en_quick_srefresh
						 * srefresh_exit_no_refresh,
						 * pwr, srefresh_exit
						 */
	__raw_writel(0x00050500, DDR_CR034);	/* cksrx, */
						/* cksre, lowpwr_ref_en */

	/* Frequency change */
	__raw_writel(0x00000100, DDR_CR038);	/* freq change... */
	__raw_writel(0x04001002, DDR_CR039);

	__raw_writel(0x00000001, DDR_CR041);	/* dfi_init_start */
	__raw_writel(0x00000000, DDR_CR045);	/* wrmd */
	__raw_writel(0x00000000, DDR_CR046);	/* rdmd */
	__raw_writel(0x00000000, DDR_CR047);	/* REF_PER_AUTO_TEMPCHK:
						 *   LPDDR2 set to 2, else 0
						 */

	/* DRAM device Mode registers */
	__raw_writel(0x00460420, DDR_CR048);	/* mr0, ddr3 burst of 8 only
						 * mr1, if freq < 125
						 * dll_dis = 1, rtt = 0
						 * if freq > 125, dll_dis = 0
						 * rtt = 3
						 */
	__raw_writel(0x00000000, DDR_CR049);	/* mr2 */
	__raw_writel(0x00000000, DDR_CR051);	/* mr3 & mrsingle_data_0 */
	__raw_writel(0x00000000, DDR_CR052);

	__raw_writel(0x00000000, DDR_CR057);	/* ctrl_raw */

	/* ECC */
	__raw_writel(0x00000000, DDR_CR058);

	/* ZQ stuff */
	__raw_writel(0x01000200, DDR_CR066);	/* zqcl, zqinit */
	__raw_writel(0x02000040, DDR_CR067);	/* zqcs */
	__raw_writel(0x00000200, DDR_CR069);	/* zq_on_sref_exit, zq_req */

	__raw_writel(0x00000040, DDR_CR070);	/* ref_per_zq */
	__raw_writel(0x00000000, DDR_CR071);	/* zqreset, ddr3 set to 0 */
	__raw_writel(0x01000000, DDR_CR072);	/* zqcs_rotate, no_zq_init */

	/* DRAM controller misc */
	__raw_writel(0x0a010200, DDR_CR073);	/* arebit, col_diff, row_diff
						   bank_diff */
	__raw_writel(0x0101ffff, DDR_CR074);	/* bank_split, addr_cmp_en
						   cmd/age cnt */
	__raw_writel(0x01010101, DDR_CR075);	/* rw same pg, rw same en
						   pri en, plen */
	__raw_writel(0x03030101, DDR_CR076);	/* #q_entries_act_dis
						 * (#cmdqueues
						 * dis_rw_grp_w_bnk_conflict
						 * w2r_split_en, cs_same_en */
	__raw_writel(0x01000101, DDR_CR077);	/* cs_map, inhibit_dram_cmd
						 * dis_interleave, swen */
	__raw_writel(0x0000000C, DDR_CR078);	/* qfull, lpddr2_s4, reduc
						   burst_on_fly */
	__raw_writel(0x01000000, DDR_CR079);	/* ctrlupd_req_per aref en
						 * ctrlupd_req
						 * ctrller busy
						 * in_ord_accept */
	/* disable interrupts */
	__raw_writel(0x1FFFFFFF, DDR_CR082);

	/* ODT */
	__raw_writel(0x01010000, DDR_CR087);	/* odt: wr_map_cs0
						 * rd_map_cs0
						 * port_data_err_id */
	__raw_writel(0x00040000, DDR_CR088);	/* todtl_2cmd */
	__raw_writel(0x00000002, DDR_CR089);	/* add_odt stuff */

	__raw_writel(0x00020000, DDR_CR091);
	__raw_writel(0x00000000, DDR_CR092);	/* tdqsck _min, max */

	__raw_writel(0x00002819, DDR_CR096);	/* wlmrd, wldqsen */

	/* AXI ports */
	__raw_writel(0x00202000, DDR_CR105);
	__raw_writel(0x20200000, DDR_CR106);
	__raw_writel(0x00002020, DDR_CR110);
	__raw_writel(0x00202000, DDR_CR114);
	__raw_writel(0x20200000, DDR_CR115);

//synchronous vs. asynchronous mode
//DDRMC_CR117 17–16 Clock domain relativity between AXI port 0 and memory
//controller core.
//Set to 0 for asynchronous or set to AXI0_FITYPREG 2 for 1:2 port:core
//pseudo-sync.
//3 sync hangs!
	__raw_writel(0x00000101, DDR_CR117);	/* FIFO type (0-async, 1-2:1
						 *	2-1:2, 3- sync, w_pri
						 * r_pri
						 */
	__raw_writel(0x01010000, DDR_CR118);	/* w_pri, rpri, en */
	__raw_writel(0x00000000, DDR_CR119);	/* fifo_type? */

	__raw_writel(0x02020000, DDR_CR120);
	__raw_writel(0x00000202, DDR_CR121);
	__raw_writel(0x01010064, DDR_CR122);
	__raw_writel(0x00010101, DDR_CR123);
	__raw_writel(0x00000064, DDR_CR124);

	/* TDFI */
	__raw_writel(0x00000000, DDR_CR125);
	__raw_writel(0x00000B00, DDR_CR126);	/* PHY rdlat */
	__raw_writel(0x00000000, DDR_CR127);	/* dram ck dis */

	__raw_writel(0x00000000, DDR_CR131);
	__raw_writel(0x00000506, DDR_CR132);	/* wrlat, rdlat */
	__raw_writel(0x00020000, DDR_CR137);
	__raw_writel(0x04070303, DDR_CR139);

	__raw_writel(0x00000000, DDR_CR136);	/* ??? why later */

	__raw_writel(0x682C0000, DDR_CR154);	/* Bit 19–18 DDR_SEL_PAD_Contr
						   DDR_SEL = 11 (DDR3 mode) */
	__raw_writel(0x00000102, DDR_CR155);	/* pad_ibe, _sel */
	__raw_writel(0x00000006, DDR_CR158);	/* twr */
	__raw_writel(0x00000006, DDR_CR159);	/* todth */

	ddr_phy_init();
	__raw_writel(0x1FFFFFFF, DDR_CR082);	/* ??? */
	__raw_writel(0x00000601, DDR_CR000);	/* DDR3, start */

	udelay(200);

	/* auto detect memory size */
	temp = pMem[0];
	pMem[0] = 0xabadcafe;
	asm volatile("" ::: "memory");
	if (pMem[0] == pMem[128*1024*1024/4]) {
		/* only detected 128 MB (e.g. wrap around detected)
		   re-configure memory controller for 128 MB */
		__raw_writel(0x00000600, DDR_CR000);	/* DDR3, stop */
		__raw_writel(0x0a010300, DDR_CR073);	/* arebit, col_diff,
							   row_diff,
							   bank_diff */
		__raw_writel(0x00000601, DDR_CR000);	/* DDR3, start */

		udelay(200);
	}
	pMem[0] = temp;

	rows = (__raw_readl(DDR_CR001) & 0x1F) -
	       ((__raw_readl(DDR_CR073) >> 8) & 3);
	cols = ((__raw_readl(DDR_CR001) >> 8) & 0xF) -
	       ((__raw_readl(DDR_CR073) >> 16) & 7);
	banks = 1 << (3 - (__raw_readl(DDR_CR073) & 3));
	port = ((__raw_readl(DDR_CR078) >> 8) & 1) ? 1 : 2;

	dram_size = (1 << (rows + cols)) * banks * port;

	return dram_size;
}

void ddr_phy_init(void)
{
#define PHY_DQ_TIMING		0x00002213
#define PHY_DQS_TIMING		0x00002615
#define PHY_CTRL		0x00200000
#define PHY_MASTER_CTRL		0x00000020
#define PHY_SLAVE_CTRL		0x00002000

	/* phy_dq_timing_reg freq set 0 */
	__raw_writel(PHY_DQ_TIMING, DDR_PHY000);
	__raw_writel(PHY_DQ_TIMING, DDR_PHY016);
	__raw_writel(PHY_DQ_TIMING, DDR_PHY032);

	/* phy_dqs_timing_reg freq set 0 */
	__raw_writel(PHY_DQS_TIMING, DDR_PHY001);
	__raw_writel(PHY_DQS_TIMING, DDR_PHY017);
	__raw_writel(PHY_DQS_TIMING, DDR_PHY033);

	/* phy_gate_lpbk_ctrl_reg freq set 0 */
	__raw_writel(PHY_CTRL, DDR_PHY002);	/* read delay bit21:19 */
	__raw_writel(PHY_CTRL, DDR_PHY018);	/* phase_detect_sel bit18:16 */
	__raw_writel(PHY_CTRL, DDR_PHY034);	/* bit lpbk_ctrl bit12 */

	/* phy_dll_master_ctrl_reg freq set 0 */
	__raw_writel(PHY_MASTER_CTRL, DDR_PHY003);
	__raw_writel(PHY_MASTER_CTRL, DDR_PHY019);
	__raw_writel(PHY_MASTER_CTRL, DDR_PHY035);

	/* phy_dll_slave_ctrl_reg freq set 0 */
	__raw_writel(PHY_SLAVE_CTRL, DDR_PHY004);
	__raw_writel(PHY_SLAVE_CTRL, DDR_PHY020);
	__raw_writel(PHY_SLAVE_CTRL, DDR_PHY036);

	__raw_writel(0x00001105, DDR_PHY050);
}

static int do_mc_reg_dump(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[])
{
	u32 addr;
	for (addr = DDR_CR000; addr <= DDR_CR161; addr += 4) {
		printf("DDR_CR[%d] = 0x%08x\n", (addr-DDR_CR000)/4,
				__raw_readl(addr));
	}
	for (addr = DDR_PHY000; addr <= DDR_PHY052; addr += 4) {
		printf("DDR_PHY[%d] = 0x%08x\n", (addr-DDR_PHY000)/4,
				__raw_readl(addr));
	}

	return 0;
}
U_BOOT_CMD(
	mcdump, 1, 1, do_mc_reg_dump,
	"Dump MC registers",
	""
);

int dram_init(void)
{
	setup_iomux_ddr();
	gd->ram_size = ddr_ctrl_init();

	return 0;
}

#if defined(CONFIG_CMD_NET)
int fecpin_setclear(struct eth_device *dev, int setclear)
{
	struct fec_info_s *info = (struct fec_info_s *)dev->priv;

	/* 5.2.1 Software MUX Pad Control Register 0 (IOMUXC_PTA6) */
	__raw_writel(0x00103191, IOMUXC_PAD_000);	/* RMII_CLKOUT */

	if (setclear) {
		if (info->iobase == CONFIG_SYS_FEC1_IOBASE) {
			__raw_writel(0x00103192, IOMUXC_PAD_054); /* MDC */
			__raw_writel(0x00103193, IOMUXC_PAD_055); /* MDIO */
			__raw_writel(0x00103191, IOMUXC_PAD_056); /* RxDV */
			__raw_writel(0x00103191, IOMUXC_PAD_057); /* RxD1 */
			__raw_writel(0x00103191, IOMUXC_PAD_058); /* RxD0 */
			__raw_writel(0x00103191, IOMUXC_PAD_059); /* RxER */
			__raw_writel(0x00103192, IOMUXC_PAD_060); /* TxD1 */
			__raw_writel(0x00103192, IOMUXC_PAD_061); /* TxD0 */
			__raw_writel(0x00103192, IOMUXC_PAD_062); /* TxEn */
		}
	} else {
		if (info->iobase == CONFIG_SYS_FEC1_IOBASE) {
			__raw_writel(0x00003192, IOMUXC_PAD_054); /* MDC */
			__raw_writel(0x00003193, IOMUXC_PAD_055); /* MDIO */
			__raw_writel(0x00003191, IOMUXC_PAD_056); /* RxDV */
			__raw_writel(0x00003191, IOMUXC_PAD_057); /* RxD1 */
			__raw_writel(0x00003191, IOMUXC_PAD_058); /* RxD0 */
			__raw_writel(0x00003191, IOMUXC_PAD_059); /* RxER */
			__raw_writel(0x00003192, IOMUXC_PAD_060); /* TxD1 */
			__raw_writel(0x00003192, IOMUXC_PAD_061); /* TxD0 */
			__raw_writel(0x00003192, IOMUXC_PAD_062); /* TxEn */
		}
	}

	return 0;
}
#endif /* CONFIG_CMD_NET */

#ifdef CONFIG_REVISION_TAG
u32 get_board_rev(void)
{
#ifdef CONFIG_BOARD_LATE_INIT
	int i;
	unsigned short major = 0, minor = 0, release = 0;
	size_t size = 4096;

	if (config_block == NULL)
		return 0;

	/* Parse revision information in config block */
	for (i = 0; i < (size - 8); i++) {
		if (config_block[i] == 0x02 && config_block[i+1] == 0x40 &&
				config_block[i+2] == 0x08) {
			break;
		}
	}

	major = (config_block[i+3] << 8) | config_block[i+4];
	minor = (config_block[i+5] << 8) | config_block[i+6];
	release = (config_block[i+7] << 8) | config_block[i+8];

	/* Check validity */
	if (major)
		return ((major & 0xff) << 8) | ((minor & 0xf) << 4) |
				((release & 0xf) + 0xa);
	else
		return 0;
#else
	return 0;
#endif /* CONFIG_BOARD_LATE_INIT */
}
#endif /* CONFIG_REVISION_TAG */

#ifdef CONFIG_SERIAL_TAG
void get_board_serial(struct tag_serialnr *serialnr)
{
#ifdef CONFIG_BOARD_LATE_INIT
	int array[8];
	int i;
	unsigned int serial		= 0;
	unsigned int serial_offset	= 11;

	if (config_block == NULL) {
		serialnr->low = 0;
		serialnr->high = 0;
		return;
	}

	/* Get MAC address from config block */
	memcpy(&serial, config_block + serial_offset, 3);
	serial = ntohl(serial);
	serial >>= 8;

	/* Check validity */
	if (serial) {
		/* Convert to Linux serial number format (hexadecimal coded
		   decimal) */
		i = 7;
		while (serial) {
			array[i--] = serial % 10;
			serial /= 10;
		}
		while (i >= 0)
			array[i--] = 0;
		serial = array[0];
		for (i = 1; i < 8; i++) {
			serial *= 16;
			serial += array[i];
		}
	}

	serialnr->low = serial;
#else
	serialnr->low = 0;
#endif /* CONFIG_BOARD_LATE_INIT */
	serialnr->high = 0;
}
#endif /* CONFIG_SERIAL_TAG */

void setup_iomux_ddr(void)
{
#define DDR_IOMUX	0x00000140
#define DDR_IOMUX1	0x00010140 /* Differential input mode */
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_RESET);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A15);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A14);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A13);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A12);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A11);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A10);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A9);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A8);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A7);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A6);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A5);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A4);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A3);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A2);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A1);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_A0);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_BA2);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_BA1);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_BA0);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_CAS);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_CKE);
	__raw_writel(DDR_IOMUX1, IOMUXC_DDR_CLK);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_CS);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D15);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D14);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D13);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D12);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D11);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D10);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D9);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D8);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D7);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D6);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D5);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D4);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D3);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D2);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D1);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_D0);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_DQM1);	/* UDM */
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_DQM0);	/* LDM */
	__raw_writel(DDR_IOMUX1, IOMUXC_DDR_DQS1);	/* UDQS */
	__raw_writel(DDR_IOMUX1, IOMUXC_DDR_DQS0);	/* LDQS */
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_RAS);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_WE);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_ODT0);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_ODT1);
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_DDRBYTE1);	/* ? */
	__raw_writel(DDR_IOMUX, IOMUXC_DDR_DDRBYTE0);	/* ? */
}

#ifdef CONFIG_NAND_FSL_NFC
void setup_iomux_nfc(void)
{
	__raw_writel(0x002038df, IOMUXC_PAD_071);
	__raw_writel(0x002038df, IOMUXC_PAD_072);
	__raw_writel(0x002038df, IOMUXC_PAD_073);
	__raw_writel(0x002038df, IOMUXC_PAD_074);
	__raw_writel(0x002038df, IOMUXC_PAD_075);
	__raw_writel(0x002038df, IOMUXC_PAD_076);
	__raw_writel(0x002038df, IOMUXC_PAD_077);
	__raw_writel(0x002038df, IOMUXC_PAD_078);
	__raw_writel(0x005038d2, IOMUXC_PAD_094);
	__raw_writel(0x005038d2, IOMUXC_PAD_095);
	__raw_writel(0x006038d2, IOMUXC_PAD_097);
	__raw_writel(0x005038dd, IOMUXC_PAD_099);
	__raw_writel(0x006038d2, IOMUXC_PAD_100);
	__raw_writel(0x006038d2, IOMUXC_PAD_101);
}
#endif /* CONFIG_NAND_FSL_NFC */

#ifdef CONFIG_MXC_SPI
void setup_iomux_spi(void)
{
}
#endif /* CONFIG_MXC_SPI */

void setup_iomux_uart(void)
{
	__raw_writel(0x002011a2, IOMUXC_PAD_026); /* UART_C_TXD: SCI1_TX */
	__raw_writel(0x002011a1, IOMUXC_PAD_027); /* UART_C_RXD: SCI1_RX */
	__raw_writel(0x001011a2, IOMUXC_PAD_032); /* UART_A_TXD: SCI0_TX */
	__raw_writel(0x001011a1, IOMUXC_PAD_033); /* UART_A_RXD: SCI0_RX */
	__raw_writel(0x001011a2, IOMUXC_PAD_079); /* UART_B_TXD: SCI2_TX */
	__raw_writel(0x001011a1, IOMUXC_PAD_080); /* UART_B_RXD: SCI2_RX */
}
