/*
 *  (C) Copyright 2010,2011
 *  NVIDIA Corporation <www.nvidia.com>
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

#ifndef _CLK_RST_H_
#define _CLK_RST_H_


/* PLL registers - there are several PLLs in the clock controller */
struct clk_pll {
	uint pll_base;		/* the control register */
	uint pll_out;		/* output control */
	uint reserved;
	uint pll_misc;		/* other misc things */
};

/* PLL registers - there are several PLLs in the clock controller */
struct clk_pll_simple {
	uint pll_base;		/* the control register */
	uint pll_misc;		/* other misc things */
};

/*
 * Most PLLs use the clk_pll structure, but some have a simpler two-member
 * structure for which we use clk_pll_simple. The reason for this non-
 * othogonal setup is not stated.
 */
enum {
	TEGRA_CLK_PLLS		= 6,	/* Number of normal PLLs */
	TEGRA_CLK_SIMPLE_PLLS	= 3,	/* Number of simple PLLs */
	TEGRA_CLK_REGS		= 3,	/* Number of clock enable registers */
	TEGRA_CLK_SOURCES	= 64,	/* Number of periph clock sources */
};

/* Clock/Reset Controller (CLK_RST_CONTROLLER_) regs */
struct clk_rst_ctlr {
	uint crc_rst_src;			/* _RST_SOURCE_0,0x00 */
	uint crc_rst_dev[TEGRA_CLK_REGS];	/* _RST_DEVICES_L/H/U_0 */
	uint crc_clk_out_enb[TEGRA_CLK_REGS];	/* _CLK_OUT_ENB_L/H/U_0 */
	uint crc_reserved0;		/* reserved_0,		0x1C */
	uint crc_cclk_brst_pol;		/* _CCLK_BURST_POLICY_0,0x20 */
	uint crc_super_cclk_div;	/* _SUPER_CCLK_DIVIDER_0,0x24 */
	uint crc_sclk_brst_pol;		/* _SCLK_BURST_POLICY_0, 0x28 */
	uint crc_super_sclk_div;	/* _SUPER_SCLK_DIVIDER_0,0x2C */
	uint crc_clk_sys_rate;		/* _CLK_SYSTEM_RATE_0,	0x30 */
	uint crc_prog_dly_clk;		/* _PROG_DLY_CLK_0,	0x34 */
	uint crc_aud_sync_clk_rate;	/* _AUDIO_SYNC_CLK_RATE_0,0x38 */
	uint crc_reserved1;		/* reserved_1,		0x3C */
	uint crc_cop_clk_skip_plcy;	/* _COP_CLK_SKIP_POLICY_0,0x40 */
	uint crc_clk_mask_arm;		/* _CLK_MASK_ARM_0,	0x44 */
	uint crc_misc_clk_enb;		/* _MISC_CLK_ENB_0,	0x48 */
	uint crc_clk_cpu_cmplx;		/* _CLK_CPU_CMPLX_0,	0x4C */
	uint crc_osc_ctrl;		/* _OSC_CTRL_0,		0x50 */
	uint crc_pll_lfsr;		/* _PLL_LFSR_0,		0x54 */
	uint crc_osc_freq_det;		/* _OSC_FREQ_DET_0,	0x58 */
	uint crc_osc_freq_det_stat;	/* _OSC_FREQ_DET_STATUS_0,0x5C */
	uint crc_reserved2[8];		/* reserved_2[8],	0x60-7C */

	struct clk_pll crc_pll[TEGRA_CLK_PLLS];	/* PLLs from 0x80 to 0xdc */

	/* PLLs from 0xe0 to 0xf4    */
	struct clk_pll_simple crc_pll_simple[TEGRA_CLK_SIMPLE_PLLS];

	uint crc_reserved10;		/* _reserved_10,	0xF8 */
	uint crc_reserved11;		/* _reserved_11,	0xFC */

