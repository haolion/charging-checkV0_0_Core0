/*
 * 程序名称：SAE-J1939链路层
 * 描述：	1、基于CAN2.0B协议；
 * 		2、 根据协议对数据接收进行控制，包括握手，释放等；
 * 		3、打包以及解包；
 * 		4、本程序为侦听模式；
 * 		5、本程序适用于XL902的DC-MeterV0.1(直流表主板V0.1)主板。
 * 版本：V1.0
 * 编写：张颖豪
 * 版本修改：张颖豪
 * 时间：2015/12/15
 *
 */
#ifndef SAE_J1939_H
#define SEA_J1939_H

#include "CAN2.0.h"
#include "../DataStruct/gen_list.h"
#include "../DataStruct/CircularQueue.h"
#include <string.h>

#define J1939_UCOS

#ifdef J1939_UCOS
#include "os.h"
#endif



#define Debug_SAEJ1939
#define J1939_Connected		1
#define J1939_Unconnected	0
#define J7939_FirstTime	1
#define NotFirstTime	0

/*SAEJ1939 define*/
#define Priority_TPCM		7	//连接管理报文
#define PGN_TPCM	0x00EC00	//连接管理报文
#define Priority_TPDT		7	//传输数据报文
#define PGN_TPDT	0x00EB00	//传输数据报文

// J1939 Defined Addresses
#define J1939_GLOBAL_ADDRESS	255
#define J1939_NULL_ADDRESS	254

typedef enum
{
	TPCM_RTS = 16,				//请求发送
	TPCM_CTS = 17,				//准备发送
	TPCM_EndofMsgAsk = 19,		//结束应答
	TPCM_Abort = 255,			//放弃连接
	TPCM_BAM = 32				//广播公告消息

}Transmit_Mode;

/* Internal Constants*/
typedef enum
{
	J1939_FALSE = 0,
	J1939_TRUE
}J1939_Result;

//J1939的一个接收帧单元
typedef struct J1939Unit
{
	unsigned int PGN;
	unsigned char Length;
	unsigned char Data[8];
}J1939Unit;

typedef struct J1939BuffNode
{
	/* CRITICAL: First 2 parameters used to add this struct to linked	*/
	/* lists using list_add_last, etc.					*/
	DBL_LNK l;                  /* CRITICAL: DON'T MOVE.	*/
	J1939Unit Data;
}J1939BuffNode;


#define J1939RBuffSize	128
//J1939的一个接收标记信息单元
typedef struct J1939RecieveList
{
	/* CRITICAL: First 2 parameters used to add this struct to linked	*/
	/* lists using list_add_last, etc.					*/
	DBL_LNK l;                  /* CRITICAL: DON'T MOVE.	*/
	unsigned short FrameByte_Num;//整个帧的字节数
	unsigned char Frame_Num;	//帧数量


}J1939RecieveList;
//接收列表节点
/*
 * BMS与充电机的通信协议为点对点通信，其PGN中的源地址都置0
 */
typedef struct J1939RListNode
{
	/* CRITICAL: First 2 parameters used to add this struct to linked	*/
	/* lists using list_add_last, etc.					*/
	DBL_LNK l;                  /* CRITICAL: DON'T MOVE.	*/
	unsigned int PGN;			//PGN号
	char Flag_Connected;		//连接建立状态
	unsigned int timetick;		//时刻
	unsigned char NUM;			//接收到的数量
	J1939RecieveList *RLInfoHead;//链表头
}J1939RListNode;

typedef enum
{
	Rx,
	Tx
}Can_Mode;

typedef struct SAEJ1939_mailParam
{
	unsigned char	priority;
	unsigned int	PGN;
	unsigned char	PS;
	unsigned char	SA;
	unsigned char	length;
	Can_Mode TxRx;

}SAEJ1939_mailParam;

//用于传递的帧结构
#define FrameLength_Max	1786
typedef struct J1939Frame_Buff
{
	unsigned int PGN;
	unsigned short Length;
	unsigned char Buff[FrameLength_Max];

}J1939Frame_Buff;


typedef struct J1939INFO
{
	J1939BuffNode *BuffHead;
	J1939RListNode *RListHead;
	J1939RecieveList *RLInfoHead;

}J1939INFO;

//配置接收邮箱
J1939_Result J1939MailParam_Set(SAEJ1939_mailParam *mailParam, char mail_num);
//配置传输协议连接管理帧
void J1939TPMail_Set(unsigned char Address_souce,unsigned char Address_dest, Can_Mode TxRx);
J1939_Result J1939_Start(void);
void J1939_Recieve(void);
int J1939FrameNum_Recieve(void);
J1939Frame_Buff *J1939Frame_get(void);
void J1939SemTrigger(void);




#endif




