
#include "CAN2.0b.h"

/*--------------------------------------------------------------------------
 global data
--------------------------------------------------------------------------*/
//static short *g_CAN_RX_BUFFER;
static g_CAN_BUF_type *g_CAN_BUF;
static g_CAN_RX_BUF *p_CAN_RX_BUF;

static volatile unsigned short g_rx_buf_index = 0;
static volatile unsigned short g_tx_buf_index = 0;
static volatile unsigned short g_TX_Count = 0;
static volatile unsigned short g_RX_Count = 0;
static volatile unsigned short g_CAN_ERROR_FLAG = 0;	/* flag that can error isr sets */
static volatile unsigned int	g_flag_CAN_R = 0;

/*
 * Function:	Init_CAN_Port
 *
 * Parameters:	none
 *
 * Return:	none
 *
 * Description:	sets up the CAN port for the test
 */
bool Init_CAN_Port(void)
{
	unsigned int nTimer;
	short msgID;
	char mbID;
	volatile unsigned short *pCAN_Ptr;

	/*PG4=CAN RX, PG1=CAN TX */
	*pREG_PORTG_MUX &= ~(BITM_PORT_MUX_MUX1 | BITM_PORT_MUX_MUX4);
	*pREG_PORTG_MUX |= 	((( 2 << BITP_PORT_MUX_MUX1)  & BITM_PORT_MUX_MUX1) 	|
						 (( 2 << BITP_PORT_MUX_MUX4)  & BITM_PORT_MUX_MUX4)	);

	*pREG_PORTG_FER_SET  =	( BITM_PORT_FER_PX1 |
							  BITM_PORT_FER_PX4 );
    ssync();

    /* init the CAN interrupts which will be used
       Configure Interrupt Priorities using SEC */
//    adi_int_InstallHandler(INTR_CAN0_RX, CAN_RCV_HANDLER, NULL, true);
	adi_int_InstallHandler(INTR_CAN0_RX, (ADI_INT_HANDLER_PTR)CAN_RCV_HANDLER, NULL, true);
	adi_int_InstallHandler(INTR_CAN0_TX, (ADI_INT_HANDLER_PTR)CAN_XMT_HANDLER, NULL, true);
	adi_int_InstallHandler(INTR_CAN0_STAT, (ADI_INT_HANDLER_PTR)CAN_ERR_HANDLER, NULL, true);

//	/* init the CAN timing registers
//	 ===================================================
//	 BIT TIMING:
//
//	 CAN_CLOCK  : Prescaler (BRP)
//	 CAN_TIMING : SJW = 3, TSEG2 = 5, TSEG1 = 2
//	??isn't that TSEG1 should be graeter than or equal to TSEG1??
//	 ===================================================
//	 Set Bit Configuration Registers ...
//	 ===================================================*/
//	*pREG_CAN0_TIMING = 0x0352;
//
//	*pREG_CAN0_CLK  = 24;		/* 500kHz CAN Clock :: tBIT = 2us */
//
//	/*
//	tBIT = TQ x (1 + (TSEG1 + 1) + (TSEG2 + 1))
//	2e-6 = TQ x (1 + 3 + 6)
//	TQ = 2e-7
//
//	TQ = (BRP+1) / SCLK           BF609 POST SCLK:125MHZ
//	2e-7 = (BRP+1) / 125e6
//	(BRP+1) = 25
//	BRP = 24
//	*/

	*pREG_CAN0_TIMING = 0x0352;
	//汽车BMS CAN通信推荐速率为250KHz
	*pREG_CAN0_CLK  = 49;		/* 250kHz CAN Clock :: tBIT = 4us */

	ssync();


	/* init the CAN mail boxes and turn on the can ISR */
	g_CAN_BUF = heap_malloc(2, (sizeof(g_CAN_BUF_type) * BUF_SIZE * 2));
//	g_CAN_BUF = malloc( (sizeof(g_CAN_BUF_type) * BUF_SIZE * 2) );
	if( NULL == g_CAN_BUF )
	{
		return false;
	}
	//为接收缓存区分配内存
	p_CAN_RX_BUF = heap_malloc(2, (sizeof(g_CAN_RX_BUF) * BUF_SIZE));
//	g_CAN_RX_BUFFER = malloc( (sizeof(short) * BUF_SIZE) );
	if( NULL == p_CAN_RX_BUF )
	{
		return false;
	}


/*  Mailboxes 24-31 are TX Only, They'll Transmit To the RX Only Mailboxes 0-7
    Mailbox 24 Transmits ID 0x200 */
	msgID = 0x200;
	mbID = 24;
	pCAN_Ptr = (unsigned short *)(pREG_CAN0_MB00_DATA0 + (0x10 * mbID));
	*(pCAN_Ptr + 0)  = 0x7788; 		/* write msgID to DATA0 */
	*(pCAN_Ptr + 2)  = 0x5566; 		/* write msgID to DATA1 */
	*(pCAN_Ptr + 4)  = 0x3344; 		/* write msgID to DATA2 */
	*(pCAN_Ptr + 6)  = 0x1122; 		/* write msgID to DATA3 *///数据发送总是从DATA3的高位开始
	*(pCAN_Ptr + 8)  = 3; 			/* write 8 to LENGTH */
	*(pCAN_Ptr + 10) = 0; 			/* write 0 to TIMESTAMP */
	*(pCAN_Ptr + 12) = 0; 			/* write 0 to ID0 */
	*(pCAN_Ptr + 14) = (msgID << 2); /* write msgID to ID1 */
//扩展帧
	msgID = 0x201;
	mbID = 25;
	pCAN_Ptr = (unsigned short *)(pREG_CAN0_MB00_DATA0 + (0x10 * mbID));
	*(pCAN_Ptr + 0)  = 0x2900; 		/* write msgID to DATA0 */
	*(pCAN_Ptr + 2)  = 0x0629; 		/* write msgID to DATA1 */
	*(pCAN_Ptr + 4)  = 0x9006; 		/* write msgID to DATA2 */
	*(pCAN_Ptr + 6)  = 0x1990; 		/* write msgID to DATA3 */
	*(pCAN_Ptr + 8)  = 8; 			/* write 8 to LENGTH */
	*(pCAN_Ptr + 10) = 0; 			/* write 0 to TIMESTAMP */
	*(pCAN_Ptr + 12) = 0x1990; 		/* write 0 to ID0 */
	*(pCAN_Ptr + 14) = (msgID << 2)|0x2000; /* write msgID to ID1 *///扩展帧时其SRR和IDE都必须为隐性，即为1.SRR貌似会自动隐性

//	msgID = 0x200;
//	for (mbID = 24; mbID<32; mbID++)
//	{
//		pCAN_Ptr = (unsigned short *)(pREG_CAN0_MB00_DATA0 + (0x10 * mbID));
//
//		*(pCAN_Ptr + 0)  = msgID; 		/* write msgID to DATA0 */
//		*(pCAN_Ptr + 2)  = msgID; 		/* write msgID to DATA1 */
//		*(pCAN_Ptr + 4)  = msgID; 		/* write msgID to DATA2 */
//		*(pCAN_Ptr + 6)  = msgID; 		/* write msgID to DATA3 */
//		*(pCAN_Ptr + 8)  = 8; 			/* write 8 to LENGTH */
//		*(pCAN_Ptr + 10) = 0; 			/* write 0 to TIMESTAMP */
//		*(pCAN_Ptr + 12) = 0; 			/* write 0 to ID0 */
//		*(pCAN_Ptr + 14) = (msgID << 2); /* write msgID to ID1 */
//
//		msgID++;							/* Bump msgID to match Mailbox */
//	} /* end for (init TX mailbox area -- TX Only mailboxes) */


/* Mailboxes 0-7 are RX Only, They'll Receive From the TX Only Mailboxes 24-31
   Mailbox 0 Receives ID 0x200 */
	msgID = 0x200;
	for (mbID = 0; mbID<8; mbID++)
	{
		pCAN_Ptr = (unsigned short *)(pREG_CAN0_MB00_DATA0 + (0x10 * mbID));

		*(pCAN_Ptr + 0)  = 0; 		 	/* write msgID to DATA0 */
		*(pCAN_Ptr + 2)  = 0; 			/* write msgID to DATA1 */
		*(pCAN_Ptr + 4)  = 0; 			/* write msgID to DATA2 */
		*(pCAN_Ptr + 6)  = 0; 			/* write msgID to DATA3 */
		*(pCAN_Ptr + 8)  = 8; 			/* write 8 to LENGTH */
		*(pCAN_Ptr + 10) = 0; 			/* write 0 to TIMESTAMP */
		*(pCAN_Ptr + 12) = 0;		/* write 0 to ID0 */
		*(pCAN_Ptr + 14) = (msgID << 2); /* write msgID to ID1 *///扩展帧时其SRR和IDE都必须为隐性，即为1.SRR貌似会自动隐性

		msgID ++;							/* Bump msgID to match Mailbox */
	} /* end for (init RX mailbox area -- RX Only mailboxes) */
//扩展帧接收
	msgID = 0x201;
	mbID = 4;
	pCAN_Ptr = (unsigned short *)(pREG_CAN0_MB00_DATA0 + (0x10 * mbID));
	*(pCAN_Ptr + 0)  = 0; 		 	/* write msgID to DATA0 */
	*(pCAN_Ptr + 2)  = 0; 			/* write msgID to DATA1 */
	*(pCAN_Ptr + 4)  = 0; 			/* write msgID to DATA2 */
	*(pCAN_Ptr + 6)  = 0; 			/* write msgID to DATA3 */
	*(pCAN_Ptr + 8)  = 8; 			/* write 8 to LENGTH */
	*(pCAN_Ptr + 10) = 0; 			/* write 0 to TIMESTAMP */
	*(pCAN_Ptr + 12) = 0x1990; 			/* write 0 to ID0 */
	*(pCAN_Ptr + 14) = (msgID << 2)|0x2000; /* write msgID to ID1 */

	/* enable the CAN and the mailboxes */
	*pREG_CAN0_MD1 = CAN_RX_MB_LO;
	*pREG_CAN0_MD2 = CAN_RX_MB_HI;

	/* Enable All Mailboxes */
	*pREG_CAN0_MC1 = CAN_EN_MB_LO; /* mailboxes 0-7 */
	*pREG_CAN0_MC2 = CAN_EN_MB_HI; /* mailboxes 24-31 */
	ssync();

	/* Enable CAN Configuration Mode (Clear CCR) */
	*pREG_CAN0_CTL = 0;

	/* Wait for CAN Configuration Acknowledge (CCA) */
	do{
		asm("nop;");
	}while(  (*pREG_CAN0_STAT & BITP_CAN_STAT_CCA) );

	*pREG_CAN0_MBIM1 = CAN_EN_MB_LO;	/* Enable Interrupt for Mailboxes 00-8 */
	*pREG_CAN0_MBIM2 = CAN_EN_MB_HI;	/* Enable Interrupt for Mailboxes 24-31 */
	ssync();
//以下看门狗只对mail11起作用，此处由于没有使能mail11，所以程序没有现象，以下用于auto-transmit
//	*pREG_CAN0_UCCNF = BITM_CAN_UCCNF_UCE|UC_WDOG;	/* enable Universal Counter in Watchdog mode */
//	*pREG_CAN0_UCRC  = 0xABBA;		/* unique value for timestamp checking */

	return true;

} /* End Init_Port () */


