/*
 * Copyright (C) 2014, Toradex AG
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Helpers for Freescale PMIC PF0100
*/

#ifndef PF0100_H_
#define PF0100_H_

/* 7-bit I2C bus slave address */
#define PFUZE100_I2C_ADDR 		(0x08)
/* Register Addresses */
#define PFUZE100_DEVICEID		(0x0)
#define PFUZE100_REVID			(0x3)
#define PFUZE100_SW1AMODE		(0x23)
#define PFUZE100_SW1ACON		36
#define PFUZE100_SW1ACON_SPEED_VAL	(0x1<<6)	/*default */
#define PFUZE100_SW1ACON_SPEED_M	(0x3<<6)
#define PFUZE100_SW1CCON		49
#define PFUZE100_SW1CCON_SPEED_VAL	(0x1<<6)	/*default */
#define PFUZE100_SW1CCON_SPEED_M	(0x3<<6)
#define PFUZE100_SW1AVOL		32
#define PFUZE100_SW1AVOL_VSEL_M		(0x3f<<0)
#define PFUZE100_SW1CVOL		46
#define PFUZE100_SW1CVOL_VSEL_M		(0x3f<<0)
#define PFUZE100_VGEN1CTL		(0x6c)
#define PFUZE100_VGEN1_VAL		(0x30 + 0x08) /* Always ON, 1.2V */
#define PFUZE100_SWBSTCTL		(0x66)
#define PFUZE100_SWBST_VAL		(0x40 + 0x08 + 0x00) /* Always ON, Auto Switching Mode, 5.0V */

/* enables some rails on a not programmed PMIC */
/* VGEN1 to 1.2V, set SWBST to 5.0V */
void pmic_init(void);

/* programmes OTP fuses to values required on a Toradex Apalis iMX6 */
void pf0100_prog(void);

#endif /* PF0100_H_ */
