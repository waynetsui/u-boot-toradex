/*
 * (C) Copyright 2008 DENX Software Engineering
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
 *
 */
#undef DEBUG
#include <common.h>
#include <mpc512x.h>
#include <asm/bitops.h>
#include <asm/io.h>
#include <command.h>
#include <asm/processor.h>
#include <fdt_support.h>
#ifdef CONFIG_MISC_INIT_R
#include <i2c.h>
#endif

/* PSC Clocks in use */
#ifdef CONFIG_PSC_CONSOLE2
#define CLOCK_SCCR1_PSC_EN_BITS ( 				\
	    CLOCK_SCCR1_PSC_EN(CONFIG_PSC_CONSOLE) |		\
	    CLOCK_SCCR1_PSC_EN(CONFIG_PSC_CONSOLE2)		\
	)
#else
#define CLOCK_SCCR1_PSC_EN_BITS CLOCK_SCCR1_PSC_EN(CONFIG_PSC_CONSOLE)
#endif

/* Clocks in use */
#define SCCR1_CLOCKS_EN	(CLOCK_SCCR1_CFG_EN |		\
			 CLOCK_SCCR1_LPC_EN |		\
			 CLOCK_SCCR1_NFC_EN |				\
			 CLOCK_SCCR1_PSC_EN_BITS |	\
			 CLOCK_SCCR1_PSCFIFO_EN |	\
			 CLOCK_SCCR1_DDR_EN |		\
			 CLOCK_SCCR1_FEC1_EN |		\
			 CLOCK_SCCR1_TPR_EN)
#define SCCR1_CFG1_CLOCKS_EN (CLOCK_SCCR1_FEC2_EN)


#define SCCR2_CLOCKS_EN	(CLOCK_SCCR2_MEM_EN |		\
			 CLOCK_SCCR2_I2C_EN |		\
			 CLOCK_SCCR2_DIU_EN |		\
			 CLOCK_SCCR2_SDHC1_EN)

#define SCCR2_CFG0_CLOCKS_EN (CLOCK_SCCR2_USB1_EN |	\
			      CLOCK_SCCR2_DIU_EN)

#define SCCR2_CFG1_CLOCKS_EN (CLOCK_SCCR2_USB1_EN |	\
			      CLOCK_SCCR2_USB2_EN)

#define CSAW_START(start)	((start) & 0xFFFF0000)
#define CSAW_STOP(start, size)	(((start) + (size) - 1) >> 16)

long int fixed_sdram(void);

