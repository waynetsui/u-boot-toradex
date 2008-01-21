/*
 * (C) Copyright 2005
 * M. Amine SAYA, ATMEL Rousset, France.
 * Added support to AT91SAM9261 LCD Controller
 *
 * (C) Copyright 2001-2002
 * Wolfgang Denk, DENX Software Engineering -- wd@denx.de
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

/************************************************************************/
/* ** HEADER FILES							*/
/************************************************************************/

#include <config.h>
#include <common.h>
#include <version.h>
#include <stdarg.h>
#include <linux/types.h>
#include <devices.h>

#undef  CONFIG_LCD_LOGO
#include <lcd.h>
#include "lcd_lut.h"


#ifdef CONFIG_LCD
//#include <asm/arch/pio.h>

#define LCD_BPP  LCD_COLOR8

/* #define DEBUG */
#define FB_SYNC_HOR_HIGH_ACT	1	/* horizontal sync high active	*/
#define FB_SYNC_VERT_HIGH_ACT	2	/* vertical sync high active	*/

/* More or less configurable parameters for LCDC controller*/
#define SIDSAFB_FIFO_SIZE		512
#define SIDSAFB_DMA_BURST_LEN		16
#define SIDSAFB_CRST_VAL                0xc8   /* 0xda */


AT91PS_SYS  AT91_SYS = (AT91PS_SYS)AT91C_BASE_SYS;

/* 640x480x16 @ 61 Hz */
vidinfo_t panel_info = {
	vl_col:		240,
	vl_row:		320,
	vl_width:	64,
	vl_height:	86,
	vl_pixclock:    4965000,
	vl_hsp:		FB_SYNC_HOR_HIGH_ACT,
	vl_vsp:		FB_SYNC_VERT_HIGH_ACT,
	vl_bpix:	3,
	vl_tft:		1,
	/* Horizontal control Timing */
	vl_hpw:		5,
	vl_blw:		1,
	vl_elw:		33,
	/* Vertical control Timing */
	vl_vpw:		1,
	vl_bfw:		1,
	vl_efw:		0,
};




/*----------------------------------------------------------------------*/
void lcd_setcolreg (ushort regno, ushort red, ushort green, ushort blue);
void lcd_ctrl_init (void *lcdbase);
void lcd_enable	(void);
void lcd_init_lut(void);


int lcd_line_length;
int lcd_color_fg;
int lcd_color_bg;

void *lcd_base;			/* Start of framebuffer memory	*/
void *lcd_console_address;		/* Start of console buffer	*/

short console_col;
short console_row;



/************************************************************************/
void lcd_init_lut(void)
{
	unsigned int i;
	for(i =0; i <256; i++) {
#if defined(CONFIG_AT91SAM9261EK) || defined(CONFIG_AT91SAM9263EK)
		panel_info.controller.lcdc->LCDC_LUT_ENTRY[i] = 
		   ((rgbPalette[i].red) >> 3 |
		   ((rgbPalette[i].green & 0xf8) << 2) |
		   ((rgbPalette[i].blue & 0xf8) << 7));
#else
		panel_info.controller.lcdc->LCDC_LUT_ENTRY[i] = 
		   ((rgbPalette[i].blue) >> 3 |
		   ((rgbPalette[i].green & 0xfc) << 3) |
		   ((rgbPalette[i].red & 0xf8) << 8));
#endif
	}
}


void lcd_setcolreg (ushort regno, ushort red, 
		    ushort green, ushort blue)
{
#if defined(CONFIG_AT91SAM9261EK) || defined(CONFIG_AT91SAM9263EK)
  panel_info.controller.lcdc->LCDC_LUT_ENTRY[regno] = ((red >> 3) |
						       ((green & 0xf8) << 2) |
						       ((blue & 0xf8) << 7));
#else
  panel_info.controller.lcdc->LCDC_LUT_ENTRY[regno] = ((blue >> 3) |
						       ((green & 0xfc) << 3) |
						       ((red & 0xf8) << 8));
#endif
}


