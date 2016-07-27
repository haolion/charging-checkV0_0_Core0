/*
 * UART_Init.c
 *
 *  Created on: 2014-4-15
 *      Author: Administrator
 */
/*
 * UART initialization.
 * 1, #include <services\pwr\adi_pwr.h>
 * 2, #include <drivers\uart\adi_uart.h>
 * 3, #include <stdio.h>
 * 4, #define BAUD_RATE and #define the CLK value as default.
 * 5, Define ADI_UART_HANDLE and the memory used by the UART.
 * 6, Setting the pin multiplex in system.svc.
 */
#include <stdio.h>
#include "UARTx.h"

#ifdef	SoftwareDownloadMode
static volatile SerialReceiveMode ReceiveMode = FreeMode;
static volatile char flag_SoftwareDownloading = 0;//应该不需要用到
static volatile int LengthCount = 0;
static char flag_pLength = 0;
section("sdram_bank0") static SoftwareDownloadFrame Frame;
//section("sdram_bank0") unsigned char bufftest[1024];
//int itest = 0;
//int count_test = 0;
//unsigned short testchar;
#endif


ADI_UART_RESULT UARTx_IntMode_Init(int UARTx, long Baudrate, int Stopbit_Num, int Check_Mode, UART_Data_Struct* Uart_Struct)
{

	ADI_UART_RESULT    eResult = ADI_UART_FAILURE;
	uint32_t 	div;
    /* System Clock Frequencies*/
    uint32_t   fsysclk = 0u;   /* System Clock */
    uint32_t   fsclk0  = 0u;   /* System Clock0 */
    uint32_t   fsclk1  = 0u;   /* System Clock1 */


    /*初始化接收数据结构体*/
    Uart_Struct->p_Buff_Start = Uart_Struct->Buff;
    Uart_Struct->p_Buff_end = Uart_Struct->p_Buff_Start + Length_UART_BUFF - 512;//p_Buff_end后面需要留一部分512字节的长度用于缓存，这样可以减少环形节点的异常处理，提高效率
    Uart_Struct->p_Buff_Read = Uart_Struct->p_Buff_Start;
    Uart_Struct->p_Buff_Write = Uart_Struct->p_Buff_Start;
    Uart_Struct->Num_Frame = 0;
    Uart_Struct->flag = 0;
    Uart_Struct->flag_Buff_Over = 0;
    Uart_Struct->p_Buff_Tx_Start = Uart_Struct->Buff_Tx;
    Uart_Struct->p_Buff_Tx = Uart_Struct->Buff_Tx;
    Uart_Struct->flag_Tx_Finish = 0;


    /* Initialize Power service */
    if(adi_pwr_Init(myCLKIN, myCORE_MAX, mySYSCLK_MAX, myVCO_MIN) != ADI_PWR_SUCCESS)
    {
    	printf("0x%08X :Failed to initialize power service \n", eResult);
    }


    if(adi_pwr_GetSystemFreq(&fsysclk, &fsclk0, &fsclk1) != ADI_PWR_SUCCESS)
     {
         return ADI_UART_SYSCLK_FAILED;
     }

	if( UARTx == UART0)
	{

	    /* PORTx_MUX registers */
	    *pREG_PORTD_MUX |= UART0_TX_PORTD_MUX | UART0_RX_PORTD_MUX;

	    /* PORTx_FER registers */
	    *pREG_PORTD_FER |= UART0_TX_PORTD_FER | UART0_RX_PORTD_FER;

		*pREG_UART0_CTL = 0;
		*pREG_UART0_CTL |= UART_W8b;
		switch ( Stopbit_Num )
		{
			case Onebit: break;

			case Onehalfbit:*pREG_UART0_CTL |= UART_STBH;
							break;

			case twobit:*pREG_UART0_CTL |= UART_STB;
						break;

			case twohalfbit:*pREG_UART0_CTL |= UART_STB | UART_STBH;
						break;

			default:return ADI_UART_FAILURE;

		}
		switch ( Check_Mode )
		{
			case CHECK_PARITY_NO:break;

			case CHECK_PARITY_ODD:*pREG_UART0_CTL |= UART_PEN;
									break;

			case CHECK_PARITY_EVEN:	*pREG_UART0_CTL |= UART_PEN | UART_EPS;
									break;

			default:return ADI_UART_FAILURE;

		}
		//设置波特率
		div = fsclk0 / Baudrate;
//		*pREG_UART0_CLK=(((uint32_t)BIT31)|div);		//Bit clock prescaler = 1
		*pREG_UART0_CLK=(((uint32_t)BIT31)|div);		//Bit clock prescaler = 1
		//设置中断
		adi_int_InstallHandler(80,IsrUart0Tx,Uart_Struct,true);
		adi_int_InstallHandler(81,IsrUart0Rx,Uart_Struct,true);

		*pREG_UART0_IMSK_SET |= BIT0;		//允许接收中断
		*pREG_UART0_CTL |= UART_EN;		//使能串口
		Uart0_T();
		return ADI_UART_SUCCESS;

	}else
	{

	    /* PORTx_MUX registers */
	    *pREG_PORTG_MUX = UART1_TX_PORTG_MUX | UART1_RX_PORTG_MUX;

	    /* PORTx_FER registers */
	    *pREG_PORTG_FER = UART1_TX_PORTG_FER | UART1_RX_PORTG_FER;

		*pREG_UART1_CTL = 0;
		*pREG_UART1_CTL |= UART_W8b;
		switch ( Stopbit_Num )
		{
			case Onebit: break;

			case Onehalfbit:*pREG_UART1_CTL |= UART_STBH;
							break;

			case twobit:*pREG_UART1_CTL |= UART_STB;
						break;

			case twohalfbit:*pREG_UART1_CTL |= UART_STB | UART_STBH;
						break;

			default:return ADI_UART_FAILURE;

		}
		switch ( Check_Mode )
		{
			case CHECK_PARITY_NO:break;

			case CHECK_PARITY_ODD:*pREG_UART1_CTL |= UART_PEN;
									break;

			case CHECK_PARITY_EVEN:	*pREG_UART1_CTL |= UART_PEN | UART_EPS;
									break;

			default:return ADI_UART_FAILURE;

		}
		//设置波特率
		div = fsysclk / Baudrate;
		*pREG_UART1_CLK=(((uint32_t)BIT31)|div);		//Bit clock prescaler = 1

		//设置中断
		adi_int_InstallHandler(83,IsrUart1Tx,Uart_Struct,true);
		adi_int_InstallHandler(84,IsrUart1Rx,Uart_Struct,true);

		*pREG_UART1_IMSK_SET |= BIT0;		//允许接收中断
		*pREG_UART1_CTL |= UART_EN;		//使能串口
		return ADI_UART_SUCCESS;

	}
}

