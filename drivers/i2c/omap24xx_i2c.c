/*
 * Basic I2C functions
 *
 * Copyright (c) 2004 Texas Instruments
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Author: Jian Zhang jzhang@ti.com, Texas Instruments
 *
 * Copyright (c) 2003 Wolfgang Denk, wd@denx.de
 * Rewritten to fit into the current U-Boot framework
 *
 * Adapted for OMAP2420 I2C, r-woodruff2@ti.com
 *
 */

#include <common.h>

#include <asm/arch/i2c.h>
#include <asm/io.h>

#undef DEBUG_I2C

#ifdef DEBUG_I2C
static struct {
  char *name;
  unsigned addr;
} _i2c_reg_names[] = {
  { "I2C_STAT", I2C_STAT },
  { "I2C_SYSC", I2C_SYSC },
  { "I2C_CON", I2C_CON },
  { "I2C_SCLL", I2C_SCLL },
  { "I2C_SCLH", I2C_SCLH },
  { "I2C_CON", I2C_CON },
  { "I2C_OA", I2C_OA },
  { "I2C_IE", I2C_IE },
  { "I2C_CNT", I2C_CNT },
  { "I2C_SA", I2C_SA },
  { "I2C_DATA", I2C_DATA },
  { "I2C_PSC", I2C_PSC },
  { NULL, 0x0 }
};
#define __my_writew(val, addr) _my_writew(__LINE__, (val), (addr));
void _my_writew(int line, unsigned short val, unsigned int addr)
{
	int i;
	for (i=0; _i2c_reg_names[i].name; ++i)
		if (_i2c_reg_names[i].addr == addr)
			break;

	if (_i2c_reg_names[i].addr)
		printf("%d: writew(%04x, %s)\n", line, val, _i2c_reg_names[i].name);
	else
		printf("%d: writew(%04x, %08x)\n", line, val, addr);
	writew(val, addr);
}

#define __my_readw(addr) _my_readw(__LINE__, (addr))
unsigned short _my_readw(int line, unsigned int addr)
{
	unsigned short val;
	int i;
	for (i=0; _i2c_reg_names[i].name; ++i)
		if (_i2c_reg_names[i].addr == addr)
			break;

	val = readw(addr);
	if (_i2c_reg_names[i].addr)
		printf("%d: readw(%s)=%04x\n", line, _i2c_reg_names[i].name, val);
	else
		printf("%d: readw(%08x)=%04x\n", line, addr, val);
	return val;
}
#else
#define __my_writew(val, addr) writew((val), (addr))
#define __my_readw(addr) readw((addr))
#endif

static void wait_for_bb (void);
static u16 wait_for_pin (void);
static void flush_fifo(void);

