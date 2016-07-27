/*
 * 程序名称：充电机与BMS通信协议应用层
 * 描述：	1、物理层基于CAN2.0B协议；
 * 		2、 链路层基于SAE-J1939；
 * 		3、报文解析发送；
 * 		4、用户处理；
 * 		5、本程序适用于XL902的DC-MeterV0.1(直流表主板V0.1)主板。
 * 版本：V1.0
 * 编写：张颖豪
 * 版本修改：张颖豪
 * 时间：2015/12/15
 *
 */
#ifndef CBMS_APLAYER_H
#define CBMS_APLAYER_H

#include "SAE_J1939.h"
#include "../UARTx/UARTx.h"

#define XL902MeasureAuto_CBMS
//东方威思顿版本
#define LX902Dongfangweisidun


#ifdef XL902MeasureAuto_CBMS
#include "../src/Task_adc.h"
#endif

//#define CBMS_DEBUG


// Internal Constants
typedef enum
{
	CBMS_FALSE = 0,
	CBMS_TRUE
}CBMS_Result;

typedef enum CBMSVision
{
	CBMSVision2015,
	CBMSVision2011
}CBMSVision;

typedef enum XL902MeasureAutoState
{
	XL902MeasureAuto_Open,
	XL902MeasureAuto_Close
}XL902MeasureAutoState;


/*	PGN List	*/
#define PGN_NUM		20

#define PGN_CRM		0x000100	//充电机识别报文
#define PGN_BRM		0x000200	//BMS和车辆辨别报文

#define	PGN_BCP		0x000600	//动力蓄电池充电参数
#define PGN_CTS		0x000700	//充电机发送时间同步信息
#define PGN_CML		0x000800	//充电机最大输出能力
#define PGN_BRO		0x000900	//电池充电准备就绪状态
#define PGN_CRO		0x000A00	//充电机输出准备就绪状态

#define PGN_BCL		0x001000	//电池充电需求
#define PGN_BCS		0x001100	//电池充电总状态
#define PGN_CCS		0x001200	//充电机充电状态
#define PGN_BSM		0x001300	//动力蓄电池状态信息
#define PGN_BMV		0x001500	//单体动力蓄电池电压
#define PGN_BMT		0x001600	//动力蓄电池温度
#define PGN_BSP		0x001700	//动力蓄电池预留报文
#define PGN_BST		0x001900	//BMS中止充电
#define PGN_CST		0x001A00	//充电机中止充电

#define PGN_BSD		0x001C00	//BMS统计数据
#define PGN_CSD		0x001D00	//充电机统计数据

#define PGN_BEM		0x001E00	//BMS错误报文
#define PGN_CEM		0x001F00	//充电机错误报文

//#define PGN_TPCM	0x00EC00	//连接管理报文

/*	Priority List	*/
#define Priority_CRM		6	//充电机识别报文
#define Priority_BRM		6	//BMS和车辆辨别报文

#define	Priority_BCP		6	//动力蓄电池充电参数
#define Priority_CTS		6	//充电机发送时间同步信息
#define Priority_CML		6	//充电机最大输出能力
#define Priority_BRO		4	//电池充电准备就绪状态
#define Priority_CRO		4	//充电机输出准备就绪状态

#define Priority_BCL		6	//电池充电需求
#define Priority_BCS		6	//电池充电总状态
#define Priority_CCS		6	//充电机充电状态
#define Priority_BSM		6	//动力蓄电池状态信息
#define Priority_BMV		6	//单体动力蓄电池电压
#define Priority_BMT		6	//动力蓄电池温度
#define Priority_BSP		6	//动力蓄电池预留报文
#define Priority_BST		4	//BMS中止充电
#define Priority_CST		4	//充电机中止充电

#define Priority_BSD		6	//BMS统计数据
#define Priority_CSD		6	//充电机统计数据

#define Priority_BEM		2	//BMS错误报文
#define Priority_CEM		2	//充电机错误报文

