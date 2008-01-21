/*
 * (C) Copyright 2006
 * Author : Lacressonniere Nicolas (Atmel)
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
#ifndef CONFIG_DRIVER_DM9000	/* SAM9261EK uses DM9000 Phy */

#ifdef CONFIG_DRIVER_ETHER
#if (CONFIG_COMMANDS & CFG_CMD_NET)

#include <net.h>
#include <at91_net.h>
#include <miiphy.h>
#include <dm9161.h>


/* ----- Ethernet Buffer definitions ----- */
 
/* Receive Transfer descriptor structure */
typedef struct  _AT91S_RxTdDescriptor {
	unsigned int addr;
	union
	{
		unsigned int status;
		struct {
			unsigned int Length:12;
			unsigned int Rxbuf_off:2;
			unsigned int StartOfFrame:1;
			unsigned int EndOfFrame:1;
			unsigned int Cfi:1;
			unsigned int VlanPriority:3;
			unsigned int PriorityTag:1;
			unsigned int VlanTag:1;
			unsigned int TypeID:1;
			unsigned int Sa4Match:1;
			unsigned int Sa3Match:1;
			unsigned int Sa2Match:1;
			unsigned int Sa1Match:1;
			unsigned int Res0:1;
			unsigned int ExternalAdd:1;
			unsigned int UniCast:1;
			unsigned int MultiCast:1;
			unsigned int BroadCast:1;
		} S_Status;		
	} U_Status;
} AT91S_RxTdDescriptor, *AT91PS_RxTdDescriptor;


/* Transmit Transfer descriptor structure */
typedef struct _AT91S_TxTdDescriptor {
	unsigned int addr;
	union
	{
		unsigned int status;
		struct {
			unsigned int Length:11;
			unsigned int Res0:4;
			unsigned int LastBuff:1;
			unsigned int NoCrc:1;
			unsigned int Res1:10;
			unsigned int BufExhausted:1;
			unsigned int TransmitUnderrun:1;
			unsigned int TransmitError:1;
			unsigned int Wrap:1;
			unsigned int BuffUsed:1;
		} S_Status;		
	} U_Status;
} AT91S_TxTdDescriptor, *AT91PS_TxTdDescriptor;


#define RBF_ADDR      0xfffffffc
#define RBF_OWNER     (1<<0)
#define RBF_WRAP      (1<<1)
#define RBF_BROADCAST (1<<31)
#define RBF_MULTICAST (1<<30)
#define RBF_UNICAST   (1<<29)
#define RBF_EXTERNAL  (1<<28)
#define RBF_UNKOWN    (1<<27)
#define RBF_SIZE      0x07ff
#define RBF_LOCAL4    (1<<26)
#define RBF_LOCAL3    (1<<25)
#define RBF_LOCAL2    (1<<24)
#define RBF_LOCAL1    (1<<23)
#define RBF_EOF       (1<<15)
#define RBF_SOF       (1<<14)

#define MAX_ETH_FRAME_LEN  0x600        /* 1536 bytes. Max ethernet frame size */

#define RBF_FRAMEMAX 64
#define RBF_FRAMELEN 128

#define TBF_FRAMEMAX 64
#define TBF_FRAMELEN 0x600

/* alignment as per Errata #11 (64 bytes) is insufficient! */
AT91S_RxTdDescriptor RxtdList[RBF_FRAMEMAX] __attribute((aligned(512)));
AT91S_TxTdDescriptor TxtdList[TBF_FRAMEMAX] __attribute((aligned(512)));

unsigned char rbf_framebuf[RBF_FRAMEMAX][RBF_FRAMELEN] __attribute((aligned(4)));
unsigned char tbf_framebuf[TBF_FRAMEMAX][TBF_FRAMELEN] __attribute((aligned(4)));

unsigned int RxBuffIndex = 0;
unsigned int TxBuffIndex = 0;

static unsigned char stage_rx_buffer[MAX_ETH_FRAME_LEN];
volatile unsigned char *rx_pkt_tmp = stage_rx_buffer;

