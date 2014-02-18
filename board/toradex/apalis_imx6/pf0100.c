/*
 * Copyright (C) 2014, Toradex AG
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Helpers for Freescale PMIC PF0100
*/

#include <common.h>
#include <i2c.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>

#include "pf0100_otp.inc"
#include "pf0100.h"

/* 7-bit I2C bus slave address */
#define PFUZE100_I2C_ADDR 		(0x08)

iomux_v3_cfg_t const pmic_prog_pads[] = {
	MX6_PAD_GPIO_2__GPIO_1_2 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

void pmic_init(void)
{
	uchar bus = 1;
	uchar devid, revid, val;

	puts("PMIC: ");
	if(!(0 == i2c_set_bus_num(bus) && (0 == i2c_probe(PFUZE100_I2C_ADDR))))
	{
		puts("i2c bus failed\n");
		return;
	}
	/* get device ident */
	if( i2c_read(PFUZE100_I2C_ADDR, PFUZE100_DEVICEID, 1, &devid, 1) < 0)
	{
		puts("i2c pmic devid read failed\n");
		return;
	}
	if( i2c_read(PFUZE100_I2C_ADDR, PFUZE100_REVID, 1, &revid, 1) < 0)
	{
		puts("i2c pmic revid read failed\n");
		return;
	}
	printf("device id: 0x%.2x, revision id: 0x%.2x\n", devid, revid);

	/* set VGEN1 to 1.2V */
	val = PFUZE100_VGEN1_VAL;
	if( i2c_write(PFUZE100_I2C_ADDR, PFUZE100_VGEN1CTL, 1, &val, 1))
	{
		puts("i2c write failed\n");
		return;
	}

	/* set SWBST to 5.0V */
	val = PFUZE100_SWBST_VAL;
	if( i2c_write(PFUZE100_I2C_ADDR, PFUZE100_SWBSTCTL, 1, &val, 1))
	{
		puts("i2c write failed\n");
		return;
	}
}

void pf0100_prog(void)
{
	unsigned char bus = 1;
	unsigned char val;
	unsigned i;

	/* set up gpio to manipulate vprog, initially off */
	imx_iomux_v3_setup_multiple_pads(pmic_prog_pads, ARRAY_SIZE(pmic_prog_pads));
	gpio_direction_output(IMX_GPIO_NR(1, 2), 0);

	if(!(0 == i2c_set_bus_num(bus) && (0 == i2c_probe(PFUZE100_I2C_ADDR))))
	{
		puts("i2c bus failed\n");
		return;
	}

	for (i=0; i<ARRAY_SIZE(pmic_otp_prog); i++) {
		switch(pmic_otp_prog[i].cmd) {
		case pmic_i2c:
			val = (unsigned char) (pmic_otp_prog[i].value & 0xff);
			if( i2c_write(PFUZE100_I2C_ADDR, pmic_otp_prog[i].reg,
					1, &val, 1))
			{
				printf("i2c write failed, reg 0x%2x, value0x%2x\n",
						pmic_otp_prog[i].reg, val);
				return;
			}
			break;
		case pmic_delay:
			udelay(pmic_otp_prog[i].value * 1000);
			break;
		case pmic_vpgm:
			gpio_set_value(IMX_GPIO_NR(1, 2), pmic_otp_prog[i].value);
			break;
		case pmic_pwr:
			/* TODO */
			break;
		}
	}
}


static int do_pf0100_prog(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[])
{
	puts("Programming PMIC OTP...");
	pf0100_prog();
	puts("done. Power cycle the board\n");
	return 0;
}

U_BOOT_CMD(
	pf0100_otp_prog, 1, 0, do_pf0100_prog,
	"Program the OTP fuses on the PMIC PF0100",
	""
);
