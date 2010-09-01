/*
 *
 * (C) Copyright 2009
 * Martha Marx, Silicon Turnkey Express, mmarx@silicontkx.com
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* these were originally in mpc512x.h */
#define MDDRC_BASE_OFFSET	0x09000
#define DDR_LAW_BAR		0x00a0
#define DDR_LAW_AR		0x00a4
#define IOCTL_BASE_ADDR		0x0a000

/* DRAM Controller Register Offsets */
#define DDR_SYS_CONFIG			0x00
#define DDR_TIME_CONFIG0		0x04
#define DDR_TIME_CONFIG1		0x08
#define DDR_TIME_CONFIG2		0x0C
#define DDR_COMMAND			0x10
#define DDR_COMPACT_COMMAND		0x14
#define DDR_SELF_REFRESH_CMD_0		0x18
#define DDR_SELF_REFRESH_CMD_1		0x1C
#define DDR_SELF_REFRESH_CMD_2		0x20
#define DDR_SELF_REFRESH_CMD_3		0x24
#define DDR_SELF_REFRESH_CMD_4		0x28
#define DDR_SELF_REFRESH_CMD_5		0x2C
#define DDR_SELF_REFRESH_CMD_6		0x30
#define DDR_SELF_REFRESH_CMD_7		0x34
#define DDR_DQS_CONFIG_COUNT		0x38
#define DDR_DQS_CONFIG_TIME		0x3C
#define DDR_DQS_DELAY_STATUS		0x40

/* DRAM Controller Priority Manager Register Offsets */
#define DRAMPRIOM_PRIOMAN_CONFIG1                        0x080
#define DRAMPRIOM_PRIOMAN_CONFIG2                        0x084
#define DRAMPRIOM_HIPRIO_CONFIG                          0x088
#define DRAMPRIOM_LUT_TABLE0_MAIN_UP                     0x08C
#define DRAMPRIOM_LUT_TABLE1_MAIN_UP                     0x090
#define DRAMPRIOM_LUT_TABLE2_MAIN_UP                     0x094
#define DRAMPRIOM_LUT_TABLE3_MAIN_UP                     0x098
#define DRAMPRIOM_LUT_TABLE4_MAIN_UP                     0x09C
#define DRAMPRIOM_LUT_TABLE0_MAIN_LOW                    0x0A0
#define DRAMPRIOM_LUT_TABLE1_MAIN_LOW                    0x0A4
#define DRAMPRIOM_LUT_TABLE2_MAIN_LOW                    0x0A8
#define DRAMPRIOM_LUT_TABLE3_MAIN_LOW                    0x0AC
#define DRAMPRIOM_LUT_TABLE4_MAIN_LOW                    0x0B0
#define DRAMPRIOM_LUT_TABLE0_ALT_UP                      0x0B4
#define DRAMPRIOM_LUT_TABLE1_ALT_UP                      0x0B8
#define DRAMPRIOM_LUT_TABLE2_ALT_UP                      0x0BC
#define DRAMPRIOM_LUT_TABLE3_ALT_UP                      0x0C0
#define DRAMPRIOM_LUT_TABLE4_ALT_UP                      0x0C4
#define DRAMPRIOM_LUT_TABLE0_ALT_LOW                     0x0C8
#define DRAMPRIOM_LUT_TABLE1_ALT_LOW                     0x0CC
#define DRAMPRIOM_LUT_TABLE2_ALT_LOW                     0x0D0
#define DRAMPRIOM_LUT_TABLE3_ALT_LOW                     0x0D4
#define DRAMPRIOM_LUT_TABLE4_ALT_LOW                     0x0D8
#define DRAMPRIOM_PERF_MONITOR_CONFIG                    0x0DC
#define DRAMPRIOM_EVENT_TIME_COUNTER                     0x0E0
#define DRAMPRIOM_EVENT_TIME_PRESET                      0x0E4
#define DRAMPRIOM_PERF_MNTR1_ADDR_LOW                    0x0E8
#define DRAMPRIOM_PERF_MNTR2_ADDR_LOW                    0x0EC
#define DRAMPRIOM_PERF_MNTR1_ADDR_HI                     0x0F0
#define DRAMPRIOM_PERF_MNTR2_ADDR_HI                     0x0F4
#define DRAMPRIOM_PERF_MNTR1_READ_CNTR                   0x100
#define DRAMPRIOM_PERF_MNTR2_READ_CNTR                   0x104
#define DRAMPRIOM_PERF_MNTR1_WRITE_CNTR                  0x108
#define DRAMPRIOM_PERF_MNTR2_WRITE_CNTR                  0x10C
#define DRAMPRIOM_GRANTED_ACK_CNTR0                      0x110
#define DRAMPRIOM_GRANTED_ACK_CNTR1                      0x114
#define DRAMPRIOM_GRANTED_ACK_CNTR2                      0x118
#define DRAMPRIOM_GRANTED_ACK_CNTR3                      0x11C
#define DRAMPRIOM_GRANTED_ACK_CNTR4                      0x120
#define DRAMPRIOM_CUMULATIVE_WAIT_CNTR0                  0x124
#define DRAMPRIOM_CUMULATIVE_WAIT_CNTR1                  0x128
#define DRAMPRIOM_CUMULATIVE_WAIT_CNTR2                  0x12C
#define DRAMPRIOM_CUMULATIVE_WAIT_CNTR3                  0x130
#define DRAMPRIOM_CUMULATIVE_WAIT_CNTR4                  0x134
#define DRAMPRIOM_SUMMED_PRIORITY_CNTR0                  0x138
#define DRAMPRIOM_SUMMED_PRIORITY_CNTR1                  0x13C
#define DRAMPRIOM_SUMMED_PRIORITY_CNTR2                  0x140
#define DRAMPRIOM_SUMMED_PRIORITY_CNTR3                  0x144
#define DRAMPRIOM_SUMMED_PRIORITY_CNTR4                  0x148