/* structure to interface the PHY */
AT91S_PhyOps PhyOps;

AT91PS_EMAC p_mac;

extern int AT91F_EMAC_Hardware_Init(void);

/*********** EMAC Phy layer Management functions *************************/
/*
 * Name:
 *	at91_EmacEnableMDIO
 * Description:
 *	Enables the MDIO bit in MAC control register
 * Arguments:
 *	p_mac - pointer to struct AT91S_EMAC
 * Return value:
 *	none
 */
void at91_EmacEnableMDIO (AT91PS_EMAC p_mac)
{
	/* Mac CTRL reg set for MDIO enable */
	p_mac->EMAC_NCR |= AT91C_EMAC_MPE;	/* Management port enable */
}

/*
 * Name:
 *	at91_EmacDisableMDIO
 * Description:
 *	Disables the MDIO bit in MAC control register
 * Arguments:
 *	p_mac - pointer to struct AT91S_EMAC
 * Return value:
 *	none
 */
void at91_EmacDisableMDIO (AT91PS_EMAC p_mac)
{
	/* Mac CTRL reg set for MDIO disable */
	p_mac->EMAC_NCR &= ~AT91C_EMAC_MPE;	/* Management port disable */
}


/*
 * Name:
 *	at91_EmacReadPhy
 * Description:
 *	Reads data from the PHY register
 * Arguments:
 *	dev - pointer to struct net_device
 *	RegisterAddress - unsigned char
 * 	pInput - pointer to value read from register
 * Return value:
 *	TRUE - if data read successfully
 */
uchar at91_EmacReadPhy (AT91PS_EMAC p_mac,
				unsigned char PhyAddress,
				unsigned char RegisterAddress,
				unsigned short *pInput)
{
	p_mac->EMAC_MAN = (AT91C_EMAC_SOF & (0x01 << 30)) |
			  (AT91C_EMAC_RW & (0x2 << 28)) |
			  ((PhyAddress & 0x1F) << 23) |
			  (RegisterAddress << 18) |
			  (AT91C_EMAC_CODE & (0x2 << 16));

	/* Wait until IDLE bit in Network Status register is cleared */
	while (!(p_mac->EMAC_NSR & AT91C_EMAC_IDLE));

	*pInput = (unsigned short) (p_mac->EMAC_MAN & 0x0000FFFF);

	return TRUE;
}


/*
 * Name:
 *	at91_EmacWritePhy
 * Description:
 *	Writes data to the PHY register
 * Arguments:
 *	dev - pointer to struct net_device
 *	RegisterAddress - unsigned char
 * 	pOutput - pointer to value to be written in the register
 * Return value:
 *	TRUE - if data read successfully
 */
uchar at91_EmacWritePhy (AT91PS_EMAC p_mac,
				unsigned char PhyAddress,
				unsigned char RegisterAddress,
				unsigned short *pOutput)
{
	p_mac->EMAC_MAN = (AT91C_EMAC_SOF & (0x01 << 30)) |
			  (AT91C_EMAC_RW & (0x1 << 28)) |
			  ((PhyAddress & 0x1F) << 23) |
			  (RegisterAddress << 18) |
			  (AT91C_EMAC_CODE & (0x2 << 16)) |
			  *pOutput;
	
	/* Wait until IDLE bit in Network Status register is cleared */
	while (!(p_mac->EMAC_NSR & AT91C_EMAC_IDLE));
	
	return TRUE;
}