	uint crc_clk_src[TEGRA_CLK_SOURCES]; /*_I2S1_0...	0x100-1fc */
	uint crc_reserved20[80];	/*			0x200-33C */
	uint crc_cpu_cmplx_set;		/* _CPU_CMPLX_SET_0,	0x340	  */
	uint crc_cpu_cmplx_clr;		/* _CPU_CMPLX_CLR_0,	0x344     */

	/* T30 things */
	uint crc_clk_cpu_cmplx_set;	/* _CLK_CPU_CMPLX_SET_0, 0x348	  */
	uint crc_clk_cpu_cmplx_clr;	/* _CLK_CPU_CMPLX_CLR_0, 0x34c    */
	uint crc_reserved12[2];		/* reserved_12		0x350-354 */
	uint crc_rst_devices_v;		/* _RST_DEVICES_V	0x358	  */
	uint crc_rst_devices_w;		/* _RST_DEVICES_W	0x35c	  */
	uint crc_clk_out_enb_v;		/* _CLK_OUT_ENB_V	0x360	  */
	uint crc_clk_out_enb_w;		/* _CLK_OUT_ENB_W	0x364	  */
	uint crc_cclkg_burst_policy;	/* _CCLKG_BURST_POLICY	0x368	  */
	uint crc_super_cclkg_divider;	/* _SUPER_CCLKG_DIVIDER	0x36c	  */
	uint crc_cclklp_burst_policy;	/* _CCLKLP_BURST_POLICY	0x370	  */
	uint crc_super_cclklp_divider;	/* _SUPER_CCLKLP_DIVIDER 0x374	  */
	uint crc_clk_cpug_cmplx;	/* _CLK_CPUG_CMPLX	0x378	  */
	uint crc_clk_cpulp_cmplx;	/* _CLK_CPULP_CMPLX	0x37c	  */
	uint crc_cpu_softrst_ctrl;	/* _CPU_SOFTRST_CTRL	0x380	  */
	uint crc_reserved13[11];	/* reserved_13		0x384-3ac */
	uint crc_clk_source_g3d2;	/* _CLK_SOURCE_G3D2	0x3b0	  */
	uint crc_clk_source_mselect;	/* _CLK_SOURCE_MSELECT	0x3b4	  */

	/* the road goes ever on and on... */
};

/* CLK_RST_CONTROLLER_CLK_CPU_CMPLX_0 */
#define CPU1_CLK_STP_RANGE	9:9
#define CPU0_CLK_STP_RANGE	8:8

/* CLK_RST_CONTROLLER_PLLx_BASE_0 */
#define PLL_BYPASS_RANGE	31:31
#define PLL_ENABLE_RANGE	30:30
#define PLL_BASE_OVRRIDE_RANGE	28:28
#define PLL_DIVP_RANGE		22:20
#define PLL_DIVN_RANGE		17:8
#define PLL_DIVM_RANGE		4:0

/* CLK_RST_CONTROLLER_PLLx_MISC_0 */
#define PLL_CPCON_RANGE		11:8
#define PLL_LFCON_RANGE		7:4
#define PLLU_VCO_FREQ_RANGE	20:20
#define PLL_VCO_FREQ_RANGE	3:0

/* CLK_RST_CONTROLLER_OSC_CTRL_0 */
#define OSC_FREQ_RANGE		31:30

/* CLK_RST_CONTROLLER_CLK_SOURCE_x_OUT_0 */
#define OUT_CLK_DIVISOR_RANGE	7:0
#define OUT_CLK_SOURCE_RANGE	31:30
#define OUT_CLK_SOURCE4_RANGE	31:28

/* CLK_RST_CONTROLLER_CLK_SOURCE_MSELECT */
#define MSELECT_CLK_M_SHIFT	30
#define MSELECT_CLK_M_MASK	(3U << MSELECT_CLK_M_SHIFT)

