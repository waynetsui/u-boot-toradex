/*
 * (C) Copyright 2008-2011
 * Logic Product Development, <www.logicpd.com>
 * Peter Barada <peter.barada@logicpd.com>
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

// GPIO
extern unsigned int pin_get_gpio_input(unsigned int pin);
extern unsigned int pin_set_gpio_dataout(unsigned int pin, unsigned int set);
extern unsigned int pin_init_gpio(unsigned int pin_num, unsigned int in_out);

// Turn on VAUX1 voltage for Product ID
extern void init_vaux1_voltage(void);