/*
 * Function:	EX_INTERRUPT_HANDLER(CAN_RCV_HANDLER)
 *
 * Parameters:	none
 *
 * Return:		none
 * Description:	handles CAN RX ISR
 */
void CAN_RCV_HANDLER(void)
{
	unsigned short mbim_status;
	unsigned short bit_pos = 0;
	char mbID;
	volatile unsigned short *pCAN_Ptr;


	g_RX_Count++;
	g_flag_CAN_R = 1;

	mbim_status = *pREG_CAN0_MBRIF1;
	while (!(mbim_status & 0x8000))
	{
		mbim_status <<= 1;
		bit_pos++;
	}

	mbID = (15 - bit_pos);

	pCAN_Ptr = (unsigned short *)(pREG_CAN0_MB00_DATA0 + (0x10 * mbID));
//	以下数据读取时，假如接收到的长度没有达到8，那么其读取8位时会将之前的数据读出，且断电之后还能保存。这种情况也发生在邮箱的配置。
//  数据接收总是由Data3的高位开始
	p_CAN_RX_BUF[g_rx_buf_index].Mailbox_ID0 = ((*(pCAN_Ptr + 14)>>2)&0x7FF);	//提取其中的11bit
	p_CAN_RX_BUF[g_rx_buf_index].Mailbox_ID1 = ((*(pCAN_Ptr + 14)&0x03)<<16)|(*(pCAN_Ptr + 12));
	p_CAN_RX_BUF[g_rx_buf_index].Data0 = *(pCAN_Ptr + 6);
	p_CAN_RX_BUF[g_rx_buf_index].Data1 = *(pCAN_Ptr + 4);
	p_CAN_RX_BUF[g_rx_buf_index].Data2 = *(pCAN_Ptr + 2);
	p_CAN_RX_BUF[g_rx_buf_index].Data3 = *(pCAN_Ptr + 0);
	*pREG_CAN0_MBRIF1 = (1 << mbID);			//write 1 clear 貌似下面也进行了中断标志清除
	ssync();

	bit_pos = 1 << (15 - bit_pos);  	/* Place Mask for Clearing Status     		*/

	*pREG_CAN0_MBRIF1 	= bit_pos;		/* W1C MB RX IRQ							*/
	*pREG_CAN0_RMP1 	= bit_pos;		/* W1C Status Bit							*/

	if(++g_rx_buf_index == BUF_SIZE)
		g_rx_buf_index=0;


} /* end CAN_RCV_HANDLER */


