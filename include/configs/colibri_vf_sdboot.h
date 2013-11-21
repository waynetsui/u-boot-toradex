/*
 * Copyright 2013 Toradex, Inc.
 *
 * Configuration settings for the Colibri VF50 module booting from SD card.
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

/*
 * Use this configuration for an u-boot which stores its environment in SD/MMC
 * Useful if SD/MMC is selected as the boot device, so also the environment is taken from there
 *
 * Use colibri_vf61.h for everything which is not specific to using SD/MMC as the boot device!!
 */

#ifndef __CONFIG_SDBOOT_H
#define __CONFIG_SDBOOT_H

#include "colibri_vf.h"

#undef DEFAULT_BOOTCOMMAND
#undef CONFIG_BOOTCOMMAND

#define DEFAULT_BOOTCOMMAND \
	"run sdboot; run nfsboot"
#define CONFIG_BOOTCOMMAND	DEFAULT_BOOTCOMMAND


#ifdef CONFIG_ENV_IS_NOWHERE
#undef CONFIG_ENV_IS_NOWHERE
#endif
#ifdef CONFIG_ENV_IS_IN_NAND
#undef CONFIG_ENV_IS_IN_NAND
#undef CONFIG_ENV_OFFSET
#undef CONFIG_ENV_RANGE
#endif

/* Environment stored in SD/MMC */
#define CONFIG_ENV_IS_IN_MMC 1

#if defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_SYS_MMC_ENV_DEV 0 /* use MMC0, slot on eval board and Iris */
#define CONFIG_ENV_OFFSET (512 * 1024)
#endif

#endif /* __CONFIG_SDBOOT_H */