void i2c_init (int speed, int slaveadd)
{
#ifdef CONFIG_OMAP3_LV_SOM
	int psc, scl=0, iclk;
#else
	u16 scl;
#endif

	__my_writew(0x2, I2C_SYSC); /* for ES2 after soft reset */
	udelay(1000);
	__my_writew(0x0, I2C_SYSC); /* will probably self clear but */

	if (__my_readw (I2C_CON) & I2C_CON_EN) {
		__my_writew (0, I2C_CON);
		udelay (50000);
	}

#ifdef CONFIG_OMAP3_LV_SOM
	speed /= 1000; // Speed is in Hz, not kHz

	/* compute divisors - dynamic decision based on i/p clock */
	psc = I2C_PSC_MAX;
	while (psc >= I2C_PSC_MIN) {
		iclk = I2C_IP_CLK / (psc + 1);
		switch (speed) {
		case OMAP_I2C_STANDARD:
			scl = (iclk * 10 / (OMAP_I2C_STANDARD * 2));
			break;
		case OMAP_I2C_HIGH_SPEED:
			/* PSC ignored for HS */
		case OMAP_I2C_FAST_MODE:
			scl = (iclk * 10 / (OMAP_I2C_FAST_MODE * 2));
			break;
			/* no default case  - fall thru */
		}
#ifdef DEBUG_I2C
		printf("Search- speed= %d SysClk=%d, iclk=%d,psc=0x%x[%d],scl=0x%x[%d]\n",
		       speed, I2C_IP_CLK, iclk, psc, psc, scl, scl);
#endif
		/* Check for decimal places.. if yes, we ignore it */
		if (scl % 10) {
			scl = -1;
		} else {
			scl /= 10;
			scl -= 7;
		}
		if (scl >= 0) {
			break;
		}
		psc--;
	}
	/* Did not find an optimal config */
	if (psc < I2C_PSC_MIN) {
		printf
		    ("Unable to set Prescalar for i2c_clock=%d syI2C_IP_CLK=%d\n",
		     speed, I2C_IP_CLK);
		psc = 0;
		return;

	}
	iclk = I2C_IP_CLK / (psc + 1);
	/* Initialize the I2C clock timers to generate an I2C bus clock
	 * frequency of i2c_clock kilohertz (default is 100 KHz).
	 */
	switch (speed) {
	case OMAP_I2C_STANDARD:
		scl =
		    (((iclk / (OMAP_I2C_STANDARD * 2)) - 7) &
		     I2C_SCLL_SCLL_M) << I2C_SCLL_SCLL;
		break;
	case OMAP_I2C_HIGH_SPEED:
		scl =
		    (((I2C_IP_CLK / (OMAP_I2C_HIGH_SPEED * 2)) - 7) &
		     I2C_SCLH_HSSCLL_M) << I2C_SCLL_HSSCLL;
		/* Fall through for the FS settings */
	case OMAP_I2C_FAST_MODE:
		scl |=
		    (((iclk / (OMAP_I2C_FAST_MODE * 2)) - 7) &
		     I2C_SCLL_SCLL_M) << I2C_SCLL_SCLL;
		break;
		/* no default case */
	}

#ifdef DEBUG_I2C
	printf(" speed= %d SysClk=%d, iclk=%d,psc=0x%x[%d],scl=0x%x[%d]\n",
	       speed, I2C_IP_CLK, iclk, psc, psc, scl, scl);
#endif
	__my_writew (psc, I2C_PSC);

#else
	/* 12MHz I2C module clock */
	__my_writew (0, I2C_PSC);
	speed = speed/1000;		    /* 100 or 400 */
	scl = ((12000/(speed*2)) - 7);	/* use 7 when PSC = 0 */
#endif
	__my_writew (scl, I2C_SCLL);
	__my_writew (scl, I2C_SCLH);
	/* own address */
	__my_writew (slaveadd, I2C_OA);
	__my_writew (I2C_CON_EN, I2C_CON);

	/* have to enable intrrupts or OMAP i2c module doesn't work */
	__my_writew (I2C_IE_XRDY_IE | I2C_IE_RRDY_IE | I2C_IE_ARDY_IE |
		I2C_IE_NACK_IE | I2C_IE_AL_IE, I2C_IE);
	udelay (1000);
	flush_fifo();
	__my_writew (0xFFFF, I2C_STAT);
	__my_writew (0, I2C_CNT);
}