/* CLK_RST_CONTROLLER_SCLK_BURST_POLICY */
#define SCLK_SYS_STATE_SHIFT	28U
#define SCLK_SYS_STATE_MASK	(15U << SCLK_SYS_STATE_SHIFT)
enum {
	SCLK_SYS_STATE_STDBY,
	SCLK_SYS_STATE_IDLE,
	SCLK_SYS_STATE_RUN,
	SCLK_SYS_STATE_IRQ = 4U,
	SCLK_SYS_STATE_FIQ = 8U,
};
#define SCLK_COP_FIQ_MASK	(1 << 27)
#define SCLK_CPU_FIQ_MASK	(1 << 26)
#define SCLK_COP_IRQ_MASK	(1 << 25)
#define SCLK_CPU_IRQ_MASK	(1 << 24)
#define SCLK_SWAKEUP_FIQ_SOURCE_SHIFT		12
#define SCLK_SWAKEUP_FIQ_SOURCE_MASK		\
		(7 << SCLK_SWAKEUP_FIQ_SOURCE_SHIFT)
#define SCLK_SWAKEUP_IRQ_SOURCE_SHIFT		8
#define SCLK_SWAKEUP_IRQ_SOURCE_MASK		\
		(7 << SCLK_SWAKEUP_FIQ_SOURCE_SHIFT)
#define SCLK_SWAKEUP_RUN_SOURCE_SHIFT		4
#define SCLK_SWAKEUP_RUN_SOURCE_MASK		\
		(7 << SCLK_SWAKEUP_FIQ_SOURCE_SHIFT)
#define SCLK_SWAKEUP_IDLE_SOURCE_SHIFT		0
#define SCLK_SWAKEUP_IDLE_SOURCE_MASK		\
		(7 << SCLK_SWAKEUP_FIQ_SOURCE_SHIFT)
enum {
	SCLK_SOURCE_CLKM,
	SCLK_SOURCE_PLLC_OUT1,
	SCLK_SOURCE_PLLP_OUT4,
	SCLK_SOURCE_PLLP_OUT3,
	SCLK_SOURCE_PLLP_OUT2,
	SCLK_SOURCE_CLKD,
	SCLK_SOURCE_CLKS,
	SCLK_SOURCE_PLLM_OUT1,
};
#define SCLK_SWAKE_FIQ_SRC_PLLM_OUT1	(7 << 12)
#define SCLK_SWAKE_IRQ_SRC_PLLM_OUT1	(7 << 8)
#define SCLK_SWAKE_RUN_SRC_PLLM_OUT1	(7 << 4)
#define SCLK_SWAKE_IDLE_SRC_PLLM_OUT1	(7 << 0)

/* CLK_RST_CONTROLLER_SUPER_SCLK_DIVIDER */
#define SUPER_SCLK_ENB_SHIFT		31U
#define SUPER_SCLK_ENB_MASK		(1U << 31)
#define SUPER_SCLK_DIVIDEND_SHIFT	8
#define SUPER_SCLK_DIVIDEND_MASK	(0xff << SUPER_SCLK_DIVIDEND_SHIFT)
#define SUPER_SCLK_DIVISOR_SHIFT	0
#define SUPER_SCLK_DIVISOR_MASK		(0xff << SUPER_SCLK_DIVISOR_SHIFT)


/* CLK_RST_CONTROLLER_CLK_SYSTEM_RATE */
#define CLK_SYS_RATE_HCLK_DISABLE_SHIFT	7
#define CLK_SYS_RATE_HCLK_DISABLE_MASK	(1 << CLK_SYS_RATE_HCLK_DISABLE_SHIFT)
#define CLK_SYS_RATE_AHB_RATE_SHIFT	4
#define CLK_SYS_RATE_AHB_RATE_MASK	(3 << CLK_SYS_RATE_AHB_RATE_SHIFT)
#define CLK_SYS_RATE_PCLK_DISABLE_SHIFT	3
#define CLK_SYS_RATE_PCLK_DISABLE_MASK	(1 << CLK_SYS_RATE_PCLK_DISABLE_SHIFT)
#define CLK_SYS_RATE_APB_RATE_SHIFT	0
#define CLK_SYS_RATE_APB_RATE_MASK	(3 << CLK_SYS_RATE_AHB_RATE_SHIFT)

#endif	/* CLK_RST_H */
