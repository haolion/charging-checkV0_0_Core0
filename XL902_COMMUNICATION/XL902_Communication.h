/*
 * 功能：XL902串口协议解析
 * 版本：V1.0
 * 描述：运行于XL803M-609V1.0信号板平台上
 * 公司：深圳市星龙科技股份有限公司
 * 作者：张颖豪
 * 日期：2015/5/28
 * 备注：无
 */

#ifndef XL902_COMMUNICATION_H
#define XL902_COMMUNICATION_H

#include "../UARTx/UARTx.h"
#include "os.h"
#include "string.h"
#include "math.h"
#include "../src/Task_adc.h"
#include "../EnergyPulse/EnergyPulse.h"
#include "../SWITCH/XL902Switch.h"
#include "../soft_update/Flash/pc28f128p33.h"
#include "../DataStruct/Data_conversion.h"
#include "../soft_update/Flash/Flash_API.h"
#include "../Charger-BMS_Communication/CBMS_APLayer.h"
#include "../LTC2943/LTC2943.h"
#include "../XL902RippleWave/XL902RippleWave.h"
#include "../XL902PowerManager/XL902PowerManager.h"
#include "../soft_update/Flash/FlashAddress_map.h"


//if debug
//#define Debug_Hao
//#define Debug_ComputerSoft	1
#define Password	"XL-902"	//用于校准
#define Pi	3.141592

//后面可以增加一个串口，专门用于发送调试信息，新样机使用

//调试串口之后另开辟一函数

//git测试

//需要发送的波形数据的长度
#define	WaveDataPointNumSend	256


#ifdef BMS_Listen_Save
extern CAN_RX_BUF Buf_BMS_Listen_Save[Buf_BMS_Listen_Save_Num];
extern unsigned short Buf_BMS_Listen_Save_Count;//第0帧用于存储接收到的个数
#endif


extern OS_SEM g_Sem_Frame;
extern OS_SEM g_Sem_7793_Run;
//外部变量
//串口控制信息结构体
extern UART_Data_Struct UART0_Data;//串口结构体，用于缓存
//extern UART_Data_Struct UART0_Rx_Data, UART0_Tx_Data;

typedef enum XL902_CResult
{
	XL902CR_True,
	XL902CR_False
}XL902_CResult;

typedef enum Type_Command
{
	Frame_ERR = -1,							//帧错误
	WorkMode_Set,					//设置工作模式
	WorkMode_Read,					//读取工作模式
	Param_Set,						//设置参数
	Param_Read,						//读取参数
	Range_Set,						//设置量程
	MeasureMode_Set,				//设置测量模式
	Range_Read,						//读取量程
	Measurement_Read,				//读取测量值
	Measure_Begin,					//电能测量开始
	Measure_End,					//电能测量结束
	State_EMErr,					//电能误差状态
	PulseOut_Set,					//电能脉冲输出设置
	PulseIn_Set,					//电能脉冲输入设置
	Version,						//版本信息
	CalibrationFactor_Write,		//校准系数写
	CalibrationFactor_Read,			//校准系数读
	ZeroCompensation_Write,			//零位补偿系数写
	ZeroCompensation_Read,			//零位补偿系数读
	MI_Read,						//读取模块信息
	MI_Write,						//写模块信息
	State_Read,						//读取BMS状态
	Data_Read,						//读取BMS数据
	Error_Read,						//读取BMS错误
	BATS_Read,						//读取电池容量
	Ripple_Read,					//纹波数据读取
	SwitchRange_Read,				//档位量程读取
	wave_read,						//波形数据读取
	RippleCalibration_Write,		//纹波校准系数写
	RippleCalibration_Read,			//纹波校准系数读
	BMSVision_Set,					//BMS版本选择（可选择2015新国标和2011老国标，默认新国标）
	BMSAutoState_Set,				//BMS自动模式设置
	BoardReStart,					//底板重启
} Type_Command;

