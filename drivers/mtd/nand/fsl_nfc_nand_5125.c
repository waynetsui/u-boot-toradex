/*
 * Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * Based on drivers/mtd/nand/mpc5121_nand.c
 * which was based on drivers/mtd/nand/mxc_nd.c
 * ported by Cloudy chen <chen_yunsong@mtcera> LimePC Multimedia Technologies Co., Limited
 *from mpc5121 to mpc5125
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */
#include <command.h>
#include <common.h>
#include <malloc.h>

#include <mpc5125_nfc.h>
#include <asm/io.h>
#include <nand.h>
#include "mpc5125_nfc_struct.h"
#define MIN(x, y)		((x < y) ? x : y)

#define NAND_DEBUG_INFO()	printf("%s line:%d\n",__func__,__LINE__)
#ifndef CONFIG_PPC
#define CONFIG_PPC
#endif

static struct fsl_nfc_private {
	struct mtd_info mtd;
	char spare_only;
	char status_req;
	u16 col_addr;
	int writesize;
	int sparesize;
	int width;
	int chipsel;
} *priv;

static int get_status;
static int get_id;

#define IS_2K_PAGE_NAND		(priv->writesize == 2048)
#define IS_4K_PAGE_NAND		(priv->writesize == 4096)
#define IS_LARGE_PAGE_NAND	(priv->writesize > 512)

/*
 * Define delays in microsec for NAND device operations
 */
#define TROP_US_DELAY			2000

#if defined(CONFIG_PPC)
#define NFC_WRITEL(r, v)		out_be32(r, v)
#define NFC_WRITEW(r, v)		out_be16(r, v)
#define NFC_WRITEB(r, v)		out_8(r, v)
#define NFC_READL(r)			in_be32(r)
#define NFC_READW(r)			in_be16(r)
#define NFC_READB(r)			in_8(r)
#elif defined(CONFIG_ARM)
#define NFC_WRITEL(r, v)		writel(v, r)
#define NFC_WRITEW(r, v)		writew(v, r)
#define NFC_WRITEB(r, v)		writeb(r, v)
#define NFC_READL(r)			readl(r)
#define NFC_READW(r)			readw(r)
#define NFC_READB(r)			readb(r)
#endif


#ifdef CONFIG_MTD_NAND_FSL_NFC_SWECC
static int hardware_ecc;
#else
static int hardware_ecc = 0;
#endif


static void mpc5125_nfc_addr_cycle(struct mtd_info *mtd, int column, int page);
u8 mpc5125_nfc_get_id(struct mtd_info *mtd,int col);
static void mpc5125_cfg_iopad_init(void)
{
	//printf("enter mpc5125_cfg_iopad_init\n");
		/* FLASH IO PAD Setting */
		out_8(ioctl + BALL_NFC_IO0, PAD_NFC_IO | DS_MSR_3);
		out_8(ioctl + BALL_NFC_IO1, PAD_NFC_IO | DS_MSR_3);
		out_8(ioctl + BALL_NFC_IO2, PAD_NFC_IO | DS_MSR_3);
		out_8(ioctl + BALL_NFC_IO3, PAD_NFC_IO | DS_MSR_3);
		out_8(ioctl + BALL_NFC_IO4, PAD_NFC_IO | DS_MSR_3);
		out_8(ioctl + BALL_NFC_IO5, PAD_NFC_IO | DS_MSR_3);
		out_8(ioctl + BALL_NFC_IO6, PAD_NFC_IO | DS_MSR_3);
		out_8(ioctl + BALL_NFC_IO7, PAD_NFC_IO | DS_MSR_3);

		/* FLASH CONTROL PAD Setting */
		out_8(ioctl + BALL_NFC_ALE, PAD_NFC_ALE | DS_MSR_3);
		out_8(ioctl + BALL_NFC_CLE, PAD_NFC_CLE | DS_MSR_3);
		out_8(ioctl + BALL_NFC_WE, PAD_NFC_WE | DS_MSR_3);
		out_8(ioctl + BALL_NFC_RE, PAD_NFC_RE | DS_MSR_3);

		/* NFC_CE0 */
		out_8(ioctl + BALL_NFC_CE0, PAD_NFC_CE0 | DS_MSR_3);

		out_8(ioctl + BALL_NFC_RB0, PAD_NFC_RB0| ST_Enabled | DS_MSR_3 | PUD_PUE);
}

