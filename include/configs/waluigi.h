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
#define TEGRA3_SYSMEM		"mem=1023M@2048M"
#define V_PROMPT		"Tegra3 # "

#define CONFIG_TEGRA3_WALUIGI
#define CONFIG_SYS_SKIP_ARM_RELOCATION
#define CONFIG_SYS_PLLP_BASE_IS_408MHZ

#include "tegra3-common.h"

#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_EXTRA_ENV_SETTINGS_COMMON \
	"board=waluigi\0" \

#define CONFIG_DEFAULT_DEVICE_TREE "tegra3-waluigi"

#define CONFIG_CONSOLE_MUX
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_STD_DEVICES_SETTINGS	"stdin=serial,tegra-kbc\0" \
					"stdout=serial,lcd\0" \
					"stderr=serial,lcd\0"

#define CONFIG_SYS_BOARD_ODMDATA	0x80080105 /* 2GB, ??? */

#define CONFIG_ENV_SECT_SIZE    CONFIG_ENV_SIZE
#define CONFIG_ENV_OFFSET       (SZ_4M - CONFIG_ENV_SECT_SIZE)

/* GPIO */
#define CONFIG_TEGRA_GPIO
#define CONFIG_CMD_TEGRA_GPIO_INFO

/* I2C */
#define CONFIG_TEGRA_I2C
#define CONFIG_SYS_I2C_INIT_BOARD
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_SYS_MAX_I2C_BUS		4	/* TODO: 5 in T30 */
#define CONFIG_SYS_I2C_SPEED		100000
#define CONFIG_CMD_I2C

/* PMU */
#define CONFIG_TEGRA_PMU

#define CONFIG_ENV_IS_NOWHERE

#endif /* __CONFIG_H */