void IsrUart0Tx(uint32_t   iid,void* k)		//中断函数申明
{
	UART_Data_Struct* Data_Struct;
	Data_Struct = k;
	*pREG_UART0_THR = *(Data_Struct->p_Buff_Tx);
	Data_Struct->flag_Tx_Finish = 1;
	Uart0_T_Stop();			//该语句执行时会令接收中断屏蔽，不知道为什么。
	Uart0_R();
}

void IsrUart0Rx(uint32_t   iid,void* k)		//中断函数申明
{
	UART_Data_Struct* Data_Struct;
	int i;
	Data_Struct = k;
#ifdef	SoftwareDownloadMode
//	bufftest[itest++] = *pREG_UART0_RBR;
	//此处加入条件判断，兼容程序下载
	if((*pREG_UART0_RBR == 0x68) && (ReceiveMode == FreeMode))//只有在空闲模式下才能判断通信头
	{
		ReceiveMode = SoftwareDownloading;
		flag_pLength = 0;
	}else if(ReceiveMode == SoftwareDownloading)
	{
		if(flag_pLength == 0)
		{
			Frame.Length = (unsigned short)*pREG_UART0_RBR;
			flag_pLength++;
		}else if(flag_pLength == 1)
		{
//			Frame.Length = Frame.Length << 8;
			Frame.Length |= (unsigned short)(*pREG_UART0_RBR << 8);
			Frame.Length -= 8;//因为帧头和长度、CRC以及帧尾等不放入Buff，因此此处减少8个字节
			LengthCount = -4;//前面三个自己不需要(此时已经忽略了地址两位了)，忽略掉
			flag_pLength ++;
		}else if(LengthCount < Frame.Length)
		{
			if(LengthCount >= 0)
			{
				Frame.Buff[LengthCount] = *pREG_UART0_RBR;
			}
		}else//此处表示到了CRC以及帧尾部分
		{
			i = LengthCount - Frame.Length;
			if(i == 0)//此时表示到了CRC处
			{

			}else//此时表示接收完一帧，通知解包任务可以解包了，然后由解包任务重新复位ReceiveMode。
			{
				ReceiveMode = SoftwareDownloaded;
				LengthCount = -1;
			}
		}
		LengthCount ++;
	}else
	{
		ReceiveMode = ASCIIMode;
		if(Data_Struct->flag_Buff_Over == 1)			//数据缓存越界
		{
			Data_Struct->p_Buff_Write = Data_Struct->p_Buff_Start;
			Data_Struct->flag_Buff_Over = 0;
		}
	//	*(Data_Struct->p_Buff_Write++) = '2';
		*(Data_Struct->p_Buff_Write++) = (char)*pREG_UART0_RBR;
//		Data_Struct->flag = 1;//此处将标志置1，代表有数据接收到，用于其它任务判断，例如用于超时判断
	//	Uart0_T();
	}
	Data_Struct->flag = 1;//此处将标志置1，代表有数据接收到，用于其它任务判断，例如用于超时判断
#else

	if(Data_Struct->flag_Buff_Over == 1)			//数据缓存越界
	{
		Data_Struct->p_Buff_Write = Data_Struct->p_Buff_Start;
		Data_Struct->flag_Buff_Over = 0;
	}
//	*(Data_Struct->p_Buff_Write++) = '2';
	*(Data_Struct->p_Buff_Write++) = (char)*pREG_UART0_RBR;
	Data_Struct->flag = 1;//此处将标志置1，代表有数据接收到，用于其它任务判断，例如用于超时判断
//	Uart0_T();
#endif
}

