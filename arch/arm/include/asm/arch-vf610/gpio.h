/*
 * Copyright (C) 2014
 * Bhuvanchandra DV, Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __ASM_ARCH_VF610_GPIO_H
#define __ASM_ARCH_VF610_GPIO_H

#define GPIO_TO_PORT(n)			(n >> 5)
#define VF610_GPIO_DIRECTION_IN		0x0
#define VF610_GPIO_DIRECTION_OUT	0x1

/* GPIO registers */
struct gpio_regs {
	u32 gpio_pdor;
	u32 gpio_psor;
	u32 gpio_pcor;
	u32 gpio_ptor;
	u32 gpio_pdir;
};

#endif	/* __ASM_ARCH_VF610_GPIO_H */
