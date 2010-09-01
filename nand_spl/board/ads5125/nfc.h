/*
 *
 * (C) Copyright 2009
 * Martha Marx, Silicon Turnkey Express, mmarx@silicontkx.com
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

#define TWO_K	0x800
#define NUMPAGES 137 /* if u-boot grows .. extend this */

/* NAND Flash Controller Register Offsets */
#define NFC_BUF_MAIN1		0x0000
#define NFC_BUF_MAIN2		0x0200
#define NFC_BUF_MAIN3		0x0400
#define NFC_BUF_MAIN4		0x0600
#define NFC_BUF_MAIN5		0x0800
#define NFC_BUF_MAIN6		0x0A00
#define NFC_BUF_MAIN7		0x0C00
#define NFC_BUF_MAIN8		0x0E00
#define NFC_BUF_SPARE1		0x1000
#define NFC_BUF_SPARE2		0x1040
#define NFC_BUF_SPARE3		0x1080
#define NFC_BUF_SPARE4		0x10c0
#define NFC_BUF_SPARE5		0x1100
#define NFC_BUF_SPARE6		0x1140
#define NFC_BUF_SPARE7		0x1180
#define NFC_BUF_SPARE8		0x11c0

#define NFC_RAM_BUF_ADDR	0x1e04
#define NFC_FLASH_ADDR		0x1e06
#define NFC_FLASH_CMD		0x1e08
#define NFC_NFC_CFG		0x1e0a
#define NFC_ECC_STATUS1		0x1e0c
#define NFC_ECC_STATUS2		0x1e0e
#define NFC_SPAS		0x1e10
#define NFC_NF_WR_PROT		0x1e12
#define NFC_FL_WP_STAT		0x1e18
#define NFC_NF_CFG1		0x1e1a
#define NFC_NF_CFG2		0x1e1c
#define NFC_UNLOCKSTART_BLKADDR0	0x1E20
#define NFC_UNLOCKEND_BLKADDR0		0x1E22
#define NFC_UNLOCKSTART_BLKADDR1	0x1E24
#define NFC_UNLOCKEND_BLKADDR1		0x1E26
#define NFC_UNLOCKSTART_BLKADDR2	0x1E28
#define NFC_UNLOCKEND_BLKADDR2		0x1E2A
#define NFC_UNLOCKSTART_BLKADDR3	0x1E2C
#define NFC_UNLOCKEND_BLKADDR3		0x1E2E

#define RAM_BUFFER_ADDRESS_RBA_4 0x4
#define RAM_BUFFER_ADDRESS_RBA_3 0x3
#define NFC_BUFSIZE_1KB 0x0
#define NFC_BUFSIZE_2KB 0x1
#define NFC_CONFIGURATION_UNLOCKED 0x2
#define ECC_STATUS_RESULT_NO_ERR 0x0
#define ECC_STATUS_RESULT_1BIT_ERR 0x1
#define ECC_STATUS_RESULT_2BIT_ERR 0x2
#define NF_WR_PROT_UNLOCK 0x4
#define NAND_FLASH_CONFIG1_FORCE_CE (1 << 7)
#define NAND_FLASH_CONFIG1_RST (1 << 6)
#define NAND_FLASH_CONFIG1_BIG (1 << 5)
#define NAND_FLASH_CONFIG1_INT_MSK (1 << 4)
#define NAND_FLASH_CONFIG1_ECC_EN (1 << 3)
#define NAND_FLASH_CONFIG1_SP_EN (1 << 2)
#define NAND_FLASH_CONFIG2_INT_DONE (1 << 15)
#define NAND_FLASH_CONFIG2_FDO_PAGE (0 << 3)
#define NAND_FLASH_CONFIG2_FDO_ID (2 << 3)
#define NAND_FLASH_CONFIG2_FDO_STATUS (4 << 3)
#define NAND_FLASH_CONFIG2_FDI_EN (1 << 2)
#define NAND_FLASH_CONFIG2_FADD_EN (1 << 1)
#define NAND_FLASH_CONFIG2_FCMD_EN (1 << 0)
#define FDO_PAGE_SPARE_VAL 0x8

/* Bit Definitions */
#define NFC_INT (1 << 15)
#define NFC_SP_EN (1 << 2)
#define NFC_ECC_EN (1 << 3)
#define NFC_INT_MSK (1 << 4)
#define NFC_BIG (1 << 5)
#define NFC_RST (1 << 6)
#define NFC_CE (1 << 7)
#define NFC_ONE_CYCLE (1 << 8)
#define NFC_BLS_LOCKED 0
#define NFC_BLS_LOCKED_DEFAULT 1
#define NFC_BLS_UNLOCKED 2
#define NFC_WPC_LOCK_TIGHT 1
#define NFC_WPC_LOCK (1 << 1)
#define NFC_WPC_UNLOCK (1 << 2)
#define NFC_FLASH_ADDR_SHIFT 0
#define NFC_UNLOCK_END_ADDR_SHIFT 0



#define NAND_CMD_RESET 0xff
#define NFC_ECC_MODE_4 1
#define NFC_CMD 0x1
#define NFC_ADDR 0x2
#define NFC_INPUT 0x4
#define NFC_OUTPUT 0x8
#define NFC_ID 0x10
#define NFC_STATUS 0x20
#define NAND_CMD_READ0 0
#define NAND_CMD_READSTART 0x30
#define NAND_CMD_READCACHE 0x31
#define NAND_CMD_READCACHEND 0x34