/*
 * OOB placement block for use with hardware ecc generation
 */
static struct nand_ecclayout nand_hw_eccoob_512 = {
	.eccbytes = 9,
	.eccpos = {
		7, 8, 9, 10, 11, 12, 13, 14, 15,
	},
	.oobfree = {
		{0, 5} /* byte 5 is factory bad block marker */
	},
};

static struct nand_ecclayout nand_hw_eccoob_2k = {
	.eccbytes = 36,
	.eccpos = {
		/* 9 bytes of ecc for each 512 bytes of data */
		7, 8, 9, 10, 11, 12, 13, 14, 15,
		23, 24, 25, 26, 27, 28, 29, 30, 31,
		39, 40, 41, 42, 43, 44, 45, 46, 47,
		55, 56, 57, 58, 59, 60, 61, 62, 63,
	},
	.oobfree = {
		{2, 5}, /* bytes 0 and 1 are factory bad block markers */
		{16, 7},
		{32, 7},
		{48, 7},
	},
};
#if 1
/*for ecc_MODE=0x6 45bytes*2*/
static struct nand_ecclayout nand_hw_eccoob_4k = {
	.eccbytes = 90,	/* actually 72 but only room for 64 */
	.eccpos = {
		/* 9 bytes of ecc for each 512 bytes of data */
		19,20,21,22,23,24,25,26,27,28,29,30,
		31,32,33,34,35,36,37,38,39,40,
		41, 42, 43, 44, 45, 46, 47,48,49,50,
		51,52,53,54,55, 56, 57, 58, 59, 60,
		61, 62, 63,
		83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,
		98,99,100,
		101,102,103,104,105,106,107,108,109,110,
		111,112,113,114,115,116,117,118,119,120,
		121,122,123,124,125,126,127/* 120, 121, 122, 123, 124, 125, 126, 127, */
	},
	.oobavail = 30,
	.oobfree = { {4, 15}, {68, 15}}
};
#else
static struct nand_ecclayout nand_hw_eccoob_4k = {
	.eccbytes = 64,	/* actually 72 but only room for 64 */
	.eccpos = {
		/* 9 bytes of ecc for each 512 bytes of data */
		7, 8, 9, 10, 11, 12, 13, 14, 15,
		23, 24, 25, 26, 27, 28, 29, 30, 31,
		39, 40, 41, 42, 43, 44, 45, 46, 47,
		55, 56, 57, 58, 59, 60, 61, 62, 63,
		71, 72, 73, 74, 75, 76, 77, 78, 79,
		87, 88, 89, 90, 91, 92, 93, 94, 95,
		103, 104, 105, 106, 107, 108, 109, 110, 111,
		119, /* 120, 121, 122, 123, 124, 125, 126, 127, */
	},
	.oobfree = {
		{2, 5}, /* bytes 0 and 1 are factory bad block markers */
		{16, 7},
		{32, 7},
		{48, 7},
		{64, 7},
		{80, 7},
		{96, 7},
		{112, 7},
	},
};
#endif
static struct nand_ecclayout nand_hw_eccoob_4k_218_spare = {
	.eccbytes = 64,	/* actually 144 but only room for 64 */
	.eccpos = {
		/* 18 bytes of ecc for each 512 bytes of data */
		7, 8, 9, 10, 11, 12, 13, 14, 15,
		    16, 17, 18, 19, 20, 21, 22, 23, 24,
		33, 34, 35, 36, 37, 38, 39, 40, 41,
		    42, 43, 44, 45, 46, 47, 48, 49, 50,
		59, 60, 61, 62, 63, 64, 65, 66, 67,
		    68, 69, 70, 71, 72, 73, 74, 75, 76,
		85, 86, 87, 88, 89, 90, 91, 92, 93,
		    94, /* 95, 96, 97, 98, 99, 100, 101, 102,
		111, 112, 113, 114, 115, 116, 117, 118, 119,
		    120, 121, 122, 123, 124, 125, 126, 127, 128,
		137, 138, 139, 140, 141, 142, 143, 144, 145,
		    146, 147, 148, 149, 150, 151, 152, 153, 154,
		163, 164, 165, 166, 167, 168, 169, 170, 171,
		    172, 173, 174, 175, 176, 177, 178, 179, 180,
		189, 190, 191, 192, 193, 194, 195, 196, 197,
		    198, 199, 200, 201, 202, 203, 204, 205, 206, */
	},
	.oobfree = {
		{2, 5}, /* bytes 0 and 1 are factory bad block markers */
		{25, 8},
		{51, 8},
		{77, 8},
		{103, 8},
		{129, 8},
		{155, 8},
		{181, 8},
	},
};

 struct mpc5125_nfc_save_struct g_nfc_save;