void lcd_ctrl_init (void *lcdbase)
{
	unsigned long value;

	AT91F_LCDC_TFT_CfgPIO();

#ifdef CONFIG_AT91SAM9261EK
	/* Enable HCLOCK in PMC */
	*AT91C_PMC_SCER = AT91C_PMC_HCK1;
#endif

#ifdef CONFIG_AT91SAM9263EK
	/* Enable LCDC clock in PMC */
	*AT91C_PMC_PCER = 1 << AT91C_ID_LCDC;
#endif
#ifdef CONFIG_AT91SAM9RLEK
	/* Enable LCDC clock in PMC */
	*AT91C_PMC_PCER = 1 << AT91C_ID_LCDC;
#endif
	/* Init controller field in panel_info */
	panel_info.controller.lcdc = (AT91PS_LCDC) AT91C_BASE_LCDC;
	panel_info.controller.frame_buffer = (unsigned long) lcdbase;

	/* Turn off the LCD controller and the DMA controller */
	panel_info.controller.lcdc->LCDC_PWRCON = 0x00;
	panel_info.controller.lcdc->LCDC_DMACON = 0;
	
	/* Reset LCDC DMA */
	panel_info.controller.lcdc->LCDC_DMACON = AT91C_LCDC_DMARST;

	/* ...set frame size and burst length = 8 words (?) */
	value = ((unsigned int)panel_info.vl_row * (unsigned int)panel_info.vl_col * (unsigned int)NBITS(panel_info.vl_bpix)) / 32;
	value |= ((512 - 1) << 24);
	panel_info.controller.lcdc->LCDC_FRMCFG = value;

	/* Set pixel clock */
	value = AT91C_MASTER_CLOCK / panel_info.vl_pixclock;

	if (AT91C_MASTER_CLOCK % panel_info.vl_pixclock)
	  value++;

	value = (value / 2) - 1;

	if (!value) {
		panel_info.controller.lcdc->LCDC_LCDCON1 = AT91C_LCDC_BYPASS;
	} else
		panel_info.controller.lcdc->LCDC_LCDCON1 = value << 12;

	/* Initialize control register 2 */
	if (panel_info.vl_tft)
	  value = ( AT91C_LCDC_MEMOR_LITTLEIND | AT91C_LCDC_DISTYPE_TFT | AT91C_LCDC_CLKMOD);
	else
	  value = ( AT91C_LCDC_MEMOR_LITTLEIND | AT91C_LCDC_CLKMOD);

	if (!((unsigned int)panel_info.vl_hsp & FB_SYNC_HOR_HIGH_ACT))
		value |= 1 << 10;	/* INVLINE */
	if (!((unsigned int)panel_info.vl_vsp & FB_SYNC_VERT_HIGH_ACT))
		value |= 1 << 9;	/* INVFRAME */

	value |= (unsigned int)(panel_info.vl_bpix << 5);	
	
	panel_info.controller.lcdc->LCDC_LCDCON2 = value;

	/* Vertical timing */
	value = (unsigned int)((panel_info.vl_vpw - 1) << 16);
	value |= (unsigned int)(panel_info.vl_bfw << 8);
	value |= (unsigned int)panel_info.vl_efw;
	panel_info.controller.lcdc->LCDC_TIM1 = value;

	/* Horizontal timing */
	value = (unsigned int)((panel_info.vl_elw - 1) << 21);
	value |= (unsigned int)((panel_info.vl_hpw - 1) << 8);
	value |= (unsigned int)(panel_info.vl_blw - 1);

	panel_info.controller.lcdc->LCDC_TIM2 = value;

	value = (unsigned int)((panel_info.vl_col - 1) << 21);
	value |= (unsigned int)(panel_info.vl_row - 1);

	panel_info.controller.lcdc->LCDC_LCDFRCFG = value;

	/* FIFO Threshold: Use formula from data sheet */
	value = SIDSAFB_FIFO_SIZE - (2 * SIDSAFB_DMA_BURST_LEN + 3);
	panel_info.controller.lcdc->LCDC_FIFO = value;

	/* Toggle LCD_MODE every frame */
	value = 0;
	panel_info.controller.lcdc->LCDC_MVAL = value;

	/* Disable all interrupts */
	panel_info.controller.lcdc->LCDC_IDR = ~0UL;
	/* Set contrast */
	value = AT91C_LCDC_PS_DIVIDEDBYEIGHT | AT91C_LCDC_POL_POSITIVEPULSE | AT91C_LCDC_ENA_PWMGEMENABLED;
	panel_info.controller.lcdc->LCDC_CTRSTCON = value;
	panel_info.controller.lcdc->LCDC_CTRSTVAL = 0xDA; /*SIDSAFB_CRST_VAL;*/

	panel_info.controller.lcdc->LCDC_BA1 = (unsigned int)lcdbase;
	panel_info.controller.lcdc->LCDC_FRMCFG = (15 << 24) + ((unsigned int)panel_info.vl_col * (unsigned int)panel_info.vl_row * (unsigned int)NBITS(panel_info.vl_bpix)) / 32;
	panel_info.controller.lcdc->LCDC_DMACON    = AT91C_LCDC_DMAEN;
	panel_info.controller.lcdc->LCDC_PWRCON    = AT91C_LCDC_PWR | 0x0C;
}


/*----------------------------------------------------------------------*/
void lcd_enable (void)
{
#ifdef CONFIG_AT91SAM9261EK
       *AT91C_PIOA_SODR = AT91C_PIO_PA12;
#endif

#ifdef CONFIG_AT91SAM9263EK
       *AT91C_PIOD_SODR = AT91C_PIO_PD12;
#endif
}

/*----------------------------------------------------------------------*/
void lcd_disable (void)
{
#ifdef CONFIG_AT91SAM9261EK
       *AT91C_PIOA_CODR = AT91C_PIO_PA12;
#endif

#ifdef CONFIG_AT91SAM9263EK
       *AT91C_PIOD_CODR = AT91C_PIO_PD12;
#endif
}

/*----------------------------------------------------------------------*/

ulong calc_fbsize (void)
{
	ulong size;
	int line_length = ((unsigned int)panel_info.vl_col * (unsigned int)NBITS(panel_info.vl_bpix)) / 8;

	size = line_length * (unsigned int)panel_info.vl_row;
	size += PAGE_SIZE;

	return size;
}

#endif /* CONFIG_LCD */
