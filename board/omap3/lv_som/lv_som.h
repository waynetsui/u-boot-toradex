/*
 * (C) Copyright 2008
 * Texas Instruments
 * Nishanth Menon <nm@ti.com>
 *
 * Derived from: board/omap3/beagle/beagle.h
 * Dirk Behme <dirk.behme@gmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef _BOARD_LV_SOM_H_
#define _BOARD_LV_SOM_H_

const omap3_sysinfo sysinfo = {
	SDP_3430_V1,
	SDP_3430_V2,
	DDR_STACKED,
	"OMAP3 LV SOM",
	"NAND",
};

static void setup_net_chip(void);


#endif /* _BOARD_ZOOM_H_ */