/*
 * Functions to transfer data to/from spare erea.
 */
static void copy_from_spare(struct mtd_info *mtd, void *pbuf, int len)
{

	u16 ooblen = mtd->oobsize;
	u8 i, count;
	unsigned int   sbsize, blksize;
	/* Calculate number of valid bytes in each spare buffer */
	count = mtd->writesize >> 11;
	count=(count>0)?count:1;
	sbsize = (ooblen / count >> 1) << 1;
	/*printk("%s line:%d  %s len:%d\n",__FUNCTION__,__LINE__,wr?"write":"read",size);*/
	for(i=0;(i<count)&&len;i++)
	{
		blksize = min(sbsize, len);
		memcpy_fromio(pbuf,
			CONFIG_SYS_NAND_BASE + NFC_SPARE_AREA(i), blksize);
		/*mpc5125_spare_debug(buffer,blksize);*/
		pbuf += blksize;
		len -= blksize;
	}
}

static void copy_to_spare(struct mtd_info *mtd, void *pbuf, int len)
{

	u16 ooblen = mtd->oobsize;
	u8 i, count;
	unsigned int   sbsize, blksize;
	/* Calculate number of valid bytes in each spare buffer */
	count = mtd->writesize >> 11;
	count=(count>0)?count:1;
	sbsize = (ooblen / count >> 1) << 1;
	/*printk("%s line:%d  %s len:%d\n",__FUNCTION__,__LINE__,wr?"write":"read",size);*/
	for(i=0;(i<count)&&len;i++)
	{
		blksize = min(sbsize, len);
		memcpy_toio(CONFIG_SYS_NAND_BASE + NFC_SPARE_AREA(i) ,
							pbuf, blksize);
		/*mpc5125_spare_debug(buffer,blksize);*/
		pbuf += blksize;
		len -= blksize;
	}

}
static  u32 nfc_read(uint reg)
{
	return in_be32(CONFIG_SYS_NAND_BASE + reg);
}

/* Write NFC register */
static void nfc_write(uint reg, u32 val)
{
	out_be32(CONFIG_SYS_NAND_BASE + reg, val);
}

/* Set bits in NFC register */
static inline void nfc_set(uint reg, u32 bits)
{
	nfc_write(reg, nfc_read(reg) | bits);
}

/* Clear bits in NFC register */
static inline void nfc_clear( uint reg, u32 bits)
{
	nfc_write(reg, nfc_read( reg) & ~bits);
}

static void nfc_set_field(u32 reg, u32 mask, u32 shift, u32 val)
{
	out_be32(CONFIG_SYS_NAND_BASE + reg,
			(in_be32(CONFIG_SYS_NAND_BASE + reg) & (~mask))
			| val << shift);
}
static  int nfc_get_field(u32 reg, u32 field_mask)
{
	return in_be32(CONFIG_SYS_NAND_BASE + reg) & field_mask;
}

/* clear cmd_done and cmd_idle falg for the coming command */
static void mpc5125_nfc_clear(void)
{

	nfc_write(NFC_IRQ_STATUS, 1 << CMD_DONE_CLEAR_SHIFT);
	nfc_write(NFC_IRQ_STATUS, 1 << IDLE_CLEAR_SHIFT);

}
/*!
 * This function polls the NFC to wait for the basic operation to complete by
 * checking the INT bit of config2 register.
 *
 * @max_retries	number of retry attempts (separated by 1 us)
 */