/*
 * Function:	EX_INTERRUPT_HANDLER(CAN_XMT_HANDLER)
 *
 * Parameters:	none
 *
 * Return:		none
 *
 * Description:	handles CAN TX ISR
 */
void CAN_XMT_HANDLER(void)
{
	unsigned short mbim_status;
	unsigned short bit_pos = 0;
	char mbID;
	volatile unsigned short *pCAN_Ptr;
	int x = 0;

	g_TX_Count++;

	mbim_status = *pREG_CAN0_MBTIF2;
	while (!(mbim_status & 0x8000))
	{
		mbim_status <<= 1;
		bit_pos++;
	}

	mbID = (31 - bit_pos);

	pCAN_Ptr = (unsigned short *)(pREG_CAN0_MB00_DATA0 + (0x10 * mbID));

	g_CAN_BUF[g_rx_buf_index + g_tx_buf_index].zero = ((*pCAN_Ptr + 0x0E) >> 2);
	g_CAN_BUF[g_rx_buf_index + g_tx_buf_index].one = *pREG_CAN0_UCCNT;
	*pREG_CAN0_MB00_DATA0 = (1 << (15 - bit_pos));		//??
	ssync();

	bit_pos = 1 << (15 - bit_pos);  		/* Place Mask for Clearing Status     		*/

	*pREG_CAN0_MBTIF2 	= bit_pos;			/* W1C Detected IRQ							*/
	*pREG_CAN0_TA2		= bit_pos;			/* W1C Status Bit							*/

	if(++g_tx_buf_index == BUF_SIZE)
	{
		g_tx_buf_index=0;
	}


} /* end CAN_XMT_HANDLER */



