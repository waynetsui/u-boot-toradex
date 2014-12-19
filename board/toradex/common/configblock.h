/*
 * Copyright (c) 2011-2014 Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifdef CONFIG_TRDX_CFG_BLOCK
int read_trdx_cfg_block(void);
void get_board_serial_char(char *serialnr);
void get_board_product_number(unsigned short *prodnr);
#endif