static void wait_op_done(int max_retries)
{
	unsigned int tmp;
	mpc5125_nfc_clear();
	nfc_set_field(NFC_FLASH_CMD2, START_MASK,
			START_SHIFT, 0x01);
	while (1) {
		max_retries--;
		tmp=in_be32(CONFIG_SYS_NAND_BASE + NFC_IRQ_STATUS);
		if(((tmp&(3<<29))==(3<<29))||((max_retries <= 0)))
		{
			out_be32(CONFIG_SYS_NAND_BASE+NFC_IRQ_STATUS,(3<<17));
			break;
		}
		udelay(1);
	}
	if (max_retries <= 0)
		printk( "%s: INT not set\n",__func__);
	mpc5125_nfc_clear();
}


/* Invoke command cycle */
static void
mpc5125_nfc_send_cmd(u32 cmd_byte1,u32 cmd_byte2, u32 cmd_code)
{
	mpc5125_nfc_clear();
	nfc_set_field( NFC_FLASH_CMD2, CMD_BYTE1_MASK,
			CMD_BYTE1_SHIFT, cmd_byte1);

	nfc_set_field( NFC_FLASH_CMD1, CMD_BYTE2_MASK,
			CMD_BYTE2_SHIFT, cmd_byte2);

	nfc_set_field( NFC_FLASH_CMD2, BUFNO_MASK,
			BUFNO_SHIFT, 0);

	nfc_set_field(NFC_FLASH_CMD2, CMD_CODE_MASK,
			CMD_CODE_SHIFT, cmd_code);

	if (cmd_code == RANDOM_OUT_CMD_CODE)
		nfc_set_field( NFC_FLASH_CMD2, BUFNO_MASK,
			BUFNO_SHIFT, 1);
	/*
	nfc_set_field(NFC_FLASH_CMD2, START_MASK,
			START_SHIFT, 0x01);
	*/
}
static  void mpc5125_nfc_send_one_byte(u32 cmd_byte1, u32 cmd_code)
{
	mpc5125_nfc_clear();

	nfc_set_field( NFC_FLASH_CMD2, CMD_BYTE1_MASK,
			CMD_BYTE1_SHIFT, cmd_byte1);

	nfc_set_field( NFC_FLASH_CMD2, BUFNO_MASK,
			BUFNO_SHIFT, 0);

	nfc_set_field(NFC_FLASH_CMD2, CMD_CODE_MASK,
			CMD_CODE_SHIFT, cmd_code);
	/*
	nfc_set_field(NFC_FLASH_CMD2, START_MASK,
			START_SHIFT, 0x01);
	*/
}

