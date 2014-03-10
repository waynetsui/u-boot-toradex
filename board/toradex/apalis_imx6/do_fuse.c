/*
 * Copyright (C) 2014, Toradex AG
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Helpers for i.MX OTP fusing during module production
*/

#include <common.h>
#include <fuse.h>

static unsigned mfgr_fuse(void);
unsigned mfgr_fuse(void)
{
	unsigned val, val6;
	unsigned mac_hi, mac_lo;
	uchar mac_id[6];

	fuse_sense(0, 5, &val);
	printf("Fuse 0, 5: %8x\n", val);
	fuse_sense(0, 6, &val6);
	printf("Fuse 0, 6: %8x\n", val6);
	fuse_sense(4, 3, &val);
	printf("Fuse 4, 3: %8x\n", val);
	fuse_sense(4, 2, &val);
	printf("Fuse 4, 2: %8x\n", val);
	if(val6 & 0x10)
	{
		puts("BT_FUSE_SEL already fused, will do nothing\n");
		return 1;
	}

	if (!eth_getenv_enetaddr("ethaddr", mac_id))
	{
		puts("ethaddr not set or not valid in environment, will do nothing\n");
		return 1;
	}
	mac_hi = (mac_id[0] << 8) + mac_id[1];
	mac_lo = (mac_id[2] << 24) + (mac_id[3] << 16) + (mac_id[4] << 8) + mac_id[5];

	/* boot cfg */
	fuse_prog(0, 5, 0x00005062);
	/* BT_FUSE_SEL */
	fuse_prog(0, 6, 0x00000010);
	/* MAC addr */
	fuse_prog(4, 3, mac_hi);
	fuse_prog(4, 2, mac_lo);

	return 0;
}

int do_mfgr_fuse(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[])
{
	puts("Fusing...\n");
	if(!mfgr_fuse())
		puts("done.\n");
	else
		puts("failed.\n");
	return 0;
}

U_BOOT_CMD(
	mfgr_fuse, 1, 0, do_mfgr_fuse,
	"OTP fusing during module production\n"
	"requires the environment variable ethaddr to be set\n",
	""
);