//#define Priority_TPCM		7	//连接管理报文

/*	Length	List	*/
#define Length_CRM		8		//充电机识别报文
#define Length_BRM		41		//BMS和车辆辨别报文

#define	Length_BCP		13		//动力蓄电池充电参数
#define Length_CTS		7		//充电机发送时间同步信息
#define Length_CML		6		//充电机最大输出能力
#define Length_BRO		1		//电池充电准备就绪状态
#define Length_CRO		1		//充电机输出准备就绪状态

#define Length_BCL		5		//电池充电需求
#define Length_BCS		9		//电池充电总状态
#define Length_CCS		6		//充电机充电状态
#define Length_BSM		7		//动力蓄电池状态信息
#define Length_BMV		0		//单体动力蓄电池电压		//NULL表示其长度不定，此处为了方便程序处理
#define Length_BMT		0		//动力蓄电池温度
#define Length_BSP		0		//动力蓄电池预留报文
#define Length_BST		4		//BMS中止充电
#define Length_CST		4		//充电机中止充电

#define Length_BSD		7		//BMS统计数据
#define Length_CSD		5		//充电机统计数据

#define Length_BEM		4		//BMS错误报文
#define Length_CEM		4		//充电机错误报文

//#define Length_TPCM		8		//连接管理报文

/*	address	*/
#define	Adress_Charge	0x56
#define Adress_BMS		0xF4

/*	SPN definition	*/
#define	BMS_Unrecognized	0x00
#define BMS_Recognized		0xAA

#define BMS_Notready		0x00
#define BMS_Ready			0xAA
#define BMS_Invalid			0xFF

#define Charger_Notready	0x00
#define Charger_Ready		0xAA
#define Charger_Invalid		0xFF

#define Charger_CC			0x01
#define Charger_CV			0x02

#define	VBAT_OK				0x00
#define VBAT_High			0x01
#define	VBAT_Low			0x02
#define	BATSOC_OK			0x00
#define BATSOC_High			0x04
#define BATSOC_Low			0x08
#define	CBAT_OK				0x00
#define	CBAT_High			0x10
#define CBAT_Low			0x20
#define TBAT_OK				0x00
#define TBAT_High			0x40
#define	TBAT_Low			0x80

#define	IBAT_OK				0x00
#define IBAT_Abnormal		0x01
#define	IBAT_Unbelievable	0x02
#define	BATConnect_OK		0x00
#define	BATConnect_Abnormal	0x04
#define	BATConnect_Unbelievable	0x08
#define Charg_Allow			0x00
#define	Charg_Ban			0x10

////Transmit  protocol//
//#define TPCM_RTS 16				//请求发送
//#define TPCM_CTS 17				//准备发送
//#define TPCM_EMA 19				//消息结束应答
//#define TPCM_ABORT	255			//放弃连接
//#define TPCM_BAM	32			//广播公告消息


/*--------------------------------------------------------------------------
   constant defines
--------------------------------------------------------------------------*/
#define CANBUF_SIZE		1024

#define CANMAIL_NUM		20

//typedef enum
//{
//	CRM = 0,		//充电机识别报文
//	BRM,			//BMS和车辆辨别报文
//	BCP,			//动力蓄电池充电参数
//	CTS,			//充电机发送时间同步信息
//	CML,			//充电机最大输出能力
//	BRO,			//电池充电准备就绪状态
//	CRO,			//充电机输出准备就绪状态
//	BCK,			//电池充电需求
//	BCS,			//电池充电总状态
//	CCS,			//充电机充电状态
//	BSM,			//动力蓄电池状态信息
//	BMV,			//单体动力蓄电池电压
//	BMT,			//动力蓄电池温度
//	BSP,			//动力蓄电池预留报文
//	BST,			//BMS中止充电
//	CST,			//充电机中止充电
//	BSD,			//BMS统计数据
//	CSD,			//充电机统计数据
//	BEM,			//BMS错误报文
//	CEM				//充电机错误报文
//}PGN;
//
//typedef struct g_Frame_Recieve
//{
//	PGN Frame_PGN;
//	unsigned short Data0;
//	unsigned short Data1;
//	unsigned short Data2;
//	unsigned short Data3;
//}g_Frame_Recieve;