static void copy_id_to_sram(void)
{
	unsigned int *area_buf;
	area_buf = CONFIG_SYS_NAND_BASE + NFC_MAIN_AREA(0);
	*area_buf++=nfc_read(NFC_FLASH_STATUS1);
	*area_buf++=nfc_read(NFC_FLASH_STATUS2);
}
/* Write command to NAND flash */
static void mpc5125_nfc_command(struct mtd_info *mtd, unsigned command,
						int column, int page)
{
	priv->col_addr = (column >= 0) ? column : 0;
	priv->spare_only = 0;
	get_id = 0;
	get_status = 0;
	MTDDEBUG (MTD_DEBUG_LEVEL2, "command %08x page:%08x column:%08x\n",command,page,column);
	switch (command) {
	case NAND_CMD_PAGEPROG:
		mpc5125_nfc_send_cmd(PROGRAM_PAGE_CMD_BYTE1,
								PROGRAM_PAGE_CMD_BYTE2,
								PROGRAM_PAGE_CMD_CODE);
		wait_op_done(TROP_US_DELAY);
		nfc_write(NFC_ROW_ADDR_INC, 0x0);
		nfc_write(NFC_FLASH_CMD2, 0x8001c003);
		wait_op_done(TROP_US_DELAY);
		break;
	/*
	 * NFC does not support sub-page reads and writes,
	 * so emulate them using full page transfers.
	 */
	case NAND_CMD_READ0:
		column = 0;
		goto read0;
		break;

	case NAND_CMD_READ1:
		priv->col_addr += 256;
		command = NAND_CMD_READ0;
		column = 0;
		goto read0;
		break;

	case NAND_CMD_READOOB:
		priv->spare_only = 1;
		command = NAND_CMD_READ0;
		column = 0;
read0:
		mpc5125_nfc_send_cmd( PAGE_READ_CMD_BYTE1,
								PAGE_READ_CMD_BYTE2,
								READ_PAGE_CMD_CODE);
		break;

	case NAND_CMD_SEQIN:
		/*mpc5125_nfc_command(mtd,NAND_CMD_READ0, column, page);*/
		column = 0;
		goto read0;
		break;

	case NAND_CMD_ERASE1:
		mpc5125_nfc_send_cmd( ERASE_CMD_BYTE1,
								ERASE_CMD_BYTE2,
								ERASE_CMD_CODE);
		break;
	case NAND_CMD_ERASE2:
		return;
	case NAND_CMD_READID:
		get_id = 1;
		mpc5125_nfc_send_one_byte(command, READ_ID_CMD_CODE);
		wait_op_done(TROP_US_DELAY);
		copy_id_to_sram();
		
		return;
	case NAND_CMD_STATUS:
		get_status = 1;
		mpc5125_nfc_send_one_byte(command, STATUS_READ_CMD_CODE);
		break;
	case NAND_CMD_RNDOUT:
		mpc5125_nfc_send_cmd( RANDOM_OUT_CMD_BYTE1,
								RANDOM_OUT_CMD_BYTE2,
								RANDOM_OUT_CMD_CODE);
		break;

	
	case NAND_CMD_RESET:
		mpc5125_nfc_send_one_byte(command, RESET_CMD_CODE);
		break;
	
	default:
		return;
	}
	mpc5125_nfc_addr_cycle(mtd, priv->col_addr, page);
	wait_op_done(TROP_US_DELAY);
	MTDDEBUG (MTD_DEBUG_LEVEL2,"%s line:%d\n",__func__,__LINE__);
}


/*!
 * This function requests the NFC to perform a read of the
 * NAND device status and returns the current status.
 *
 * @return	device status
 */
static u16 get_dev_status(void)
{

	u32 flash_status = 0;
	u8 *pstatus;

	flash_status = nfc_read(NFC_FLASH_STATUS2);
	pstatus = (u8 *)&flash_status;
	return *(pstatus + 3);
}
 u8 mpc5125_nfc_get_id(struct mtd_info *mtd,int col)
{
	u32 flash_id1 = 0;
	u8 *pid;

	flash_id1 = nfc_read(NFC_FLASH_STATUS1);
	pid = (u8 *)&flash_id1;

	return *(pid+col );
}

/*!
 * This functions is used by upper layer to checks if device is ready
 *
 * @mtd		MTD structure for the NAND Flash
 *
 * @return	0 if device is busy else 1
 */
static int fsl_nfc_dev_ready(struct mtd_info *mtd)
{
	return 1;
}

/*!
 * This function reads byte from the NAND Flash
 *
 * @mtd		MTD structure for the NAND Flash
 *
 * @return	data read from the NAND Flash
 */
static u_char fsl_nfc_read_byte(struct mtd_info *mtd)
{
	void *area_buf;
	u_char rv;

	if (priv->status_req) {
		rv = get_dev_status() & 0xff;
		return rv;
	}

	if (priv->spare_only)
		area_buf = CONFIG_SYS_NAND_BASE + NFC_SPARE_AREA(0);
	else 
		area_buf = CONFIG_SYS_NAND_BASE + NFC_MAIN_AREA(0);

	rv = NFC_READB(area_buf + priv->col_addr);
	priv->col_addr++;
	return rv;
}

/*!
  * This function reads word from the NAND Flash
  *
  * @mtd	MTD structure for the NAND Flash
  *
  * @return	data read from the NAND Flash
  */
static u16 fsl_nfc_read_word(struct mtd_info *mtd)
{
	u16 rv;
	void *area_buf;

	/* If we are accessing the spare region */
	if (priv->spare_only)
		area_buf = CONFIG_SYS_NAND_BASE + NFC_SPARE_AREA(0);
	else
		area_buf = CONFIG_SYS_NAND_BASE + NFC_MAIN_AREA(0);

	/* Update saved column address */
	rv = NFC_READW(area_buf + priv->col_addr);
	priv->col_addr += 2;

	return rv;
}