void IsrUart1Tx(uint32_t   iid,void* k)		//中断函数申明
{
	*pREG_UART1_THR=0;
	Uart0_T_Stop();
}

void IsrUart1Rx(uint32_t   iid,void* k)		//中断函数申明
{
	unsigned by;
	by=*pREG_UART1_RBR;

}

void Uart0_Transmit(UART_Data_Struct *Uart0_Data_Struct,char *p_Buff, int Buff_Length)
{
	int t;
	for(t = 0; t < Buff_Length; t++)
	{
		Uart0_Data_Struct->p_Buff_Tx = p_Buff + t;
		Uart0_Data_Struct->flag_Tx_Finish = 0;
		Uart0_T();
		while( Uart0_Data_Struct->flag_Tx_Finish == 0 )
		{
			asm("nop;");
		}
	}
}

//以下两函数利用TX的中断服务函数进行发送和停止
void Uart0_T_Stop(void)
{
	*pREG_UART0_IMSK_CLR|=BIT1;		//关闭发送中断
}

void Uart0_T(void)
{
	*pREG_UART0_IMSK_SET|=BIT1;		//允许发送中断
}

void Uart0_R_Stop(void)
{
	*pREG_UART0_IMSK_CLR|=BIT0;		//关闭接收中断
}

void Uart0_R(void)
{
	*pREG_UART0_IMSK_SET|=BIT0;		//关闭接收中断
}

#ifdef	SoftwareDownloadMode
SerialReceiveMode ReceiveMode_Get(void)
{
	return ReceiveMode;
}
#endif

#ifdef	SoftwareDownloadMode
void ReceiveMode_Set(SerialReceiveMode mode)
{
	ReceiveMode = mode;
}
#endif
#ifdef	SoftwareDownloadMode
SoftwareDownloadFrame* SoftwareDownloadFrameGet_p(void)
{
	return (&Frame);
}
#endif