typedef struct SAEJ1939_PGN256
{
	unsigned char recognize;			//辨识结果
	unsigned char Charger_Num;			//充电机编号
	char Charger_AreaCoding[6];			//充电机/充电桩所在区域编码，标准ASCII码	可选项

}SAEJ1939_PGN256;		//充电机辨识报文

typedef struct SAEJ1939_PGN512
{
//	unsigned short Frame_Length;		//帧长度，包含在有包含可选项的帧中
	unsigned char BMSCommunication_Ver[3];	//BMS通信协议版本号
	unsigned char BAT_Type;				//电池类型
	unsigned short BAT_Capacity;		//电池额定容量
	unsigned short BAT_Voltage;			//电池额定总电压
	char BAT_Manufacturer[4];			//电池生产厂商，标准ASCII码	可选项
	unsigned char BAT_SeriesNum[4];		//电池组序号	可选项
	unsigned char BAT_ProductionYear;	//电池生产日期-年	可选项
	unsigned char BAT_ProductionMonth;	//电池生产日期-月	可选项
	unsigned char BAT_ProductionDay;	//电池生产日期-日	可选项
	unsigned char BAT_ChargeNum[3];		//电池组充电次数		可选项
	unsigned char BAT_Own;				//电池组产权标识		可选项
	unsigned char Reserve;				//预留	可选项
	unsigned char VIN[17];				//车辆识别码	可选项

}SAEJ1939_PGN512;		//BMS和车辆辨识报文

typedef struct SAEJ1939_PGN1536
{
	unsigned short ChargeVoltage_CellMax;	//单体最高允许充电电压
	unsigned short ChargeCurrent_Max;	//最高允许充电电流
	unsigned short BAT_NominalPower;	//电池标称总能量
	unsigned short ChargeVoltage_Max;	//最高允许充电总电压
	unsigned char Temperature_Max;		//最高允许温度
	unsigned short SOC;					//整车动力蓄电池荷电状态
	unsigned short BAT_Voltage;			//整车动力蓄电池总电压

}SAEJ1939_PGN1536;		//动力蓄电池充电参数报文

typedef struct SAEJ1939_PGN1792
{
	unsigned char Time_Sync[7];			//年/月/日/时/分/秒		可选项

}SAEJ1939_PGN1792;		//充电机发送时间同步信息报文

typedef struct SAEJ1939_PGN2048
{
	unsigned short VoltageOutput_Max;	//最高输出电压
	unsigned short VoltageOutput_Min;	//最低输出电压
	unsigned short CurrentOutput_Max;	//最大输出电流
	unsigned short CurrentOutput_Min;	//最小输出电流(2015新国标)

}SAEJ1939_PGN2048;		//充电机发送时间同步信息报文

typedef struct SAEJ1939_PGN2304
{
	unsigned char IsBMSReady;			//BMS是否充电准备就绪

}SAEJ1939_PGN2304;		//BMS充电准备就绪报文

typedef struct SAEJ1939_PGN2560
{
	unsigned char IsChargerRaedy;		//充电机是否准备就绪

}SAEJ1939_PGN2560;		//充电机输出准备就绪报文

typedef struct SAEJ1939_PGN4096
{
	unsigned short Voltage_Requirement;	//电压需求
	unsigned short Current_Requirement;	//电流需求
	unsigned char Charge_Mode;			//充电模式

}SAEJ1939_PGN4096;		//电池充电需求报文