/*---------------------------------------------------------------------------- */
/* \fn    AT91F_EMACInit */
/* \brief This function initialise the ethernet */
/* \return Status ( Success = 0) */
/*---------------------------------------------------------------------------- */
int AT91F_EMACInit(bd_t * bd,
		   unsigned int pRxTdList,
		   unsigned int pTxTdList)
{
	unsigned int tick = 0;
	unsigned short status;
	  
	/* Wait for PHY auto negotiation completed */
	at91_EmacEnableMDIO(p_mac);
    
	do {
		at91_EmacReadPhy(p_mac, AT91C_PHY_ADDR, DM9161_BMSR, &status);
		at91_EmacReadPhy(p_mac, AT91C_PHY_ADDR, DM9161_BMSR, &status); 

		tick++;
	}
	while (!(status & DM9161_AUTONEG_COMP) && (tick < AT91C_ETH_TIMEOUT));

	at91_EmacDisableMDIO(p_mac);
	
	if (tick == AT91C_ETH_TIMEOUT)
	{
		printf ("-E- Autonegociation Timeout\n\r");
		return 1;
	} else
		printf ("End of Autonegociation\n\r");
    
	/* the sequence write EMAC_SA1L and write EMAC_SA1H must be respected */
	p_mac->EMAC_SA1L = (bd->bi_enetaddr[3] << 24) | (bd->bi_enetaddr[2] << 16)
			 | (bd->bi_enetaddr[1] <<  8) | (bd->bi_enetaddr[0]);
	p_mac->EMAC_SA1H = (bd->bi_enetaddr[5] <<  8) | (bd->bi_enetaddr[4]);

	p_mac->EMAC_RBQP = pRxTdList;
	p_mac->EMAC_TBQP = pTxTdList;


	p_mac->EMAC_NCFGR  |= (AT91C_EMAC_CAF | AT91C_EMAC_NBC );
	p_mac->EMAC_NCFGR  &= ~(AT91C_EMAC_CLK);
	
	p_mac->EMAC_NCR  |= (AT91C_EMAC_TE | AT91C_EMAC_RE);

	return 0;
}

int eth_init (bd_t * bd)
{
	volatile unsigned int uStatus;
	unsigned int val, i;
	int ret;
	static int first_time = 0;
	
	if (!first_time)
	{
		/* Init PIOs in peripheral mode */
		AT91F_EMAC_Hardware_Init();

		*AT91C_PMC_PCER = 1 << AT91C_ID_EMAC;	/* Peripheral Clock Enable Register */
	
		/* Disable Tx and Rx */
		p_mac->EMAC_NCR = 0;
		/* Clear statistics */
		p_mac->EMAC_NCR = AT91C_EMAC_CLRSTAT;
	
		uStatus = p_mac->EMAC_ISR;
		p_mac->EMAC_TSR = 0xFFFFFFFF;
		p_mac->EMAC_RSR = 0xFFFFFFFF;

		/* We don't use interrupts. disable them */
		p_mac->EMAC_IDR = 0xFFFFFFFF;
			
		/* Enable EMAC in MII mode and enable MII Clock (25MHz) */
		p_mac->EMAC_USRIO = AT91C_EMAC_CLKEN;

#ifdef CONFIG_AT91C_USE_RMII
		p_mac->EMAC_USRIO |= AT91C_EMAC_RMII;
#endif

		/* Set MDIO clock */
#if (AT91C_MASTER_CLOCK > 80000000)
		p_mac->EMAC_NCFGR |= AT91C_EMAC_CLK_HCLK_64; 
#else
		p_mac->EMAC_NCFGR |= AT91C_EMAC_CLK_HCLK_32; 
#endif
	
		/* Init Ethernet buffers */
		RxBuffIndex = 0;
		TxBuffIndex = 0;

		/* Initialise RxtdList descriptor */
		for (i = 0; i < RBF_FRAMEMAX; ++i) {
			val = (unsigned int)(&rbf_framebuf[i][0]);
			RxtdList[i].addr = val & 0xFFFFFFF8;
			RxtdList[i].U_Status.status = 0;
		}	
		/* Set the WRAP bit at the end of the list descriptor */
		RxtdList[RBF_FRAMEMAX-1].addr |= RBF_WRAP;

		/* Initialise TxtdList descriptor */
		for (i = 0; i < TBF_FRAMEMAX; ++i) {
			val = (unsigned int)(&tbf_framebuf[i][0]);
			TxtdList[i].addr = val & 0xFFFFFFF8;
			TxtdList[i].U_Status.status = 0;
			TxtdList[i].U_Status.S_Status.BuffUsed = 1;
		}
		TxtdList[0].U_Status.S_Status.BuffUsed = 0;
		/* Set the WRAP bit at the end of the list descriptor */
		TxtdList[TBF_FRAMEMAX-1].U_Status.S_Status.Wrap = 1;
	
		at91_GetPhyInterface (&PhyOps);

		/* Phy Software Reset */
		if (!PhyOps.Reset(p_mac))
		{
			printf ("PHY not reset!!\n\r");
			return 0;
		}

		if (!PhyOps.IsPhyConnected (p_mac))
		{
			printf ("PHY not connected!!\n\r");
			return 0;
		}

		/* MII management start from here */
		if (!(ret = PhyOps.Init (p_mac)))
		{
			printf ("MAC: error during RMII initialization\n");
			return 0;
		}

		if (AT91F_EMACInit(bd, (unsigned int)RxtdList, (unsigned int)TxtdList))
			return 0;
			
		first_time = 1;
	}
	
	/* Initialize Receive Frame Pointer */
	rx_pkt_tmp = stage_rx_buffer;
			
	return 0;
}