static  iopin_t ioregs_common_init[] = {

	/* FUNC3=LPC_CS2 Sets Next 3 to LPC pads - done in start.S since
	 it's CPLD and catch22 here...must read what CFG switch is set
	{
		IO_CTRL_LPC_AX03, 3, 0,
		IO_PIN_FMUX(2) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	} */
	/* FUNC1=PSC9_3 Sets Next 2 to PSC9 pads x4f & x50*/
	{
		IO_CTRL_I2C1_SCL, 2, 0,
		IO_PIN_FMUX(1) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC3=GPIO14 Sets 1 pad */
	{
		IO_CTRL_PSC_MCLK_IN, 1, 0,
		IO_PIN_FMUX(3) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC3=IRQ0 Sets 1 pad */
	{
		IO_CTRL_PSC1_1, 1, 0,
		IO_PIN_FMUX(3) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC2=CKSTP_OUT Sets 1 pad */
	{
		IO_CTRL_PSC1_4, 1, 0,
		IO_PIN_FMUX(3) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC3=I2C1_SCL Sets Next 2 to I2C1 pads */
	{
		IO_CTRL_J1850_TX, 2, 0,
		IO_PIN_FMUX(3) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* ORs all pads to highest slew rate*/
	{
		IO_CTRL_LPC_CLK, IO_CTRL_PSC1_4 - IO_CTRL_LPC_CLK +1, 1,
		IO_PIN_FMUX(0) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	}
};
static  iopin_t ioregs_cfg0_init[] = {
	/* FUNC2=DIU_LD00 Sets Next 2 to DIU pads */
	{
		IO_CTRL_DIU_LD00, 2, 0,
		IO_PIN_FMUX(2) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(0)
	},
	/* FUNC2=DIU_LD08 Sets Next 2 to DIU pads */
	{
		IO_CTRL_DIU_LD08, 2, 0,
		IO_PIN_FMUX(2) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(0)
	},
	/* FUNC2=DIU_LD16 Sets Next 2 to DIU pads */
	{
		IO_CTRL_DIU_LD16, 2, 0,
		IO_PIN_FMUX(2) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(0)
	},
	/* FUNC2=FEC2_RXD_1 Sets Next 12 to FEC2 pads */
	{
		IO_CTRL_USB1_DATA0, 12, 0,
		IO_PIN_FMUX(2) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(0)
	}
};

static  iopin_t ioregs_cfg1_init[] = {
	/* FUNC2=USB1_DATA0 Sets Next 4 to USB1 pads */
	{
		IO_CTRL_DIU_CLK, 4, 0,
		IO_PIN_FMUX(2) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC3=GPIO32 Sets 1 (2 ??) pad */
	{
		IO_CTRL_DIU_LD00, 2, 0,
		IO_PIN_FMUX(3) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC2=USB1_DATA4 Sets Next 6 to USB1 pads */
	{
		IO_CTRL_DIU_LD02, 6, 0,
		IO_PIN_FMUX(2) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC3=GPIO37 Sets 2 to GPIO pads */
	{
		IO_CTRL_DIU_LD08, 2, 0,
		IO_PIN_FMUX(3) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC2=USB1_NEXT Sets Next 6 to USB1 and USB2 pads */
	{
		IO_CTRL_DIU_LD10, 6, 0,
		IO_PIN_FMUX(2) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC1=I2C3_SCL Sets Next 2 to I2C3 pads */
	{
		IO_CTRL_DIU_LD16, 8, 0,
		IO_PIN_FMUX(1) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC2=USB2_DATA4 Sets Next 8 to USB2 pads */
	{
		IO_CTRL_DIU_LD18, 8, 0,
		IO_PIN_FMUX(2) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC2=FEC2_RXD_1 Sets Next 12 to FEC2 pads */
	{
		IO_CTRL_USB1_DATA0, 12, 0,
		IO_PIN_FMUX(2) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	}
	/* FUNC3=GPIO10 Sets 1 pad -- COL unused in RMII??
	{
		IO_CTRL_USB1_DIR, 1, 0,
		IO_PIN_FMUX(3) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	} */
};
int board_early_init_f (void)
{
	immap_t *im = (immap_t *) CONFIG_SYS_IMMR;
	u32 lpcaw;


	/*
	 * Initialize Local Window for the CPLD registers access (CS2 selects
	 * the CPLD chip)
	 */
/*	 
	out_be32(&im->sysconf.lpcs2aw, CSAW_START(CONFIG_SYS_CPLD_BASE) |
			      CSAW_STOP(CONFIG_SYS_CPLD_BASE, CONFIG_SYS_CPLD_SIZE));
	out_be32(&im->lpc.cs_cfg[2], CONFIG_SYS_CS2_CFG);
*/

	/*
	 * According to MPC5121e RM, configuring local access windows should
	 * be followed by a dummy read of the config register that was
	 * modified last and an isync
	 */
	lpcaw = in_be32(&im->sysconf.lpcs2aw);
	__asm__ __volatile__ ("isync");

	/*
	 * Disable Boot NOR FLASH write protect - CPLD Reg 8 NOR FLASH Control
	 *
	 * Without this the flash identification routine fails, as it needs to issue
	 * write commands in order to establish the device ID.
	 */
#if (BOARD_TYPE==BOARD_TYPE_ADS5125)
	if (in_8((u8 *)(CONFIG_SYS_CPLD_BASE + 0x08)) & 0x04) {
		out_8((u8 *)(CONFIG_SYS_CPLD_BASE + 0x08), 0xC1);
	} else {
		/* running from Backup flash */
		out_8((u8 *)(CONFIG_SYS_CPLD_BASE + 0x08), 0x32);
	}
#endif
	/*
	 * Configure Flash Speed
	 */
#if (BOARD_TYPE==BOARD_TYPE_ADS5125)
	out_be32(&im->lpc.cs_cfg[0], CONFIG_SYS_CS0_CFG);
	out_be32((u32 *)(CONFIG_SYS_IMMR + LPC_OFFSET + CS_ALE_TIMING_CONFIG),CONFIG_SYS_CS_ALETIMING);
#endif
	/*
	 * Enable clocks
	 */
#if (BOARD_TYPE==BOARD_TYPE_ADS5125)
	if (IS_CFG1_SWITCH) {
		out_be32(&im->clk.sccr[0], SCCR1_CLOCKS_EN | SCCR1_CFG1_CLOCKS_EN);
		out_be32(&im->clk.sccr[1], SCCR2_CLOCKS_EN | SCCR2_CFG1_CLOCKS_EN);
	} else {
		out_be32(&im->clk.sccr[0], SCCR1_CLOCKS_EN);
		out_be32(&im->clk.sccr[1], SCCR2_CLOCKS_EN | SCCR2_CFG0_CLOCKS_EN);
	}
#else
	out_be32(&im->clk.sccr[0], SCCR1_CLOCKS_EN | SCCR1_CFG1_CLOCKS_EN);
	out_be32(&im->clk.sccr[1], SCCR2_CLOCKS_EN | SCCR2_CFG1_CLOCKS_EN);	
#endif
	/* initialize function mux & slew rate IO inter alia on IO Pins  */
	/* there are two peripheral options controlled by switch 8 */
#if (BOARD_TYPE==BOARD_TYPE_ADS5125)
        if (IS_CFG1_SWITCH)
		iopin_initialize(ioregs_cfg1_init,
			sizeof(ioregs_cfg1_init) / sizeof(ioregs_cfg1_init[0]));
	else
#endif
		iopin_initialize(ioregs_cfg0_init,
			sizeof(ioregs_cfg0_init) / sizeof(ioregs_cfg0_init[0]));

	iopin_initialize(ioregs_common_init,
			sizeof(ioregs_common_init) / sizeof(ioregs_common_init[0]));

	/* enable default pins */
	out_8(&im->io_ctrl.regs[IO_CTRL_GBOBE], IOCTRL_GBOBE_ON);
	/*enable nfc_ce2 Cloudy*/
	out_8(&im->io_ctrl.regs[IO_CTRL_LPC_ACK_B], 0x3b);
	return 0;
}

phys_size_t initdram (int board_type)
{
	u32 msize = 0;

	msize = fixed_sdram ();

	return msize;
}

/*
 * fixed sdram init -- the board doesn't use memory modules that have serial presence
 * detect or similar mechanism for discovery of the DRAM settings
 */
long int fixed_sdram (void)
{
	immap_t *im = (immap_t *) CONFIG_SYS_IMMR;
	u32 msize = CONFIG_SYS_DDR_SIZE * 1024 * 1024;
#if ( BOARD_TYPE!=BOARD_TYPE_5125_MPU)
	u32 msize_log2 = __ilog2 (msize);
	u32 i;

	/* Initialize IO Control */
	out_8(&im->io_ctrl.regs[IO_CTRL_MEM], IOCTRL_MUX_DDR);

	/* Initialize DDR Local Window */
	out_be32(&im->sysconf.ddrlaw.bar, CONFIG_SYS_DDR_BASE & 0xFFFFF000);
	out_be32(&im->sysconf.ddrlaw.ar, msize_log2 - 1);

	/*
	 * According to MPC5121e RM, configuring local access windows should
	 * be followed by a dummy read of the config register that was
	 * modified last and an isync
	 */
	i = im->sysconf.ddrlaw.ar;
	__asm__ __volatile__ ("isync");

	/* Enable DDR */
	out_be32(&im->mddrc.ddr_sys_config, MDDRC_SYS_CFG_EN);

	/* Initialize DDR Priority Manager */
	out_be32(&im->mddrc.prioman_config1, MDDRCGRP_PM_CFG1);
	out_be32(&im->mddrc.prioman_config2, MDDRCGRP_PM_CFG2);
	out_be32(&im->mddrc.hiprio_config, MDDRCGRP_HIPRIO_CFG);
	out_be32(&im->mddrc.lut_table0_main_upper, MDDRCGRP_LUT0_MU);
	out_be32(&im->mddrc.lut_table0_main_lower, MDDRCGRP_LUT0_ML);
	out_be32(&im->mddrc.lut_table1_main_upper, MDDRCGRP_LUT1_MU);
	out_be32(&im->mddrc.lut_table1_main_lower, MDDRCGRP_LUT1_ML);
	out_be32(&im->mddrc.lut_table2_main_upper, MDDRCGRP_LUT2_MU);
	out_be32(&im->mddrc.lut_table2_main_lower, MDDRCGRP_LUT2_ML);
	out_be32(&im->mddrc.lut_table3_main_upper, MDDRCGRP_LUT3_MU);
	out_be32(&im->mddrc.lut_table3_main_lower, MDDRCGRP_LUT3_ML);
	out_be32(&im->mddrc.lut_table4_main_upper, MDDRCGRP_LUT4_MU);
	out_be32(&im->mddrc.lut_table4_main_lower, MDDRCGRP_LUT4_ML);
	out_be32(&im->mddrc.lut_table0_alternate_upper, MDDRCGRP_LUT0_AU);
	out_be32(&im->mddrc.lut_table0_alternate_lower, MDDRCGRP_LUT0_AL);
	out_be32(&im->mddrc.lut_table1_alternate_upper, MDDRCGRP_LUT1_AU);
	out_be32(&im->mddrc.lut_table1_alternate_lower, MDDRCGRP_LUT1_AL);
	out_be32(&im->mddrc.lut_table2_alternate_upper, MDDRCGRP_LUT2_AU);
	out_be32(&im->mddrc.lut_table2_alternate_lower, MDDRCGRP_LUT2_AL);
	out_be32(&im->mddrc.lut_table3_alternate_upper, MDDRCGRP_LUT3_AU);
	out_be32(&im->mddrc.lut_table3_alternate_lower, MDDRCGRP_LUT3_AL);
	out_be32(&im->mddrc.lut_table4_alternate_upper, MDDRCGRP_LUT4_AU);
	out_be32(&im->mddrc.lut_table4_alternate_lower, MDDRCGRP_LUT4_AL);

	/* Initialize MDDRC */
	out_be32(&im->mddrc.ddr_time_config0, MDDRC_SYS_CFG);
	out_be32(&im->mddrc.ddr_time_config0, MDDRC_TIME_CFG0);
	out_be32(&im->mddrc.ddr_time_config1, MDDRC_TIME_CFG1);
	out_be32(&im->mddrc.ddr_time_config2, MDDRC_TIME_CFG2);
	/* Initialize DDR */
	for (i = 0; i < 10; i++)
		out_be32(&im->mddrc.ddr_command,DDR_NOP);
	udelay(1);
	out_be32(&im->mddrc.ddr_command,DDR_PCHG_ALL);
	out_be32(&im->mddrc.ddr_command,DDR_NOP);
	out_be32(&im->mddrc.ddr_command,DDR_NOP);
	out_be32(&im->mddrc.ddr_command,DDR_NOP);
	out_be32(&im->mddrc.ddr_command,DDR_RFSH);
	out_be32(&im->mddrc.ddr_command,DDR_RFSH);
	out_be32(&im->mddrc.ddr_command,DDR_EM2);
	out_be32(&im->mddrc.ddr_command,DDR_EM3);
	out_be32(&im->mddrc.ddr_command,DDR_EN_DLL);
	out_be32(&im->mddrc.ddr_command,DDR_RES_DLL);
	out_be32(&im->mddrc.ddr_command,DDR_PCHG_ALL);
	out_be32(&im->mddrc.ddr_command,DDR_RFSH);
	out_be32(&im->mddrc.ddr_command,DDR_NOP);
	out_be32(&im->mddrc.ddr_command,DDR_INIT_DEV_OP);
	out_be32(&im->mddrc.ddr_command,DDR_PCHG_ALL);
	for (i = 0; i < 5; i++)
		out_be32(&im->mddrc.ddr_command,DDR_RFSH);
	out_be32(&im->mddrc.ddr_command,DDR_OCD_DEFAULT);
	out_be32(&im->mddrc.ddr_command,DDR_OCD_EXIT);
	for (i = 0; i < 5; i++)
		out_be32(&im->mddrc.ddr_command,DDR_NOP);
	udelay(10);
	/* Start MDDRC */
	out_be32(&im->mddrc.ddr_time_config0,MDDRC_TIME_CFG0_RUN);
	out_be32(&im->mddrc.ddr_sys_config, MDDRC_SYS_CFG_RUN);
#endif
	return msize;
}
#ifdef CONFIG_MISC_INIT_R
struct i2c_init_struct{
	u8 reg_addr;
	u8 data;
};
extern  void OnHdmiCableConnected(void);
int misc_init_r(void)
{
	u8 tmp_val;

	extern int ads5121_diu_init(void);
#if(BOARD_TYPE!=BOARD_TYPE_5125_MPU)
	if (IS_CFG1_SWITCH) /* no diu in CFG1 */
		return 0;

#if defined(CONFIG_HARD_I2C) || defined(CONFIG_SOFT_I2C)

        if (IS_CFG1_SWITCH){
	/* turn on ??? */
		i2c_set_bus_num(2);
	} else {
	/* Using this for DIU init before the driver in linux takes over
	 *  Enable the TFP410 Encoder (I2C address 0x38)
	 */

		i2c_set_bus_num(1);
		tmp_val = 0xBF;
		i2c_write(0x38, 0x08, 1, &tmp_val, sizeof(tmp_val));
		/* Verify if enabled */
		tmp_val = 0;
		i2c_read(0x38, 0x08, 1, &tmp_val, sizeof(tmp_val));
		debug("DVI Encoder Read: 0x%02lx\n", tmp_val);

		tmp_val = 0x10;
		i2c_write(0x38, 0x0A, 1, &tmp_val, sizeof(tmp_val));
		/* Verify if enabled */
		tmp_val = 0;
		i2c_read(0x38, 0x0A, 1, &tmp_val, sizeof(tmp_val));
		debug("DVI Encoder Read: 0x%02lx\n", tmp_val);
	}
#endif
#ifdef CONFIG_FSL_DIU_FB
#if	!(defined(CONFIG_VIDEO) || defined(CONFIG_CFB_CONSOLE))
	ads5121_diu_init();
#endif
#endif
#endif
#if(BOARD_TYPE == BOARD_TYPE_5125_MPU )
#if (HDMI_CHIP_SELECT==HDMI_CHIP_SIL9034)
	/*sil9034 enable */
	struct i2c_init_struct i2c_data[6]=
	{
		{0x08,0x36},
		{0x0c,0x03},
		{0x0f,0x04},
		{0x33,0x30},
		{0x34,0x00},
		{0x08,0x37},
	};
	for(tmp_val=0;tmp_val<sizeof(i2c_data)/sizeof(i2c_data[0]);tmp_val++)
	{
		if (i2c_write(0x39, i2c_data[tmp_val].reg_addr, 1, &(i2c_data[tmp_val].data), 1) != 0)
			printf("Error writing the chip 0x39 offset %d.\n",i2c_data[tmp_val].data);
	}
#elif (HDMI_CHIP_SELECT==HDMI_CHIP_SIL9022A)
	/*sil9022a enable DVI signal*/
	struct i2c_init_struct i2c_data[]=
	{
		{0xc7,0x00},
		{0x1a,0x10},
		{0x1e,0x00},
		{0x1a,0x00},
	};
	for(tmp_val=0;tmp_val<sizeof(i2c_data)/sizeof(i2c_data[0]);tmp_val++)
	{
		if (i2c_write(0x39, i2c_data[tmp_val].reg_addr, 1, &(i2c_data[tmp_val].data), 1) != 0)
			printf("HDMI,Error writing reg==0x%x.\n",i2c_data[tmp_val].reg_addr);
	}
#endif
#endif

	return 0;
}
#endif /* CONFIG_MISC_INIT_R */
int checkboard (void)
{
#if(BOARD_TYPE==BOARD_TYPE_ADS5125)
	ushort brd_rev = *(vu_short *) (CONFIG_SYS_CPLD_BASE + 0x00);
	uchar cpld_rev = *(vu_char *) (CONFIG_SYS_CPLD_BASE + 0x02);
	uchar cpld_rev_min = *(vu_char *) (CONFIG_SYS_CPLD_BASE + 0x03);

	if (cpld_rev_min != '\0')
		printf ("Board: ADS5125 rev. 0x%04x (CPLD rev. 0x%02x.%02x-BETA)\n",
		brd_rev, cpld_rev,cpld_rev_min);
	else
		printf ("Board: ADS5125 rev. 0x%04x (CPLD rev. 0x%02x)\n",
		brd_rev, cpld_rev);
	if (IS_CFG1_SWITCH) /* CAN1+2, SDHC1, USB1+2, FEC1+2, I2C1+2*/
		printf("Peripheral Option Set 1\n");
	else	/* CAN1+2, SDHC1, DIU, USB1, FEC1, I2C1+3*/
		printf("Peripheral Option Set 0\n");
#else
	printf("board:	mpc5125_mpu\n");
#endif
	return 0;
}

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
void ft_board_setup(void *blob, bd_t *bd)
{
	ft_cpu_setup(blob, bd);
	fdt_fixup_memory(blob, (u64)bd->bi_memstart, (u64)bd->bi_memsize);
}
#endif /* defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP) */


#if defined(CONFIG_NAND_FSL_NFC)
void ads5125_fsl_nfc_board_cs(int chip)
{
#if(BOARD_TYPE==BOARD_TYPE_ADS5125)
	unsigned char *csreg = (unsigned char *)(CONFIG_SYS_CPLD_BASE + 0x09);
	u8 v;
	printf("ads5125_fsl_nfc_board_cs chip=%d\n",chip);
	v = in_8(csreg);
	v |= 0xf;
	v &= ~(1<<chip);

	out_8(csreg, v);
#endif
}
#endif

