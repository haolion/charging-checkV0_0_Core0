/*
 * UART_Init.h
 *
 *  Created on: 2014-4-15
 *      Author: Administrator
 */

#ifndef UARTx_INIT_H_
#define UARTx_INIT_H_
#include <drivers\uart\adi_uart.h>
#include <services\pwr\adi_pwr.h>
#include <services\stdio\adi_stdio.h>
#include <stdio.h>
#include <services/gpio/adi_gpio.h>
#include "defines.h"
#include "../system/adi_initialize.h"

//定义程序下载的版本，不需要时可以注释掉
#define SoftwareDownloadMode

#ifdef	SoftwareDownloadMode
typedef enum
{
	FreeMode,
	SoftwareDownloading,	//表示正在接收程序下载的过程
	SoftwareDownloaded,		//表示程序下载接收完了1帧
	ASCIIMode,				//表示ASCII码模式的数据接收
}SerialReceiveMode;
typedef struct SoftwareDownloadFrame
{
	unsigned short Length;
	unsigned char Buff[1024*400];
}SoftwareDownloadFrame;
#endif

#define UART_EN		BIT0
#define UART_MOD	(BIT4|BIT5)
#define UART_W8b	(BIT8|BIT9)//word length 8 bit
#define UART_STB	BIT12	//Stop Bits.
#define UART_STBH	BIT13	//Stop Bits (Half Bit Time).
#define UART_PEN	BIT14	//Parity Enable
#define UART_EPS	BIT15	//Even Parity Select.


#define UART0_RX_PORTD_MUX  ((uint32_t) ((uint32_t) 1<<16))
#define UART0_TX_PORTD_MUX  ((uint16_t) ((uint16_t) 1<<14))

#define UART0_TX_PORTD_FER  ((uint16_t) ((uint16_t) 1<<7))
#define UART0_RX_PORTD_FER  ((uint32_t) ((uint32_t) 1<<8))

#define UART1_TX_PORTG_MUX  ((uint32_t) ((uint32_t) 0<<30))
#define UART1_RX_PORTG_MUX  ((uint32_t) ((uint32_t) 0<<28))

#define UART1_TX_PORTG_FER  ((uint32_t) ((uint32_t) 1<<15))
#define UART1_RX_PORTG_FER  ((uint32_t) ((uint32_t) 1<<14))
/* default power settings */
#define MHZTOHZ       (1000000)
#define DF_DEFAULT    (0x0)
#define MSEL_DEFAULT  (0x10)
#define SSEL_DEFAULT  (0x8)
#define CSEL_DEFAULT  (0x4)

//定义系统时钟信息
#ifndef MYCLK_IFO
#define MYCLK_IFO
#define MHZTOHZ       (1000000)
#define myCLKIN         (20 * MHZTOHZ)   /* should corresponding to the fact input osc. */
#define myCORE_MAX      (400 * MHZTOHZ)
#define mySYSCLK_MAX    (200 * MHZTOHZ)
#define mySCLK_MAX      (200 * MHZTOHZ)
#define myVCO_MIN       (72     * MHZTOHZ)
#endif
#define STDIO_UART_PHYSICAL_DEV_ID 0

enum
{
	UART0,
	UART1
};
enum
{
	Onebit,
	Onehalfbit,
	twobit,
	twohalfbit
};
enum
{
	CHECK_PARITY_NO,
	CHECK_PARITY_ODD,
	CHECK_PARITY_EVEN
};


#define Length_UART_BUFF	2048
typedef struct UART_Data_Struct		//	以下未标明TX的皆为RX数据
{
	char					Buff[Length_UART_BUFF];
	char					*p_Buff_Start;
	char		 			*p_Buff_end;			//p_Buff_end后面需要留一部分512字节的长度用于缓存，这样可以减少环形节点的异常处理，提高效率
	char					*p_Buff_Read;
	char 					*p_Buff_Write;
	unsigned int 			Num_Frame;
	char					flag;
	char					flag_Buff_Over;			//用于写数据缓存越界判断，越界时将写指针返回开始处
	char					Buff_Tx[Length_UART_BUFF];
	char					*p_Buff_Tx_Start;
	char					*p_Buff_Tx;
	int						Buff_Tx_Length;
	volatile char			flag_Tx_Finish;
}UART_Data_Struct;
//UART_Data_Struct UART0_Rx_Data, UART0_Tx_Data;

/*
 * inti the UART,user should include the adi_uart.h and this header file. in addition to,
 * user should define the TX and RX buffer by themselves.
 * user should also define the ADI_UART_HANDLE which need to be used to read the buffer.
 */
ADI_UART_RESULT UARTx_IntMode_Init(int UARTx, long Baudrate, int Stopbit_Num, int Check_Mode, UART_Data_Struct* Uart_Struct);
void Uart0_Transmit(UART_Data_Struct *Uart0_Data_Struct,char *p_Buff, int Buff_Length);
//以下两函数利用TX的中断服务函数进行发送和停止
void Uart0_T_Stop(void);
void Uart0_T(void);
void Uart0_R_Stop(void);
void Uart0_R(void);
void IsrUart0Tx(uint32_t   iid,void* k);
void IsrUart0Rx(uint32_t   iid,void* k);
void IsrUart1Tx(uint32_t   iid,void* k);
void IsrUart1Rx(uint32_t   iid,void* k);

#ifdef	SoftwareDownloadMode
SerialReceiveMode ReceiveMode_Get(void);
#endif
#ifdef	SoftwareDownloadMode
void ReceiveMode_Set(SerialReceiveMode mode);
#endif
#ifdef	SoftwareDownloadMode
SoftwareDownloadFrame* SoftwareDownloadFrameGet_p(void);
#endif
#endif /* UART_INIT_H_ */