static int i2c_read_byte (u8 devaddr, u8 regoffset, u8 * value)
{
	int i2c_error = 0;
	u16 status;

	/* wait until bus not busy */
	wait_for_bb ();

	/* one byte only */
	__my_writew (1, I2C_CNT);
	/* set slave address */
	__my_writew (devaddr, I2C_SA);
	/* no stop bit needed here */
	__my_writew (I2C_CON_EN | I2C_CON_MST | I2C_CON_STT | I2C_CON_TRX, I2C_CON);

	status = wait_for_pin ();

	if (status & I2C_STAT_XRDY) {
		/* Important: have to use byte access */
		writeb (regoffset, I2C_DATA);
		udelay (20000);
		if (__my_readw (I2C_STAT) & I2C_STAT_NACK) {
			i2c_error = 1;
		}
	} else {
		i2c_error = 1;
	}

	if (!i2c_error) {
		/* free bus, otherwise we can't use a combined transction */
		__my_writew (0, I2C_CON);
		while (__my_readw (I2C_STAT) || (__my_readw (I2C_CON) & I2C_CON_MST)) {
			udelay (10000);
			/* Have to clear pending interrupt to clear I2C_STAT */
			__my_writew (0xFFFF, I2C_STAT);
		}

		wait_for_bb ();
		/* set slave address */
		__my_writew (devaddr, I2C_SA);
		/* read one byte from slave */
		__my_writew (1, I2C_CNT);
		/* need stop bit here */
		__my_writew (I2C_CON_EN | I2C_CON_MST | I2C_CON_STT | I2C_CON_STP,
			I2C_CON);

		status = wait_for_pin ();
		if (status & I2C_STAT_RRDY) {
#if defined(CONFIG_OMAP243X) || defined(CONFIG_OMAP34XX)
			*value = readb (I2C_DATA);
#else
			*value = __my_readw (I2C_DATA);
#endif
			udelay (20000);
		} else {
			i2c_error = 1;
		}

		if (!i2c_error) {
			__my_writew (I2C_CON_EN, I2C_CON);
			while (__my_readw (I2C_STAT)
			       || (__my_readw (I2C_CON) & I2C_CON_MST)) {
				udelay (10000);
				__my_writew (0xFFFF, I2C_STAT);
			}
		}
	}
	flush_fifo();
	__my_writew (0xFFFF, I2C_STAT);
	__my_writew (0, I2C_CNT);
	return i2c_error;
}

static int i2c_write_byte (u8 devaddr, u8 regoffset, u8 value)
{
	int i2c_error = 0;
	u16 status, stat;

	/* wait until bus not busy */
	wait_for_bb ();

	/* two bytes */
	__my_writew (2, I2C_CNT);
	/* set slave address */
	__my_writew (devaddr, I2C_SA);
	/* stop bit needed here */
	__my_writew (I2C_CON_EN | I2C_CON_MST | I2C_CON_STT | I2C_CON_TRX |
		I2C_CON_STP, I2C_CON);

	/* wait until state change */
	status = wait_for_pin ();

	if (status & I2C_STAT_XRDY) {
#if defined(CONFIG_OMAP243X) || defined(CONFIG_OMAP34XX)
		/* send out 1 byte */
		writeb (regoffset, I2C_DATA);
		__my_writew (I2C_STAT_XRDY, I2C_STAT);

		status = wait_for_pin ();
		if ((status & I2C_STAT_XRDY)) {
			/* send out next 1 byte */
			writeb (value, I2C_DATA);
			__my_writew (I2C_STAT_XRDY, I2C_STAT);
		} else {
			i2c_error = 1;
		}
#else
		/* send out two bytes */
		__my_writew ((value << 8) + regoffset, I2C_DATA);
#endif
		/* must have enough delay to allow BB bit to go low */
		udelay (50000);
		if (__my_readw (I2C_STAT) & I2C_STAT_NACK) {
			i2c_error = 1;
		}
	} else {
		i2c_error = 1;
	}

	if (!i2c_error) {
#ifdef DEBUG_I2C
		int eout = 10;
#else
		int eout = 200;
#endif

		__my_writew (I2C_CON_EN, I2C_CON);
		while ((stat = __my_readw (I2C_STAT)) || (__my_readw (I2C_CON) & I2C_CON_MST)) {
			udelay (1000);
			/* have to read to clear intrrupt */
			__my_writew (0xFFFF, I2C_STAT);
			if(--eout == 0) /* better leave with error than hang */
				break;
		}
	}
	flush_fifo();
	__my_writew (0xFFFF, I2C_STAT);
	__my_writew (0, I2C_CNT);
	return i2c_error;
}

