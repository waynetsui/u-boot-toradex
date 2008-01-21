/*
 * (C) Copyright 2005
 * M. Amine SAYA ATMEL Rousset, France.
 *
 * (C) Copyright 2003
 * Texas Instruments <www.ti.com>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002-2004
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * (C) Copyright 2004
 * Philippe Robin, ARM Ltd. <philippe.robin@arm.com>
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
#include <asm/arch/hardware.h>


#define TIMER_LOAD_VAL (AT91C_MASTER_CLOCK / (16 * CFG_HZ))

/* macro to read the 20-bit wide AT91SAM9263 PITC value*/
#define READ_RESET_TIMER (p_pitc->PITC_PIVR)
#define READ_TIMER (p_pitc->PITC_PIIR)

#define HIGH_TIMER_VALUE(X)  ((((X) & 0xfff00000) >> 20))
#define TIMER_VALUE(X) ((((X) & 0xfff00000) >> 20) * TIMER_LOAD_VAL + ((X) & 0x000fffff))


ulong get_timer_masked (void);

volatile AT91PS_PITC p_pitc;

static ulong timestamp;
static ulong lastinc;

/* nothing really to do with interrupts, just starts up a counter. */
int interrupt_init (void)
{
  p_pitc = AT91C_BASE_PITC;
  
  /* Enable PITC Clock
   The clock is already enabled for system controller in boot */
  *AT91C_PMC_PCER = 1 << AT91C_ID_SYS;
  
  /* Enable PITC */
  p_pitc->PITC_PIMR = AT91C_PITC_PITEN;
  
  /* Load PITC_PIMR with the right timer value */
  p_pitc->PITC_PIMR |= TIMER_LOAD_VAL;
  
  reset_timer_masked();

  return 0;
}


/*
 * timer without interrupts
 */

void reset_timer (void)
{
	reset_timer_masked ();
}

ulong get_timer (ulong base)
{
	return get_timer_masked () - base;
}

void set_timer (ulong t)
{
	timestamp = t;
}

ulong get_all_timer(void)
{
  ulong now;

  now = READ_TIMER;
  return TIMER_VALUE(now);
}


/* delay x useconds AND perserve advance timstamp value */
void udelay (unsigned long usec)
{
  ulong tmo;

  if(usec >= 1000){                  /* if "big" number, spread normalization to seconds */
    tmo = usec / 1000;               /* start to normalize for usec to ticks per sec */
    tmo *= (AT91C_MASTER_CLOCK/16);  /* find number of "ticks" to wait to achieve target */
    tmo /= 1000;                     /* finish normalize. */
  }else{                             /* else small number, don't kill it prior to HZ multiply */
    tmo = usec * (AT91C_MASTER_CLOCK/16);
    tmo /= (1000*1000);
  }
  
  reset_timer_masked();
  tmo += get_all_timer();		/* get current timestamp */

  while(get_all_timer() < tmo);         /* loop till event */
}

void reset_timer_masked (void)
{
  /* reset time */
  lastinc = READ_RESET_TIMER;

  /* start "advancing" time stamp from 0 */
  timestamp = 0;
}

ulong get_timer_masked (void)
{
  ulong now = READ_TIMER;

  now = HIGH_TIMER_VALUE(now);

  timestamp = now;

  lastinc = now;
  
  return timestamp;
}

/* waits specified delay value and resets timestamp */
void udelay_masked (unsigned long usec)
{
  ulong tmo;
  
  if(usec >= 1000){               /* if "big" number, spread normalization to seconds */
    tmo = usec / 1000;            /* start to normalize for usec to ticks per sec */
    tmo *= (AT91C_MASTER_CLOCK/16);    /* find number of "ticks" to wait to achieve target */
    tmo /= 1000;                  /* finish normalize. */
  }else{                          /* else small number, don't kill it prior to HZ multiply */
    tmo = usec * (AT91C_MASTER_CLOCK/16);
    tmo /= (1000*1000);
  }
  
  reset_timer_masked();	/* set "advancing" timestamp to 0, set lastinc value */
  tmo += get_all_timer();

  while (get_all_timer() < tmo) /* wait for time stamp to overtake tick number.*/
    /*NOP*/;
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
  return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
  ulong tbclk;
  
  tbclk = CFG_HZ;
  return tbclk;
}

/*
 * Reset the cpu by setting up the Reset Controller
 * on the SAM9263EK board
 */
void reset_cpu (ulong ignored)
{
	/* Use Reset Controller */
	
	
	while (1);
	/* Never reached */
}