typedef struct SAEJ1939_PGN4352
{
	unsigned short Voltage_Measured;	//充电电压测量值
	unsigned short Current_Measured;	//充电电流测量值
	unsigned short Cell_V_N;			//最高单体电池电压及其组号
	unsigned char SOC;					//当前荷电状态
	unsigned short Time_Remain;			//剩余时间

}SAEJ1939_PGN4352;		//电池充电总状态报文

typedef struct SAEJ1939_PGN4608
{
	unsigned short Voltage_Output;		//电压输出值
	unsigned short Current_Output;		//电流输出值
	unsigned short Time_Cumulative;		//累计充电时间
	unsigned char ChargeAllow:2;		//充电允许（2015新国标）

}SAEJ1939_PGN4608;		//电池充电总状态报文

typedef struct SAEJ1939_PGN4864
{
	unsigned char VHighestCell_Num;		//最高单体电池电压所在编号
	unsigned char THighest;				//最高电池温度
	unsigned char THighestCell_Num;		//最高温度检测点编号
	unsigned char TLowest;				//最低电池温度
	unsigned char TLowestCell_Num;		//最低温度检测点编号
	unsigned char CellV_State: 2;		//单体电池电压状态
	unsigned char SOC: 2;				//整车电池荷电状态
	unsigned char OverC_State: 2;		//电池充电过电流状态
	unsigned char OverT_State: 2;		//电池温度过高
	unsigned char Insulation_State: 2;	//电池绝缘状态
	unsigned char Connect_State: 2;		//电池组输出连接器连接状态
	unsigned char Charge_Allow: 2;		//充电允许

}SAEJ1939_PGN4864;		//BMS发送电池状态信息报文

typedef struct SAEJ1939_PGN5376
{
	unsigned short CellBAT_V[256];		//单体电池电压	都为可选项

}SAEJ1939_PGN5376;		//单体电池电压报文

typedef struct SAEJ1939_PGN5632
{
	unsigned char CellBAT_T[128];		//单体电池温度	都为可选项

}SAEJ1939_PGN5632;		//电池温度报文

typedef struct SAEJ1939_PGN5888
{
	unsigned char BAT_Reserve[16];		//电池预留字段	都为可选项

}SAEJ1939_PGN5888;		//电池预留报文

//#pragma pack(push) //保存对齐状态
//#pragma pack(2)//设定为2字节对齐
typedef struct SAEJ1939_PGN6400
{
	unsigned char BMSStopCharge;		//BMS 中止充电原因
	unsigned short BMSStopChargeFault;	//BMS 中止充电故障原因
	unsigned char BMSStopChargeErr;		//BMS 中止充电错误原因

}SAEJ1939_PGN6400;		//BMS 中止充电报文
//#pragma pack(pop)//恢复对齐状态


typedef struct SAEJ1939_PGN6656
{
	unsigned char ChargerStop;			//充电机中止充电原因
	unsigned short ChargerFault;		//充电机中止充电故障原因
	unsigned char ChargerErr;			//充电机中止充电错误原因

}SAEJ1939_PGN6656;		//充电机中止充电报文

typedef struct SAEJ1939_PGN7168
{
	unsigned char SOC;					//中止荷电状态SOC
	unsigned short BATCellV_Min;		//电池单体最低电压
	unsigned short BATCellV_Max;		//电池单体最高电压
	unsigned char BATT_Min;				//电池最低温度
	unsigned char BATT_Max;				//电池最高温度

}SAEJ1939_PGN7168;		//BMS统计数据报文

typedef struct SAEJ1939_PGN7424
{
	unsigned short Time_Cumulative;		//累计充电时间
	unsigned short Energy_Output;		//输出能量
	unsigned char Charger_Num;			//充电机编号

}SAEJ1939_PGN7424;		//充电机统计数据报文