static void flush_fifo(void)
{	u16 stat;

	/* note: if you try and read data when its not there or ready
	 * you get a bus error
	 */
	while(1){
		stat = __my_readw(I2C_STAT);
		if(stat == I2C_STAT_RRDY){
#if defined(CONFIG_OMAP243X) || defined(CONFIG_OMAP34XX)
			readb(I2C_DATA);
#else
			__my_readw(I2C_DATA);
#endif
			__my_writew(I2C_STAT_RRDY,I2C_STAT);
			udelay(1000);
		}else
			break;
	}
}

int i2c_probe (uchar chip)
{
	int res = 1; /* default = fail */

	if (chip == __my_readw (I2C_OA)) {
		return res;
	}

	/* wait until bus not busy */
	wait_for_bb ();

	/* try to read one byte */
	__my_writew (1, I2C_CNT);
	/* set slave address */
	__my_writew (chip, I2C_SA);
	/* stop bit needed here */
	__my_writew (I2C_CON_EN | I2C_CON_MST | I2C_CON_STT | I2C_CON_STP, I2C_CON);
	/* enough delay for the NACK bit set */
	udelay (50000);

	if (!(__my_readw (I2C_STAT) & I2C_STAT_NACK)) {
		res = 0;      /* success case */
		flush_fifo();
		__my_writew(0xFFFF, I2C_STAT);
	} else {
		__my_writew(0xFFFF, I2C_STAT);	 /* failue, clear sources*/
		__my_writew (__my_readw (I2C_CON) | I2C_CON_STP, I2C_CON); /* finish up xfer */
		udelay(20000);
		wait_for_bb ();
	}
	flush_fifo();
	__my_writew (0, I2C_CNT); /* don't allow any more data in...we don't want it.*/
	__my_writew(0xFFFF, I2C_STAT);
	return res;
}

int i2c_read (uchar chip, uint addr, int alen, uchar * buffer, int len)
{
	int i;

	if (alen > 1) {
		printf ("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) {
		printf ("I2C read: address out of range\n");
		return 1;
	}

	for (i = 0; i < len; i++) {
		if (i2c_read_byte (chip, addr + i, &buffer[i])) {
			printf ("I2C read: I/O error\n");
			i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
			return 1;
		}
	}

	return 0;
}

int i2c_write (uchar chip, uint addr, int alen, uchar * buffer, int len)
{
	int i;

	if (alen > 1) {
		printf ("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) {
		printf ("I2C read: address out of range\n");
		return 1;
	}

	for (i = 0; i < len; i++) {
		if (i2c_write_byte (chip, addr + i, buffer[i])) {
			printf ("I2C read: I/O error\n");
			i2c_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
			return 1;
		}
	}

	return 0;
}

static void wait_for_bb (void)
{
	int timeout = 10;
	u16 stat;

	__my_writew(0xFFFF, I2C_STAT);	 /* clear current interruts...*/
	while ((stat = __my_readw (I2C_STAT) & I2C_STAT_BB) && timeout--) {
		__my_writew (stat, I2C_STAT);
		udelay (50000);
	}

	if (timeout <= 0) {
		printf ("timed out in wait_for_bb: I2C_STAT=%x\n",
			__my_readw (I2C_STAT));
	}
	__my_writew(0xFFFF, I2C_STAT);	 /* clear delayed stuff*/
}

static u16 wait_for_pin (void)
{
	u16 status;
	int timeout = 10;

	do {
		udelay (1000);
		status = __my_readw (I2C_STAT);
	} while (  !(status &
		   (I2C_STAT_ROVR | I2C_STAT_XUDF | I2C_STAT_XRDY |
		    I2C_STAT_RRDY | I2C_STAT_ARDY | I2C_STAT_NACK |
		    I2C_STAT_AL)) && timeout--);

	if (timeout <= 0) {
		printf ("timed out in wait_for_pin: I2C_STAT=%x\n",
			__my_readw (I2C_STAT));
			__my_writew(0xFFFF, I2C_STAT);
}
	return status;
}
