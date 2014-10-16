/*
 * Copyright (C) 2014
 * Stefan Agner, Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __ASM_ARCH_VF610_DDR_H
#define __ASM_ARCH_VF610_DDR_H

void setup_iomux_ddr(void);
void ddr_phy_init(void);
void ddr_ctrl_init(int tref, int trfc, int col_diff, int row_diff);

#endif