typedef struct SAEJ1939_PGN7680
{
	unsigned char Recieve_TimeOut1: 2;	//接收SPN = 0x00的充电机辨识报文超时
	unsigned char Recieve_TimeOut2: 2;	//接收SPN = 0xAA的充电机辨识报文超时
	unsigned char :0;
	unsigned char ChargerTSOUTTO: 2;	//接收充电机时间同步和充电机最大输出能力报文超时
	unsigned char R_BeReady_TimeOut: 2;	//接收充电机完成充电准备报文超时
	unsigned char :0;
	unsigned char R_SOC_TimeOut: 2;		//接收充电机充电状态报文延时
	unsigned char R_CStop_TimeOut: 2;	//接收充电机中止充电报文延时
	unsigned char :0;
	unsigned char R_CInfo_TimeOut: 2;	//接收充电机充电统计报文超时

}SAEJ1939_PGN7680;		//BMS错误报文

typedef struct SAEJ1939_PGN7936
{
	unsigned char R_Identify_TimeOut: 2;	//接收BMS和车辆的辨识报文超时
	unsigned char :0;
	unsigned char R_Parameter_TimeOut: 2;	//接收电池充电参数报文延时
	unsigned char R_BeReady_TimeOut: 2;		//接收BMS完成充电准备报文延时
	unsigned char :0;
	unsigned char R_BATState_TimeOut: 2;	//接收电池充电总状态报文延时
	unsigned char R_BATRequire_TimeOut: 2;	//接收电池充电要求报文超时
	unsigned char R_BMSStop_TimeOut: 2;		//接收BMS中止充电报文超时
	unsigned char :0;
	unsigned char R_BMSInfo_TimeOut: 2;		//接收BMS充电统计报文超时

}SAEJ1939_PGN7936;		//充电机错误报文

typedef struct SAEJ1939_TPCM
{
	unsigned char Control_byte;
	unsigned char bytes[7];
}SAEJ1939_TPCM;

typedef struct CBMS
{
	SAEJ1939_PGN256 SCRM;
	SAEJ1939_PGN512	SBRM;
	SAEJ1939_PGN1536 SBCP;
	SAEJ1939_PGN1792 SCTS;
	SAEJ1939_PGN2048 SCML;
	SAEJ1939_PGN2304 SBRO;
	SAEJ1939_PGN2560 SCRO;
	SAEJ1939_PGN4096 SBCL;
	SAEJ1939_PGN4352 SBCS;
	SAEJ1939_PGN4608 SCCS;
	SAEJ1939_PGN4864 SBSM;
	SAEJ1939_PGN5376 SBMV;
	SAEJ1939_PGN5632 SBMT;
	SAEJ1939_PGN5888 SBSP;
	SAEJ1939_PGN6400 SBST;
	SAEJ1939_PGN6656 SCST;
	SAEJ1939_PGN7168 SBSD;
	SAEJ1939_PGN7424 SCSD;
	SAEJ1939_PGN7680 SBEM;
	SAEJ1939_PGN7936 SCEM;

}CBMS;

typedef enum
{
	PGN256 = 0,
	PGN512,
	PGN1536,
	PGN1792,
	PGN2048,
	PGN2304,
	PGN2560,
	PGN4096,
	PGN4352,
	PGN4608,
	PGN4864,
	PGN5376,
	PGN5632,
	PGN5888,
	PGN6400,
	PGN6656,
	PGN7168,
	PGN7424,
	PGN7680,
	PGN7936,
	PGNTPCM
}CBMS_PGN;

#define CRM		PGN256	//充电机识别报文
#define BRM		PGN512	//BMS和车辆辨别报文

#define	BCP		PGN1536	//动力蓄电池充电参数
#define CTS		PGN1792	//充电机发送时间同步信息
#define CML		PGN2048	//充电机最大输出能力
#define BRO		PGN2304	//电池充电准备就绪状态
#define CRO		PGN2560	//充电机输出准备就绪状态