int eth_send (volatile void *packet, int length)
{

	if ((TxtdList[TxBuffIndex].U_Status.S_Status.BuffUsed == 0)) 
	{
		TxtdList[TxBuffIndex].addr = (unsigned int)packet;
		TxtdList[TxBuffIndex].U_Status.S_Status.Length = length;
		TxtdList[TxBuffIndex].U_Status.S_Status.LastBuff = 1;
	} 
	else
		return 2;

	p_mac->EMAC_NCR |= AT91C_EMAC_TSTART;
		
	while (!(p_mac->EMAC_TSR & AT91C_EMAC_COMP));
	
	p_mac->EMAC_TSR |= AT91C_EMAC_COMP;
		
	TxtdList[TxBuffIndex].U_Status.S_Status.Length = 0;
	TxtdList[TxBuffIndex].U_Status.S_Status.LastBuff = 0;
	
	if (TxBuffIndex == (TBF_FRAMEMAX - 1))
		TxBuffIndex = 0;
	else
		TxBuffIndex ++;

	TxtdList[TxBuffIndex].U_Status.S_Status.BuffUsed = 0;

	if (p_mac->EMAC_TSR & AT91C_EMAC_UBR)
		p_mac->EMAC_TSR |= AT91C_EMAC_UBR;

	return 0;
}

int eth_rx (void)
{
	int size = 0;
				
	while (!size)
	{
		while (!(RxtdList[RxBuffIndex].addr & RBF_OWNER));
	
		if (RxtdList[RxBuffIndex].U_Status.S_Status.StartOfFrame == 1)
			rx_pkt_tmp = stage_rx_buffer;
		
		memcpy((uchar *)rx_pkt_tmp, (uchar *)(RxtdList[RxBuffIndex].addr & RBF_ADDR), RBF_FRAMELEN);
		rx_pkt_tmp += RBF_FRAMELEN;
	      	
		if (RxtdList[RxBuffIndex].U_Status.S_Status.EndOfFrame == 1)
		{
			size = RxtdList[RxBuffIndex].U_Status.S_Status.Length;

			NetReceive ((uchar *)stage_rx_buffer, size);
			if (p_mac->EMAC_RSR & AT91C_EMAC_REC)
				p_mac->EMAC_RSR |= AT91C_EMAC_REC;
			
			rx_pkt_tmp = stage_rx_buffer;
		}

		RxtdList[RxBuffIndex].U_Status.status = 0;
		RxtdList[RxBuffIndex].addr &= ~RBF_OWNER;

		if (RxtdList[RxBuffIndex].addr & RBF_WRAP)
			RxBuffIndex = 0;
		else
			RxBuffIndex++;
	}
	
	return size;
}

void eth_halt (void)
{
};

#endif	/* CONFIG_COMMANDS & CFG_CMD_NET */
#endif  /* CONFIG_DRIVER_DM9000 */
#endif	/* CONFIG_DRIVER_ETHER */
