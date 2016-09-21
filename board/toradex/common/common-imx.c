/*
 * Copyright (c) 2016 Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/imx-regs.h>
#include <asm/io.h>
#include <common.h>

int should_load_env(void)
{
	/* Do not load environment from eMMC/NAND, it could be hostile */
	if (is_boot_from_usb()) {
		puts("Serial Downloader mode, not loading environment\n");
		return 0;
	}

	return 1;
}