//以下为传输帧的指令部分，添加指令时直接往数组中添加字符串，同时修改Command_Num即可,另外还需修改上表
#define Command_Num	32
#define	Command_Length_Max	4			//当指令的最大长度超过4，此处需要进行修改，不包括字符串结束符
static char* Command_Str[Command_Num]=	//命令指令,二级指针
					{
							"SM",
							"RSM",
							"SP",
							"RP",
							"SR",
							"MT",
							"RR",
							"ME",
							"EB",
							"EE",
							"ES",
							"EPO",
							"EPI",
							"AV",
							"ADW",
							"ADR",
							"ZCDW",
							"ZCDR",
							"RMD",
							"WMD",
							"RS",
							"RD",
							"RE",
							"RBAT",
							"RRF",
							"RRA",
							"KL",
							"RDW",
							"RDR",
							"BVS",
							"BS",
							"BRS"
					};
static char Command_arrange[Command_Num];

//档位配置信息，以下两表格需对应
#define XL902Gears_num	7
static char *ppsXL902Gears[XL902Gears_num]=
                    {
                    		"L1A",			//电压档位
                    		"L1B",			//电压档位
                    		"L2A",			//电流档位
                    		"L2B",			//电流档位
                    		"L2C",			//电流档位
                    		"L1AUTO",		//电压自动档位
                    		"L2AUTO"		//电流自动档位
                    };

//SP指令下的参数识别
#define XL902CommandSP_num	14
static  char* XL902CommandSP[XL902CommandSP_num] =
					{
							"SAMRATE",
							"PRODUCT",
							"CHARGETYPE",
							"LOADTYPE",
							"VOLTAGERANGE",
							"CURRENTRANGE",
							"METERCONST",
							"CONST",
							"PRICE",
							"MEASURETYPE",
							"U1",
							"I1",
							"U1R",
							"I1R"
					};
static char XL902CommandSP_arrange[XL902CommandSP_num];
typedef enum Type_XL902CommandSP
{
	Instruct_ERR = -1,				//指令错误
	SAMRATE,						//采样率
	PRODUCT,						//被检产品名称
	CHARGETYPE,						//充电类型
	LOADTYPE,						//负载方式
	VOLTAGERANGE,					//电压量程
	CURRENTRANGE,					//电流量程
	METERCONST,						//脉冲常数
	CONST,							//电表常数
	PRICE,							//费率
	MEASURETYPE,					//测量方式
	ChannalU1,						//电压1通道
	ChannalI1,						//电流1通道
	ChannalU1R,						//电压1纹波通道
	ChannalI1R,						//电流1纹波通道
} Type_XL902CommandSP;

//typedef enum XL902Gears
//{
//	cU750V = 0,
//	cU450V,
//	cI400A,
//	cI200A,
//	cI80A,
//	cUAUTO,
//	cIAUTO
//}XL902Gears;



//以下为各指令内容

static char* Command_MI[]=
			{
				"SOFTVER",
				"HARDVER",
				"SN",
				"PRODUCTIONDATA",
				"DEBUGMAN",
				"TESTMAN",
				"CUSTOMER",
				"END"

			};
typedef struct	Command_Param
{
	char			flag_first;			//为了区别903，此处FLASH中读出为0x22时表示已经初始化过FLASH
	Type_Command	Command_type;		//命令类型，可用于处理任务判断
	float			SAMRATE;			//采样率
	char			PRODUCT[20];		//1->AOTEXUN
	int				CHARGETYPE;			//0->直流		1->交流
	int				LOADTYPE;			//0->电动汽车	1->其它负载
	float			VOLTAGERANGE;		//电压量程V
	float			CURRENTRANGE;		//电流量程A
	int				METERCONST;			//脉冲常数
	int				CONST;				//电表常数
	float			PRICE;				//电量单价
	int				MEASURETYPE;		//0->自动		1->手动		2->半自动
	int				VOLTAGEGEAR;		//电压档位	0->自动	1->700V
	int				CURRENTGEAR;		//电流档位	0->自动	1->250A
	int				MEASUREMODE;		//测量模式	0->自动	1->手动	2->半自动
	float			VOLTAGE_L1;			//通道1的电压
	float			CURRENT_L1;			//通道1的电流
	float			ENERGY_L1;			//通道1的电能
	float			PE;					//有功误差
	float			FE;					//无功误差
	char			RANGE;				//档位
	float			A_ADJUST;			//幅度校准系数
	float			Q_ADJUST;			//相位校准系数
	float			ZC;					//零位补偿系数

}Command_Param;

