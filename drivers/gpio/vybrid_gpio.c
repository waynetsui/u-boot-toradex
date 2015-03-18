/*
 * Copyright (C) 2015
 * Bhuvanchandra DV, Toradex, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/imx-common/iomux-v3.h>

static unsigned long gpio_ports[] = {
	[0] = GPIO0_BASE_ADDR,
	[1] = GPIO1_BASE_ADDR,
	[2] = GPIO2_BASE_ADDR,
	[3] = GPIO3_BASE_ADDR,
	[4] = GPIO4_BASE_ADDR,
};

int gpio_set_value(unsigned gpio, int value)
{
	unsigned int port = GPIO_TO_PORT(gpio);
	struct gpio_regs *regs;
	u32 mask = (1 << (gpio & GPIO_PIN_MASK));

	if (port >= ARRAY_SIZE(gpio_ports))
		return -1;

	regs = (struct gpio_regs *)gpio_ports[port];

	if (value)
		writel(mask, &regs->gpio_psor);
	else
		writel(mask, &regs->gpio_pcor);

	return 0;
}

int gpio_get_value(unsigned gpio)
{
	unsigned int port = GPIO_TO_PORT(gpio);
	struct gpio_regs *regs;
	u32 mask = (1 << (gpio & GPIO_PIN_MASK));

	if (port >= ARRAY_SIZE(gpio_ports))
		return -1;

	regs = (struct gpio_regs *)gpio_ports[port];

	return ((readl(&regs->gpio_pdir) & mask)) ? 1 : 0;
}

int gpio_request(unsigned gpio, const char *label)
{
	unsigned int port = GPIO_TO_PORT(gpio);

	if (port >= ARRAY_SIZE(gpio_ports))
		return -1;

	return 0;
}

int gpio_free(unsigned gpio)
{
	return 0;
}

int gpio_direction_input(unsigned gpio)
{
	imx_iomux_gpio_set_direction(gpio, VF610_GPIO_DIRECTION_IN);

	return 0;
}

int gpio_direction_output(unsigned gpio, int value)
{
	gpio_set_value(gpio, value);

	imx_iomux_gpio_set_direction(gpio, VF610_GPIO_DIRECTION_OUT);

	return 0;
}
