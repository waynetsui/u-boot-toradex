/*
 *  (C) Copyright 2010,2011
 *  NVIDIA Corporation <www.nvidia.com>
 * Portions Copyright (c) 2011 The Chromium OS Authors
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

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/sizes.h>

/* High-level configuration options */
#define TEGRA3_SYSMEM		"mem=384M@0M nvmem=128M@384M mem=512M@512M"
#define V_PROMPT		"Tegra3 # "

#include "tegra3-common.h"

#define CONFIG_TEGRA_BOARD_STRING	"NVIDIA Cardhu"

#define CONFIG_TEGRA_ENABLE_UARTA
#define CONFIG_SYS_NS16550_COM1		NV_PA_APB_UARTA_BASE

#define CONFIG_MACH_TYPE	MACH_TYPE_CARDHU

#define CONFIG_CONSOLE_MUX
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_STD_DEVICES_SETTINGS	"stdin=serial\0" \
					"stdout=serial\0" \
					"stderr=serial\0"

#define CONFIG_SYS_BOARD_ODMDATA	0x800d8011 /* lp1, 1GB */

/* default environment */
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SECT_SIZE    CONFIG_ENV_SIZE
#define CONFIG_ENV_OFFSET       (SZ_4M - CONFIG_ENV_SECT_SIZE)

#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_EXTRA_ENV_SETTINGS_COMMON \
	"board=cardhu\0" \

#endif /* __CONFIG_H */