/*!
 * This function reads byte from the NAND Flash
 *
 * @mtd		MTD structure for the NAND Flash
 *
 * @return	data read from the NAND Flash
 */
static u_char fsl_nfc_read_byte16(struct mtd_info *mtd)
{
	/* Check for status request */
	if (priv->status_req)
		return (get_dev_status() & 0xff);

	return fsl_nfc_read_word(mtd) & 0xff;
}

/*!
 * This function writes data of length \b len from buffer \b buf to the NAND
 * internal RAM buffer's MAIN area 0.
 *
 * @mtd		MTD structure for the NAND Flash
 * @buf		data to be written to NAND Flash
 * @len		number of bytes to be written
 */
static void fsl_nfc_write_buf(struct mtd_info *mtd,
					const u_char *buf, int len)
{

	if (priv->col_addr >= mtd->writesize || priv->spare_only) {
		copy_to_spare(mtd, (char *)buf, len);
		return;
	} else {
		unsigned int size,i;
		unsigned int c=priv->col_addr;
		priv->col_addr += len;
		for(i=(c/PAGE_2K);i<4;i++)
		{
			size=min(len,PAGE_2K);
			memcpy_toio(CONFIG_SYS_NAND_BASE + NFC_MAIN_AREA(i)+c, (void *)buf, size);
			buf+=size;
			len-=size;
			if(!len)break;
		}
	}
}

/*!
 * This function id is used to read the data buffer from the NAND Flash. To
 * read the data from NAND Flash first the data output cycle is initiated by
 * the NFC, which copies the data to RAMbuffer. This data of length \b len is
 * then copied to buffer \b buf.
 *
 * @mtd		MTD structure for the NAND Flash
 * @buf		data to be read from NAND Flash
 * @len		number of bytes to be read
 */
static void fsl_nfc_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{

	if (priv->col_addr >= mtd->writesize || priv->spare_only) {
		copy_from_spare(mtd, buf, len);
		return;
	} else {
		unsigned int size,i;
		unsigned int c=priv->col_addr;
		priv->col_addr += len;
		for(i=(c/PAGE_2K);i<4;i++)
		{
			size=min(len,PAGE_2K);
			memcpy_fromio(buf,CONFIG_SYS_NAND_BASE + NFC_MAIN_AREA(i) + c, size);
			buf+=size;
			len-=size;
			if(!len)break;
		}
		
	}
}

/*!
 * This function is used by the upper layer to verify the data in NAND Flash
 * with the data in the \b buf.
 *
 * @mtd		MTD structure for the NAND Flash
 * @buf		data to be verified
 * @len		length of the data to be verified
 *
 * @return	-1 if error else 0
 *
 */
static int fsl_nfc_verify_buf(struct mtd_info *mtd, const u_char *buf,
					int len)
{
	void *main_buf = CONFIG_SYS_NAND_BASE + NFC_MAIN_AREA(0);
	/* check for 32-bit alignment? */
	u32 *p = (u32 *) buf;
	u32 v = 0;

	for (; len > 0; len -= 4, main_buf += 4)
		v = NFC_READL(main_buf);
		if (v != *p++)
			return -1;
	return 0;
}

static int fsl_nfc_get_hw_config(struct nand_chip *this)
{
	immap_t *im = (immap_t *)CONFIG_SYS_IMMR;
	u32 rcwh;
	int rcwh_romloc;
	int rcwh_ps;
	int width;
	int writesize = 0;
	int sparesize = 0;

	/*
	 * Only support 2K for now.
	 * Remove this when others are tested and debugged.
	 */

	writesize=CONFIG_FSL_NFC_WRITE_SIZE;
	sparesize = CONFIG_FSL_NFC_SPARE_SIZE;
	width=1;

	priv->sparesize = sparesize;
	priv->writesize = writesize;
	priv->width = width;
	return 0;
}


