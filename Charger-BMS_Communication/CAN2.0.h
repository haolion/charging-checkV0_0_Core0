/*
 * 程序名称：CAN2.0驱动
 * 描述：	1、包含CAN2.0a及CAN2.0b驱动；
 * 		2、 本程序运行在ucosIII的环境中，若更改系统或环境需要在其接收中断函数内的信号量进行触发时间更改；
 * 		3、程序提供了驱动的基本接口，具体初始化需要安装需要自行初始化，特别是邮箱的安装和配置；
 * 		4、本程序适用于XL902的DC-MeterV0.1(直流表主板V0.1)主板。
 * 版本：V0.0
 * 编写：张颖豪
 * 版本修改：张颖豪
 * 时间：2015/11/10
 *
 */
#ifndef CAN2_0_H
#define CAN2_0_H

#include <cdefBF609.h>
#include <services/int/adi_int.h>
#include "stdio.h"
#include "os.h"
#include "../DataStruct/CircularQueue.h"
#include <services\pwr\adi_pwr.h>

//#define BMS_Listen_Save

#ifdef BMS_Listen_Save
#define Buf_BMS_Listen_Save_Num 8000
#endif

#define CAN2_BUF_SIZE		1024

#define ID1_CAN20bEN	0x2000		//CAN2.0b使能位，默认为2.0a

//定义系统时钟信息
#ifndef MYCLK_IFO
#define MYCLK_IFO
#define MHZTOHZ       (1000000)
#define myCLKIN         (20  * MHZTOHZ)   /* should corresponding to the fact input osc. */
#define myCORE_MAX      (500 * MHZTOHZ)
#define mySYSCLK_MAX    (250 * MHZTOHZ)
#define mySCLK_MAX      (250 * MHZTOHZ)
#define myVCO_MIN       (72  * MHZTOHZ)
#endif

typedef enum
{
	CAN20a,
	CAN20b
}CAN_Type;

typedef struct CAN_RX_BUF
{
	unsigned short Mailbox_ID1;	//基本帧，11bit + 扩展帧，2bit = 13bit
	unsigned short Mailbox_ID0;	//扩展帧,16bit
	unsigned char Length;
	unsigned short Data0;
	unsigned short Data1;
	unsigned short Data2;
	unsigned short Data3;
}CAN_RX_BUF;

typedef struct CAN_TX_BUF
{
	unsigned short Mailbox_ID0;	//基本帧，11bit
	unsigned int Mailbox_ID1;	//扩展帧,18bit + 11bit = 29bit
	unsigned short Data0;
	unsigned short Data1;
	unsigned short Data2;
	unsigned short Data3;
}CAN_TX_BUF;

extern OS_SEM g_Sem_CBMS;			//外部信号量

void CAN2_Port_Ini(void);			//管口使能

bool CAN2RxHandle_Set(void);		//服务函数安装，此处暂时不引入参数
bool CAN2TxHandle_Set(void);		//服务函数安装，此处暂时不引入参数
bool CAN2ErrHandle_Set(void);		//服务函数安装，此处暂时不引入参数

bool CAN2Fre_Set(unsigned int Fre, unsigned char SJW, unsigned char TSEG1,unsigned char TSEG2);	//Can速率、采样等时间

void MailRxID_Set(CAN_Type can, unsigned short ID0,unsigned short ID1,unsigned char Length);//接收的邮箱ID配置
void MailTxID_Set(CAN_Type can, unsigned short ID0,unsigned short ID1,unsigned char Length);//发送的邮箱ID配置

void Can2_Enable(void);		//CAN 使能

void Can2MailInt_Enable(void);	//邮箱中断是使能

bool CAN2_Transmit(CAN_TX_BUF *BUFF);		//CAN 数据发送，往函数填符合结构体类型的数据，该函数为阻塞型函数

bool CAN2Buff_Is_Empty(void);		//用于判断接收的缓存区是否没有数据
bool CAN2Buff_Is_Full(void);		//用于判断接收的缓冲区数据是否满

void CAN2Buff_Read(CAN_RX_BUF *BUFF);		//读取接收缓存区的数据，配合以上缓冲区是否为空进行读取

void CAN2_RCV_HANDLER(void);
void CAN2_XMT_HANDLER(void);
void CAN2_ERR_HANDLER(void);




#endif

