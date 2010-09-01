/*
 * (C) Copyright 2008
 * Martha J Marx, Silicon Turnkey Express, mmarx@silicontkx.com
 * mpc512x I/O pin/pad initialization for the ADS5121 board
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

#include <common.h>
#include <linux/types.h>
#include <mpc512x.h>
#include <asm/io.h>

void iopin_initialize(iopin_t *ioregs_init, int len)
{
	short i, j, p;
	immap_t *im = (immap_t *)CONFIG_SYS_IMMR;
#ifdef CONFIG_ADS5125
	u_char *reg;
#else
	u_long *reg;
#endif

	reg = &im->io_ctrl.regs[0];
	if (sizeof(ioregs_init) == 0)
		return;

	for (i = 0; i < len; i++) {
#ifdef CONFIG_ADS5125
		for (p = 0, j = ioregs_init[i].p_offset;
			p < ioregs_init[i].nr_pins; p++, j++) {
			if (ioregs_init[i].bit_or)
				setbits(8, &(reg[j]), ioregs_init[i].val);
			else
				out_8(&reg[j], ioregs_init[i].val);
		}
#else
		for (p = 0, j = ioregs_init[i].p_offset / sizeof(u_long);
			p < ioregs_init[i].nr_pins; p++, j++) {
			if (ioregs_init[i].bit_or)
				setbits(be32, &(reg[j]), ioregs_init[i].val);
			else
				out_be32(&reg[j], ioregs_init[i].val);
		}
#endif
	}

	return;
}