#define BCL		PGN4096	//电池充电需求
#define BCS		PGN4352	//电池充电总状态
#define CCS		PGN4608	//充电机充电状态
#define BSM		PGN4864	//动力蓄电池状态信息
#define BMV		PGN5376	//单体动力蓄电池电压
#define BMT		PGN5632	//动力蓄电池温度
#define BSP		PGN5888	//动力蓄电池预留报文
#define BST		PGN6400	//BMS中止充电
#define CST		PGN6656	//充电机中止充电

#define BSD		PGN7168	//BMS统计数据
#define CSD		PGN7424	//充电机统计数据

#define BEM		PGN7680	//BMS错误报文
#define CEM		PGN7936	//充电机错误报文

//与其它任务进行沟通的结构体，分为3个结构体，减少参数转移时间，判断是否有新数据有flag_update标志判断
typedef struct CBMS_state			//存放各种状态变量以及一些频繁收发的数据
{
	char flag_update;			//更新标志
	char flag_Link;				//BMS连接状态
	char flag_BMSReady;			//BMS准备状态
	char flag_ChargerReady;		//充电机准备状态
	float Voltage_Need;			//电压需求
	float Current_Need;			//电流需求
	char flag_Mode;				//充电模式
	float Voltage_Output;		//电压输出值
	float Current_Output;		//电流输出值
	float Min_Charge;			//累计充电时间

}CBMS_state;

typedef struct CBMS_info			//存放各类通信频率低的数据
{
	char flag_update;			//更新标志
	float CurrentIn_Max;		//最大允许充电电流
	float VoltageIn_Max;		//最大允许充电电压
	float VoltageTotal_Now;		//电池总电压
	float VoltageOut_Max;		//最高输出电压
	float VoltageOut_Min;		//最低输出电压
	float CurrentOut_Max;		//最高输出电流
	float Voltage_Measure;		//充电电压测量值
	float Current_Measure;		//充电电流测量值
	int SOC;					//当前荷电状态
	short Min_Remain;			//剩余充电时间
	short Min_Charge;			//累计充电时间
	short Energy_Total;			//输出能量

}CBMS_info;

typedef struct CBMS_ERR			//存放各种BMS的错误信息
{
	char flag_update;			//更新标志
	char BMSStop_Cause;			//BMS中止充电原因
	unsigned short BMSStopFault_Cause;	//BMS中止充电故障原因
	char BMSStopErr_Carse;		//BMS中止充电错误原因
	char CStop_Cause;			//充电机中止充电原因
	unsigned short CStopFault_Cause;	//充电机中止充电故障原因
	char CStopErr_Cause;		//充电机中止充电错误原因
	char Timeout0x00;			//接收0x00充电机辨识报文超时
	char Timeout0xAA;			//接收0xAA充电机辨识报文超时
	char Timeout_syn;			//接收充电机时钟同步和最大输出能力报文超时
	char Timeout_ready;			//接收充电机完成充电准备报文超时
	char Timeout_state;			//接收充电机充电状态报文超时
	char Timeout_stop;			//接收中止充电报文超时
	char Timeout_stat;			//接收充电机统计报文超时
	char Timeout_recognize;		//接收BMS辨识报文超时
	char Timeout_param;			//接收电池充电参数报文超时
	char Timeout_readyC;		//接收BMS完成充电准备报文超时
	char Timeout_stateC;		//接收电池充电总状态报文超时
	char Timeout_require;		//接收电池充电要求报文超时
	char Timeout_stopC;			//接收BMS中止充电报文超时
	char Timeout_BMSStat;		//接收BMS充电统计报文超时
}CBMS_ERR;



CBMS_Result CMBS_ini(void);

void CBMSCommunication(void* arg);				//充电机与BMS通信任务
CBMS_state *CBMSState_get(void);
CBMS_info *CBMSInfo_get(void);
CBMS_ERR *CBMSErr_get(void);
void CBMSVision_Set(CBMSVision vision);
void CBMSinfo_flag_Link_UnSet(void);


#ifdef XL902MeasureAuto_CBMS
void XL902MeasureAutoState_Set(XL902MeasureAutoState state);
#endif


#endif