#ifndef CONFIG_FSL_NFC_BOARD_CS_FUNC
#error failed
static void fsl_nfc_select_chip(u8 cs)
{
	
/*
	u32 val = NFC_READW(NFC_BUF_ADDR);

	val &= ~0x60;
	val |= cs << 5;
	NFC_WRITEW(NFC_BUF_ADDR, val);
*/	
}
#define CONFIG_FSL_NFC_BOARD_CS_FUNC fsl_nfc_select_chip
#endif


/*!
 * This function is used by upper layer for select and deselect of the NAND
 * chip
 *
 * @mtd		MTD structure for the NAND Flash
 * @chip	val indicating select or deselect
 */
static void fsl_nfc_select_chip(struct mtd_info *mtd, int chip)
{
	/*
	 * This is different than the linux version.
	 * Switching between chips is done via
	 * board_nand_select_device.
	 *
	 * Only valid chip numbers here are
	 * 	0 select
	 * 	-1 deselect
	 */


	/*
	 * Turn on appropriate chip.
	 */
	 
	 nfc_write(NFC_ROW_ADDR, ((1<<chip)<<28)|((1<<chip)<<24));
	 

}



/* Do address cycle(s) */
static void mpc5125_nfc_addr_cycle(struct mtd_info *mtd, int column, int page)
{
//printf("column=0x%x,page=0x%x\n",column,page);

	if (column != -1) {
		nfc_set_field( NFC_COL_ADDR,
				COL_ADDR_MASK,
				COL_ADDR_SHIFT, column);
	}

	if (page != -1) {
		nfc_set_field( NFC_ROW_ADDR,
					ROW_ADDR_MASK,
					ROW_ADDR_SHIFT, page);
	}
	/* DMA Disable */
	nfc_clear( NFC_FLASH_CONFIG, CONFIG_DMA_REQ_MASK);
	/* PAGE_CNT = 1 */
	nfc_set_field(NFC_FLASH_CONFIG, CONFIG_PAGE_CNT_MASK,
			CONFIG_PAGE_CNT_SHIFT, 0x2);
}




static int fsl_nfc_wait(struct mtd_info *mtd, struct nand_chip *chip)
{
	return get_dev_status();
}

void board_nand_select_device(struct nand_chip *nand, int chip)
{
	if (chip >= CONFIG_FSL_NFC_CHIPS) {
		printf("FSL NFC: "
			"ERROR: Illegal chip select (chip = %d)\n", chip);
		return;
	}
	priv->chipsel = chip;
}
/**
 * nand_read_page_hwecc - [REPLACABLE] hardware ecc based page read function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	buffer to store read data
 *
 * Not for syndrome calculating ecc controllers which need a special oob layout
 */
static int mpc5125_nand_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip,
				uint8_t *buf)
{

	unsigned int stat;
	u8 ecc_bytes=0,i;
	u8 ecc_bytes_map[]={0,8,12,15,23,30,45,60};
	stat=nfc_read(NFC_FLASH_CONFIG);
	stat>>=17;
	stat&=0x7;
	ecc_bytes=ecc_bytes_map[stat];
	stat=nfc_read( MPC5125_NFC_ECC_STATUS_ADD+4);
	if(stat&0x80)
	{
		/*check the page is erased*/
		if(stat&0x3f)
		{
			mtd->ecc_stats.failed++;
			printk(KERN_WARNING "UnCorrectable RS-ECC Error\n");
		}

	}
	else if(stat&0x3f)
	{
		/*printk(KERN_WARNING "Correctable ECC %d\n",stat&0x3f);*/
		mtd->ecc_stats.corrected+=stat&0x3f;
	}
	fsl_nfc_read_buf	(mtd, buf, mtd->writesize);
	copy_from_spare(mtd,chip->oob_poi, mtd->oobsize);
	return 0;
}
/**
 * nand_write_page_hwecc - [REPLACABLE] hardware ecc based page write function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	data buffer
 */
static void mpc5125_nand_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip,
				  const uint8_t *buf)
{
	fsl_nfc_write_buf(mtd, buf, mtd->writesize);
	copy_to_spare(mtd,chip->oob_poi, mtd->oobsize);
}


