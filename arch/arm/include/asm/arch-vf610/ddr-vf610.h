/*
 * Copyright (C) 2014
 * Stefan Agner, Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __ASM_ARCH_VF610_DDR_H
#define __ASM_ARCH_VF610_DDR_H

struct ddr_lvl_info {
	u16 wrlvl_reg_en;
	u16 wrlvl_dl_0;
	u16 wrlvl_dl_1;
	u16 rdlvl_gt_reg_en;
	u16 rdlvl_gt_dl_0;
	u16 rdlvl_gt_dl_1;
	u16 rdlvl_reg_en;
	u16 rdlvl_dl_0;
	u16 rdlvl_dl_1;
};

void setup_iomux_ddr(void);
void ddr_phy_init(void);
void ddr_ctrl_init(int tref, int trfc, int col_diff, int row_diff,
		struct ddr_lvl_info *lvl);

#endif