static char* str_flagLink[2]=
			{
				"Disconnect",
				"Connect"
			};
static char* str_flagMode[2]=
			{
				"Constant-current",
				"Constant-voltage"
			};

static Command_Param Command_Q_Send;
//初始化
void XL902_Communication_ini(void);

//主任务
void UART_Frame_Get(void* ConsumerArg);
void XL902_Communication(void* ConsumerArg);		//处理指令

//处理函数
Type_Command Command_Get(UART_Data_Struct *Frame);
int XL902SPInstruct_Get(UART_Data_Struct *Frame);
int Command_CMP(char *src, char *deststr);
void Command_Length_arrange(char** src, char *array_arrange, int num);
XL902_CResult Point_Move_To(UART_Data_Struct *Command_Frame, char Signal_End);
int String_to_Int(char *String_In);
float String_to_Float(char *String_In);
char *String_Get(char *String_In);
float StringEx_to_Float(char *String_In);
void Float_to_StringEx(float src,char *dst);

//任务类函数
void FuctionParam_Set(UART_Data_Struct *Frame_Command);
void FuctionWorkMode_Set(UART_Data_Struct *Frame_Command);
void FuctionWorkMode_Read(UART_Data_Struct *Frame_Command);
void FuctionParam_Read(UART_Data_Struct *Frame_Command);
void FuctionRange_Set(UART_Data_Struct *Frame_Command);
void FuctionRange_Read(UART_Data_Struct *Frame_Command);
void FuctionMMode_Set(UART_Data_Struct *Frame_Command);
void FuctionM_Read(UART_Data_Struct *Frame_Command);
void FuctionM_Begin(UART_Data_Struct *Frame_Command);
void FuctionM_End(UART_Data_Struct *Frame_Command);
void FuctionState_EMErr(UART_Data_Struct *Frame_Command);
void FuctionCF_Write(UART_Data_Struct *Frame_Command);
void FuctionCF_Read(UART_Data_Struct *Frame_Command);
void FuctionZC_Write(UART_Data_Struct *Frame_Command);
void FuctionZC_Read(UART_Data_Struct *Frame_Command);
void FuctiongState_Read(UART_Data_Struct *Frame_Command);
void FuctiongData_Read(UART_Data_Struct *Frame_Command);
void FuctiongError_Read(UART_Data_Struct *Frame_Command);
void FuctionMI_Read(UART_Data_Struct *Frame_Command);
void FuctiongBATS_Read(UART_Data_Struct *Frame_Command);
void FuctiongRipple_Read(UART_Data_Struct *Frame_Command);
void FuctiongSwitchRange_Read(UART_Data_Struct *Frame_Command);
void Fuctiongwave_Read(UART_Data_Struct *Frame_Command);
void WaveDataToString(float *data,int DotNum,char *pString);
void WaveDataSend(char* str);
void FuctionRF_Write(UART_Data_Struct *Frame_Command);
void FuctionRF_Read(UART_Data_Struct *Frame_Command);
void FuctionPulseIn_Set(UART_Data_Struct *Frame_Command);
void FuctionBMSVision_Set(UART_Data_Struct *Frame_Command);
void FuctionBMSAutoState_Set(UART_Data_Struct *Frame_Command);
void FuctionBoardReStart(UART_Data_Struct *Frame_Command);

//外部状态查询函数
char AutoModeCheck(void);//用于查询当前运行模式为自动还是半自动
#endif