int board_nand_init(struct nand_chip *nand)
{
	struct mtd_info *mtd;

	priv = malloc(sizeof(*priv));
	if (!priv) {
		printf("FSL NFC: failed to allocate priv structure\n");
		return -1;
	}
	memset(priv, 0, sizeof(*priv));

	mpc5125_cfg_iopad_init();
	if (fsl_nfc_get_hw_config(nand) < 0)
		return -1;
	
	mtd = &priv->mtd;
	mtd->priv = nand;

	/* 5 us command delay time */
	nand->chip_delay = 8;
	nand->page_shift =0;
	nand->chip_shift = 0;
	
	nand->dev_ready = fsl_nfc_dev_ready;
	nand->cmdfunc = mpc5125_nfc_command;
	nand->waitfunc = fsl_nfc_wait;
	nand->select_chip = fsl_nfc_select_chip;
	nand->options = NAND_USE_FLASH_BBT;
	if (priv->width == 2) {
		nand->options |= NAND_BUSWIDTH_16;
		nand->read_byte = fsl_nfc_read_byte16;
	}
	nand->read_byte = fsl_nfc_read_byte;
	nand->read_word = fsl_nfc_read_word;
	nand->write_buf = fsl_nfc_write_buf;
	nand->read_buf = fsl_nfc_read_buf;
	nand->verify_buf = fsl_nfc_verify_buf;
	nand->options = NAND_NO_AUTOINCR | NAND_USE_FLASH_BBT;
	nand->ecc.mode = NAND_ECC_HW;
	nand->ecclayout=&nand_hw_eccoob_4k;
	nand->ecc.size = 512;	/* RS-ECC is applied for both MAIN+SPARE not MAIN alone */
	nand->ecc.bytes = 9;	/* used for both main and spare area */
	nand->ecc.read_page = mpc5125_nand_read_page_hwecc;
	nand->ecc.write_page = mpc5125_nand_write_page_hwecc;

	nfc_clear(NFC_IRQ_STATUS, CMD_DONE_EN_MASK| IDLE_EN_MASK);
	
		/* SET SECTOR SIZE */
	nfc_set_field( NFC_FLASH_CONFIG,
			CONFIG_ECC_SRAM_ADDR_MASK,
			CONFIG_ECC_SRAM_ADDR_SHIFT, (MPC5125_NFC_ECC_STATUS_ADD>>3)&0x00001ff);

	nfc_write(NFC_SECTOR_SIZE, CONFIG_FSL_NFC_WRITE_SIZE/2 | CONFIG_FSL_NFC_SPARE_SIZE/2);
	nfc_set_field(NFC_FLASH_CONFIG,
			CONFIG_ECC_MODE_MASK,
			CONFIG_ECC_MODE_SHIFT, ECC_45_BYTE);
	nfc_set_field( NFC_FLASH_CONFIG,
			CONFIG_ADDR_AUTO_INCR_MASK,
			CONFIG_ADDR_AUTO_INCR_SHIFT, 0);

	nfc_set_field(NFC_FLASH_CONFIG,
			CONFIG_BUFNO_AUTO_INCR_MASK,
			CONFIG_BUFNO_AUTO_INCR_SHIFT, 1);

	nfc_set_field(NFC_FLASH_CONFIG,
			CONFIG_16BIT_MASK,
			CONFIG_16BIT_SHIFT, 0);
	/* SET FAST_FLASH = 1 */
	nfc_set_field(NFC_FLASH_CONFIG,
			CONFIG_FAST_FLASH_MASK,
			CONFIG_FAST_FLASH_SHIFT, 1);

	nfc_set_field(NFC_FLASH_CONFIG,
			CONFIG_BOOT_MODE_MASK,
			CONFIG_BOOT_MODE_SHIFT, 0);

	nfc_set_field(NFC_FLASH_CONFIG,
			CONFIG_ECC_SRAM_REQ_MASK,
			CONFIG_ECC_SRAM_REQ_SHIFT, 1);
	g_nfc_save.nfc_config=nfc_read(NFC_FLASH_CONFIG);
	g_nfc_save.nfc_sectsize=nfc_read(NFC_SECTOR_SIZE);
	g_nfc_save.nfc_status=nfc_read(NFC_IRQ_STATUS);
	return 0;
}