/*
 * Function:	EX_INTERRUPT_HANDLER(CAN_ERR_HANDLER)
 *
 * Parameters:	none
 *
 * Return:		none
 *
 * Description:	handles CAN ERROR ISR
 */
void CAN_ERR_HANDLER(void)
{
	g_CAN_ERROR_FLAG = 1;
} /* end CAN_ERR_HANDLER */

/*
 * Function:	CAN_Transmit
 *
 * Parameters:	none
 *
 * Return:		1 if successful 0 if timeout
 *
 * Description:	Sends out the information in all the TX mail boxes
 */
int CAN_Transmit(void)
{
	int nStatus = 0;
	unsigned int delay;
	unsigned int nTimer;

	/* Set Transmit Requests for All TX Mailboxes */
	*pREG_CAN0_TRS1 = 0;
	*pREG_CAN0_TRS2 = 0x8000;
	ssync();

	nStatus = 1;
	return nStatus;
} /* End CAN_Transmit() */

/*
 * Function:	CAN_Receive
 *
 * Parameters:	none
 *
 * Return:		1 if successful 0 if timeout
 *
 * Description:	Sends out the information in all the TX mail boxes
 */
//int CAN_Receive(void)
//{
//	int nStatus = 0;
//	unsigned int nTimer;
//
//	nTimer = SetTimeout(0x800000);  /* this takes about 1.25 minutes to timeout */
//	if( ((unsigned int)-1) != nTimer )
//	{
//		do{
//			if(g_RX_Count >= BUF_SIZE)
//			{
//				nStatus = 1;
//				break;
//			}
//		}while( !IsTimedout(nTimer) );
//	}
//	ClearTimeout(nTimer);
//
//	return nStatus;
//} /* End CAN_Receive() */

