 /*
 * Copyright (C) 2009 Freescale Semiconductor, Inc. All rights reserved.
 * Author: Jason.jin <jason.jin@freescale.com>
 *
 * Changelog:
 * July 2008, Intial version.
 * March 2009, Use the MMC framework's read/write function.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This file is only used for booting from SD card, During the
 * booting up process, the environments was reading into memory
 * together with the u-boot images. The environments locate
 * from the last 8K of the 512k special version u-boot image.
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
#include <mmc.h>

#ifdef CONFIG_ENV_IS_IN_SDCARD

#include <environment.h>

DECLARE_GLOBAL_DATA_PTR;

#define SD_BLK_SIZE	512

/* references to names in env_common.c */
extern uchar default_environment[];
extern int default_environment_size;

char *env_name_spec = "SD CARD";
env_t *env_ptr;

uchar env_get_char_spec(int index)
{
	return *((uchar *)(gd->env_addr + index));
}

int saveenv(void)
{
	int env_blknr, env_blkcnt;
	int code_offset, code_len, env_addr;
	uchar tmp_buf[512];
	int n, tmp_crc;
	struct mmc *mmc = find_mmc_device(0);

	mmc_init(mmc);

	/* read out the first block, get the config data information */
	env_blknr = 0;
	env_blkcnt = 1;
	n = mmc->block_dev.block_read(0, env_blknr, env_blkcnt, tmp_buf);
	if (n != env_blkcnt)
		return 1;

	/* Get the Source Address, from offset 0x50 */
	code_offset = *(unsigned long *)(tmp_buf + 0x50);

	/* Get the code size from offset 0x48, the size incude the env size 8K */
	code_len = *(unsigned long *)(tmp_buf + 0x48);
	env_addr = code_offset + code_len - CONFIG_ENV_SIZE;

	env_blknr = env_addr / SD_BLK_SIZE;
	env_blkcnt = CONFIG_ENV_SIZE / SD_BLK_SIZE;

	n = mmc->block_dev.block_write(0, env_blknr, env_blkcnt, env_ptr);
	if (n != env_blkcnt)
		return 1;

	puts("done\n");
	return 0;
}

void env_relocate_spec(void)
{
	/*Just copy to relocate place*/
	memcpy((u8 *)env_ptr, (u8 *)CONFIG_ENV_ADDR_IN_RAM, CONFIG_ENV_SIZE);

	if (crc32(0, env_ptr->data, ENV_SIZE) != env_ptr->crc) {
		puts("*** Warning - bad CRC, using default environment\n\n");

		if (default_environment_size > CONFIG_ENV_SIZE) {
			gd->env_valid = 0;
			puts("*** Error - default environment is too large\n\n");
			return;
		}

		memset(env_ptr, 0, sizeof(env_t));
		memcpy(env_ptr->data, default_environment, default_environment_size);
		env_ptr->crc = crc32(0, env_ptr->data, ENV_SIZE);
	}
	gd->env_valid = 1;
}

int env_init(void)
{
	/* The ROM code has copied the u-boot and the env to RAM
	 * But we need to know if there're env settings there by crc check
	 */

	env_t *env_ptr_tmp = (env_t *)CONFIG_ENV_ADDR_IN_RAM;
	if (crc32(0, env_ptr_tmp->data, ENV_SIZE) == env_ptr_tmp->crc) {
		gd->env_addr  = (ulong)&(env_ptr_tmp->data);
		gd->env_valid = 1;
		return 0;
	}

	gd->env_addr  = (ulong)&default_environment[0];
	gd->env_valid = 0;
	return 0;
}

#endif /* CONFIG_ENV_IS_IN_SDCARD */
