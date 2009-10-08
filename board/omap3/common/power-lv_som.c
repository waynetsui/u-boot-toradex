/*
 * (C) Copyright 2004-2008
 * Texas Instruments, <www.ti.com>
 *
 * Author :
 *	Sunil Kumar <sunilsaini05@gmail.com>
 *	Shashi Ranjan <shashiranjanmca05@gmail.com>
 *
 * Derived from Beagle Board and 3430 SDP code by
 *	Richard Woodruff <r-woodruff2@ti.com>
 *	Syed Mohammed Khasim <khasim@ti.com>
 *
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
#include <common.h>
#include <asm/arch/sys_proto.h>
#include <i2c.h>

/******************************************************************************
 * Routine: power_init_r
 * Description: Configure power supply
 *****************************************************************************/
void power_init_r(void)
{
	unsigned char byte;
	unsigned short msg;

#ifdef CONFIG_DRIVER_OMAP34XX_I2C
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

	/*
	 * Configure OMAP3 supply voltages in power management
	 * companion chip.
	 */

	/* set VAUX1 to 3.0v */

	// Select output voltage
	byte = 0x04;
	i2c_write(PWRMGT_ADDR_ID4, 0x72, 1, &byte, 1);

	// Select the processor resource group
	byte = 0x20;
	i2c_write(PWRMGT_ADDR_ID4, 0x72, 1, &byte, 1);

	// Enable I2C access to the Power bus
	byte = 0x02;
	i2c_write(PWRMGT_ADDR_ID4, 0x4a, 1, &byte, 1);

	// Send Message MSB
	msg = (1 << 13) | (1<<4) | (0xd<<0); // group(process_grp1):resource(vaux1):res_active;
	byte = (msg >> 8);
	i2c_write(PWRMGT_ADDR_ID4, 0x4b, 1, &byte, 1);

	// Send Message LSB
	byte = (msg && 0xff);
	i2c_write(PWRMGT_ADDR_ID4, 0x4c, 1, &byte, 1);
#endif
}