void TEST_CAN(void* arg)
{
	int iStatus = 0;
	int nTimer;
	int x = 0;
	unsigned int bit_ctr = 0x100;
	char *p_String;
	int Length_s;
	unsigned char a,b;
	UART_Data_Struct *p_UART0_Data = (UART_Data_Struct*)arg;//串口结构体，用于缓存
	OS_ERR err;

	g_rx_buf_index = 0;
	g_tx_buf_index = 0;
	g_TX_Count = 0;
	g_RX_Count = 0;
	g_CAN_ERROR_FLAG = 0;	/* flag that can error isr sets */

	iStatus = Init_CAN_Port();

	p_String = heap_malloc(2, (sizeof(char) * 256));
	if(p_String == NULL)
	{
		printf("DDR ERR\n");
	}

	while(1)
	{
//		g_CAN_RX_BUFFER[g_rx_buf_index - 1]

#define CANMODE_TX
#ifdef	CANMODE_TX
		OSTimeDly(3000u,OS_OPT_TIME_DLY,&err);
		if(x < 2)
		{
			/* Set Transmit Requests for All TX Mailboxes */
			*pREG_CAN0_TRS1 = 0;
			*pREG_CAN0_TRS2 = bit_ctr << x;
			ssync();
//			bit_ctr = bit_ctr << 1;
			x ++;

		}
		if(x == 2)
		{
			x = 0;
//			bit_ctr = 0x100;

		}
		printf("%d.\n",g_TX_Count);
//		CAN_Transmit();
#else
		OSTimeDly(1000u,OS_OPT_TIME_DLY,&err);
		while(g_flag_CAN_R == 1)
		{
			g_flag_CAN_R = 0;
			a = (char)p_CAN_RX_BUF[g_rx_buf_index-1].Mailbox_ID0>>8;
			b = (char)p_CAN_RX_BUF[g_rx_buf_index-1].Mailbox_ID0;
			p_CAN_RX_BUF[g_rx_buf_index].Mailbox_ID0;
			Length_s = sprintf(p_String,
							"ID0: %x %x\n"
							"ID1: %x %x %x\n"
							"DATA0:%x %x\n"
							"DATA1:%x %x\n"
							"DATA2:%x %x\n"
							"DATA3:%x %x\n",
			(unsigned char)(p_CAN_RX_BUF[g_rx_buf_index-1].Mailbox_ID0>>8),(unsigned char)p_CAN_RX_BUF[g_rx_buf_index-1].Mailbox_ID0,
			(unsigned char)(p_CAN_RX_BUF[g_rx_buf_index-1].Mailbox_ID1>>16),(unsigned char)(p_CAN_RX_BUF[g_rx_buf_index-1].Mailbox_ID1>>8),(unsigned char)(p_CAN_RX_BUF[g_rx_buf_index-1].Mailbox_ID1),
			(unsigned char)(p_CAN_RX_BUF[g_rx_buf_index-1].Data0>>8),(unsigned char)p_CAN_RX_BUF[g_rx_buf_index-1].Data0,
			(unsigned char)(p_CAN_RX_BUF[g_rx_buf_index-1].Data1>>8),(unsigned char)p_CAN_RX_BUF[g_rx_buf_index-1].Data1,
			(unsigned char)(p_CAN_RX_BUF[g_rx_buf_index-1].Data2>>8),(unsigned char)p_CAN_RX_BUF[g_rx_buf_index-1].Data2,
			(unsigned char)(p_CAN_RX_BUF[g_rx_buf_index-1].Data3>>8),(unsigned char)p_CAN_RX_BUF[g_rx_buf_index-1].Data3
							);
			Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据

		}
//		printf("%d.\n",g_RX_Count);

#endif



	}

}






