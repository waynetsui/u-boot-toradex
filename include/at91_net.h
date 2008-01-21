/*
 * Ethernet:	An implementation of the Ethernet Device Driver suite for the
 *		uClinux 2.0.38 operating system. This Driver has been developed
 *		for AT75C220 board.
 *
 * NOTE:	The driver is implemented for one MAC
 *
 * Version:	@(#)at91_net.h	1.0.0	28/11/2005
 *
 * Authors:	Lineo Inc <www.lineo.com>
 *
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */

#ifndef AT91_NET
#define AT91_NET

#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>


#define TRUE                 1
#define FALSE                0

/* In this place we put all board dependent PHY addresses.
   PHY address */
#define SAM9260EK_PHY_ADDRESS 0
#define SAM9263EK_PHY_ADDRESS 0


/* Interface to drive the physical layer */
typedef struct _AT91S_PhyOps
{
	unsigned char (*Init)(AT91S_EMAC *pmac);
	unsigned int (*IsPhyConnected)(AT91S_EMAC  *pmac);
	unsigned char (*GetLinkSpeed)(AT91S_EMAC *pmac);
	unsigned char (*AutoNegotiate)(AT91S_EMAC *pmac, int *);
        unsigned char (*Reset) (AT91S_EMAC *pmac);

} AT91S_PhyOps,*AT91PS_PhyOps;



/******************  function prototypes **********************/

/* MII functions */
void at91_EmacEnableMDIO(AT91PS_EMAC p_mac);
void at91_EmacDisableMDIO(AT91PS_EMAC p_mac);
uchar at91_EmacReadPhy(AT91PS_EMAC p_mac, 
		       unsigned char PhyAddress,
		       unsigned char RegisterAddress, 
		       unsigned short *pInput);

uchar at91_EmacWritePhy(AT91PS_EMAC p_mac, 
			unsigned char PhyAddress,
			unsigned char RegisterAddress, 
			unsigned short *pOutput);

void at91_GetPhyInterface(AT91PS_PhyOps p_phyops);

#endif /* AT91_NET */
