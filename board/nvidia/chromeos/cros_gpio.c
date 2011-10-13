/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

/* Implementation of per-board GPIO accessor functions */

#include <common.h>
#include <fdt_decode.h>
#include <asm/arch/gpio.h>
#include <asm/arch/tegra.h>
#include <asm/global_data.h>
#include <chromeos/common.h>
#include <chromeos/cros_gpio.h>

#define PREFIX "cros_gpio: "

DECLARE_GLOBAL_DATA_PTR;

struct cros_gpio {
	char *name;
	int default_value;
};

/*
 * This list provides names matching the dts files and the default value that
 * the gpio should take. If the gpio is strictly required, use
 * FDT_GPIO_NONE to fail out on the gpio functions. To use a default value
 * for a gpio (in the case where it's not specified in the dts file), fill in
 * a 1 or 0 in the default value field.
 */
static struct cros_gpio gpios[CROS_GPIO_MAX_GPIO] = {
	{ "write-protect-switch", FDT_GPIO_NONE },
	{ "recovery-switch", FDT_GPIO_NONE },
	{ "developer-switch", FDT_GPIO_NONE },
	{ "lid-switch", 1 },
	{ "power-switch", 0 },
};

static int g_config_node = -1;
static unsigned long g_valid_time;

int misc_init_r(void)
{
	struct fdt_gpio_state gs;
	int i, config_node;

	config_node = fdt_path_offset(gd->blob, "/config");
	if (config_node < 0)
		return -1;

	for (i = 0; i < CROS_GPIO_MAX_GPIO; i++) {
		if (fdt_decode_gpio(gd->blob, config_node, gpios[i].name,
			&gs)) {
			/*
			 * If the gpio has a default value (ie. not required,
			 * just ignore it)
			 */
			if (gpios[i].default_value == FDT_GPIO_NONE)
				return -1;
			else
				continue;
		}
		fdt_setup_gpio(&gs);
	}

	/*
	 * In theory we have to insert a delay here for charging the input
	 * gate capacitance. Consider a 200K ohms series resister and 10
	 * picofarads gate capacitance.
	 *
	 * RC time constant is
	 *     200 K ohms * 10 picofarads = 2 microseconds
	 *
	 * Then 10-90% rise time is
	 *     2 microseconds * 2.2 = 4.4 microseconds
	 *
	 * Thus, 10 microseconds gives us a 50% margin.
	 */
	g_valid_time = timer_get_us() + 10;
	g_config_node = config_node;

	return 0;
}

int cros_gpio_fetch(enum cros_gpio_index index, cros_gpio_t *gpio)
{
	struct fdt_gpio_state gs;
	int p;

	assert(g_config_node >= 0);
	assert(index >= 0 && index < CROS_GPIO_MAX_GPIO);

	if (fdt_decode_gpio(gd->blob, g_config_node, gpios[index].name, &gs)) {
		if (gpios[index].default_value == FDT_GPIO_NONE) {
			VBDEBUG(PREFIX "fail to decode gpio: %d\n", index);
			return -1;
		} else {
			gpio->index = -1;
			gpio->port = -1;
			gpio->polarity = CROS_GPIO_ACTIVE_HIGH;
			gpio->value = gpios[index].default_value;
			return 0;
		}
	}

	gpio->index = index;
	gpio->port = gs.gpio;
	gpio->polarity = (gs.flags & FDT_GPIO_ACTIVE_LOW) ?
		CROS_GPIO_ACTIVE_LOW : CROS_GPIO_ACTIVE_HIGH;
	p = (gpio->polarity == CROS_GPIO_ACTIVE_HIGH) ? 0 : 1;

	/* We can only read GPIO after g_valid_time */
	while (timer_get_us() < g_valid_time)
		udelay(10);

	gpio->value = p ^ gpio_get_value(gpio->port);

	return 0;
}

int cros_gpio_dump(cros_gpio_t *gpio)
{
#ifdef VBOOT_DEBUG
	const char const *name[CROS_GPIO_MAX_GPIO] = {
		"wpsw", "recsw", "devsw", "lidsw", "pwrsw"
	};
	int index = gpio->index;

	if (index < 0 || index >= CROS_GPIO_MAX_GPIO) {
		VBDEBUG(PREFIX "index out of range: %d\n", index);
		return -1;
	}

	VBDEBUG(PREFIX "%-6s: port=%3d, polarity=%d, value=%d\n",
			name[gpio->index],
			gpio->port, gpio->polarity, gpio->value);
#endif
	return 0;
}
