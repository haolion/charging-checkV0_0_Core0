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
#include "CBMS_APLayer.h"


#ifdef LX902Dongfangweisidun
SAEJ1939_mailParam CBMS_Unit[] =
		{
			{Priority_CRM, PGN_CRM, Adress_BMS, Adress_Charge, Length_CRM, Rx},//充电机识别报文
			{Priority_BRM, PGN_BRM, Adress_Charge, Adress_BMS, Length_BRM, Rx},//BMS和车辆辨别报文
			{Priority_BCP, PGN_BCP, Adress_Charge, Adress_Charge, Length_BCP, Rx},//动力蓄电池充电参数
			{Priority_CTS, PGN_CTS, Adress_Charge, Adress_Charge, Length_CTS, Rx},//充电机发送时间同步信息
			{Priority_CML, PGN_CML, Adress_Charge, Adress_Charge, Length_CML, Rx},//充电机最大输出能力
			{Priority_BRO, PGN_BRO, Adress_Charge, Adress_Charge, Length_BRO, Rx},//电池充电准备就绪状态
			{Priority_CRO, PGN_CRO, Adress_Charge, Adress_Charge, Length_CRO, Rx},//充电机输出准备就绪状态
			{Priority_BCL, PGN_BCL, Adress_Charge, Adress_Charge, Length_BCL, Rx},//电池充电需求
			{Priority_BCS, PGN_BCS, Adress_Charge, Adress_Charge, Length_BCS, Rx},//电池充电总状态
			{Priority_CCS, PGN_CCS, Adress_Charge, Adress_Charge, Length_CCS, Rx},//充电机充电状态
			{Priority_BSM, PGN_BSM, Adress_Charge, Adress_Charge, Length_BSM, Rx},//动力蓄电池状态信息
			{Priority_BMV, PGN_BMV, Adress_Charge, Adress_Charge, Length_BMV, Rx},//单体动力蓄电池电压
			{Priority_BMT, PGN_BMT, Adress_Charge, Adress_Charge, Length_BMT, Rx},//动力蓄电池温度
			{Priority_BSP, PGN_BSP, Adress_Charge, Adress_Charge, Length_BSP, Rx},//动力蓄电池预留报文
			{Priority_BST, PGN_BST, Adress_Charge, Adress_BMS, Length_BST, Rx},//BMS中止充电
			{Priority_CST, PGN_CST, Adress_BMS, Adress_Charge, Length_CST, Rx},//充电机中止充电
			{Priority_BSD, PGN_BSD, Adress_Charge, Adress_BMS, Length_BSD, Rx},//BMS统计数据
			{Priority_CSD, PGN_CSD, Adress_BMS, Adress_Charge, Length_CSD, Rx},//充电机统计数据
			{Priority_BEM, PGN_BEM, Adress_Charge, Adress_Charge, Length_BEM, Rx},//BMS错误报文
			{Priority_CEM, PGN_CEM, Adress_Charge, Adress_Charge, Length_CEM, Rx} //充电机错误报文
		};
#else
SAEJ1939_mailParam CBMS_Unit[] =
		{
			{Priority_CRM, PGN_CRM, Adress_BMS, Adress_Charge, Length_CRM, Rx},//充电机识别报文
			{Priority_BRM, PGN_BRM, Adress_Charge, Adress_BMS, Length_BRM, Rx},//BMS和车辆辨别报文
			{Priority_BCP, PGN_BCP, Adress_Charge, Adress_BMS, Length_BCP, Rx},//动力蓄电池充电参数
			{Priority_CTS, PGN_CTS, Adress_BMS, Adress_Charge, Length_CTS, Rx},//充电机发送时间同步信息
			{Priority_CML, PGN_CML, Adress_BMS, Adress_Charge, Length_CML, Rx},//充电机最大输出能力
			{Priority_BRO, PGN_BRO, Adress_Charge, Adress_BMS, Length_BRO, Rx},//电池充电准备就绪状态
			{Priority_CRO, PGN_CRO, Adress_BMS, Adress_Charge, Length_CRO, Rx},//充电机输出准备就绪状态
			{Priority_BCL, PGN_BCL, Adress_Charge, Adress_BMS, Length_BCL, Rx},//电池充电需求
			{Priority_BCS, PGN_BCS, Adress_Charge, Adress_BMS, Length_BCS, Rx},//电池充电总状态
			{Priority_CCS, PGN_CCS, Adress_BMS, Adress_Charge, Length_CCS, Rx},//充电机充电状态
			{Priority_BSM, PGN_BSM, Adress_Charge, Adress_BMS, Length_BSM, Rx},//动力蓄电池状态信息
			{Priority_BMV, PGN_BMV, Adress_Charge, Adress_BMS, Length_BMV, Rx},//单体动力蓄电池电压
			{Priority_BMT, PGN_BMT, Adress_Charge, Adress_BMS, Length_BMT, Rx},//动力蓄电池温度
			{Priority_BSP, PGN_BSP, Adress_Charge, Adress_BMS, Length_BSP, Rx},//动力蓄电池预留报文
			{Priority_BST, PGN_BST, Adress_Charge, Adress_BMS, Length_BST, Rx},//BMS中止充电
			{Priority_CST, PGN_CST, Adress_BMS, Adress_Charge, Length_CST, Rx},//充电机中止充电
			{Priority_BSD, PGN_BSD, Adress_Charge, Adress_BMS, Length_BSD, Rx},//BMS统计数据
			{Priority_CSD, PGN_CSD, Adress_BMS, Adress_Charge, Length_CSD, Rx},//充电机统计数据
			{Priority_BEM, PGN_BEM, Adress_Charge, Adress_BMS, Length_BEM, Rx},//BMS错误报文
			{Priority_CEM, PGN_CEM, Adress_BMS, Adress_Charge, Length_CEM, Rx} //充电机错误报文
		};
#endif

static CBMS *spCBMS;
static J1939Frame_Buff *J1939FrameBuff;
static CBMS_state *CBMSState;
static CBMS_info *CBMSinfo;
static CBMS_ERR *CBMSErr;
static CBMSVision BMSVision = CBMSVision2015;//0表示新协议2015,1表示老协议2011.


static void TestData_ini(void);

static void CBMSFrameConverted(CBMS_PGN PGNx, unsigned char *Buff, unsigned short ByteNum);
static CBMS_PGN PGN_Get(unsigned int PGNIn);


static unsigned int Buff_Uint_Get(unsigned char *Buff, short *i);
static unsigned short Buff_Ushort_Get(unsigned char *Buff, short *i);
static unsigned char Buff_Uchar_Get(unsigned char *Buff, short *i);
static char Buff_Char_Get(unsigned char *Buff, short *i);

#ifdef XL902MeasureAuto_CBMS
static char XL902MeasureAutoSet_Lock = 0;//互斥，避免多次启动采样任务
static XL902MeasureAutoState XL902MeasureAuto_State = XL902MeasureAuto_Close;	//该变量与XL902MeasureAutoSet_Lock完全没关系，
										//该变量用于判断是否接受自动信号开启自动检测
#endif


/*
 * 函数：充电机和BMS通信的初始化
 * 描述：	1.包含物理层和链路层的初始化；
 * 接口参数：	无
 * 返回参数：	成功			CBMS_TRUE
 * 			失败			CBMS_FALSE
 * 备注：无
 *
 */
CBMS_Result CMBS_ini(void)
{
	J1939_Result Result;

	//邮箱初始化配置
	Result = J1939MailParam_Set(CBMS_Unit, PGN_NUM);
	if(Result == J1939_FALSE)
	{
#ifdef CBMS_DEBUG
		printf("J1939MailParam_Set False.\n");
#endif
		return CBMS_FALSE;
	}
#ifdef LX902Dongfangweisidun
	//传输协议邮箱配置，此处由于配置为侦听模式，其传输协议邮箱都配置为接收
	J1939TPMail_Set(Adress_Charge, Adress_Charge, Rx);
	J1939TPMail_Set(Adress_Charge, Adress_Charge, Rx);
#else
	//传输协议邮箱配置，此处由于配置为侦听模式，其传输协议邮箱都配置为接收
	J1939TPMail_Set(Adress_Charge, Adress_BMS, Rx);
	J1939TPMail_Set(Adress_BMS, Adress_Charge, Rx);
#endif
	//开始接收发送
	Result = J1939_Start();
	if(Result == J1939_FALSE)
	{
#ifdef CBMS_DEBUG
		printf("J1939MailParam_Set False.\n");
#endif
		return CBMS_FALSE;
	}

	//数据初始化
	spCBMS = heap_calloc(2, 1, sizeof(CBMS));
	if(spCBMS == NULL)
	{
#ifdef CBMS_DEBUG
		printf("J1939MailParam_Set False.\n");
#endif
		return CBMS_FALSE;
	}

	//数据初始化
	CBMSState = heap_calloc(2, 1, sizeof(CBMS_state));
	if(CBMSState == NULL)
	{
#ifdef CBMS_DEBUG
		printf("CBMSState NULL.\n");
#endif
		return CBMS_FALSE;
	}

	//数据初始化
	CBMSinfo = heap_calloc(2, 1, sizeof(CBMS_info));
	if(CBMSinfo == NULL)
	{
#ifdef CBMS_DEBUG
		printf("CBMSinfo NULL.\n");
#endif
		return CBMS_FALSE;
	}
	//初始化数据，用于调试
	TestData_ini();

	//数据初始化
	CBMSErr = heap_calloc(2, 1, sizeof(CBMS_ERR));
	if(CBMSErr == NULL)
	{
#ifdef CBMS_DEBUG
		printf("CBMSErr NULL.\n");
#endif
		return CBMS_FALSE;
	}

	return CBMS_TRUE;
}

/*
 * 函数：充电机和BMS通信应用层程序
 * 描述：	1.包含对数据接收，处理等行为，具体动作可由用户添加；
 * 接口参数：	暂时无用
 * 返回参数：	无
 * 备注：无
 *
 */
void CBMSCommunication(void* arg)
{
	OS_ERR err;
	int Frame_Num;
	int count = 0;
	CBMS_PGN PGNx;
#ifdef CBMS_DEBUG
	UART_Data_Struct *p_UART0_Data = (UART_Data_Struct*)arg;//串口结构体，用于缓存
	char *p_String;
	int Length_s;
#endif
	//初始化
	CMBS_ini();
#ifdef CBMS_DEBUG
	p_String = heap_calloc(2, 1024, sizeof(char));
	if(p_String == NULL)
	{
		printf("p_String NULL.\n");
	}
#endif
	while(1)
	{
		//接收一个信号量触发，开启任务
		J1939SemTrigger();

		//数据接收
		J1939_Recieve();

		//判断是否有接收完毕的数据帧
		Frame_Num = J1939FrameNum_Recieve();
		while(Frame_Num != 0)
		{
			Frame_Num--;
			//获取帧数据
			J1939FrameBuff = J1939Frame_get();
			if(J1939FrameBuff == NULL)
			{
#ifdef CBMS_DEBUG
				printf("J1939Frame_get NULL.\n");
#endif
				break;
			}
			//提取PGN对应的编号
			PGNx = PGN_Get(J1939FrameBuff->PGN);

			//转化为相对应的结构体，里面的数据流需要先将序号去掉，不然输出数据将包含序号，导致错误，为完成
			CBMSFrameConverted(PGNx, J1939FrameBuff->Buff, J1939FrameBuff->Length);

			//用于DEBUG判别
			count++;

			//对相应的数据帧进行处理，用户处理
			switch(PGNx)
			{
				case CRM://充电机识别报文
						CBMSState->flag_update = 1;
						if(spCBMS->SCRM.recognize == BMS_Unrecognized)
						{
							//此处可以进行某些特定动作，例如采样任务关闭,因为该指令只出现在握手阶段，此时可以利用这个进行复位，
							//防止异常中止充电造成机器计量没有复位
#ifdef XL902MeasureAuto_CBMS
							//只有发送开始检测信号时才开启检测，同时在停止检测信号以及接收到中止信号时将其改为关闭
//							if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
//							{
//								CBMSState->flag_Link = 0;
//								if((AutoModeCheck()) == 1)
//								{
//									if(XL902MeasureAutoSet_Lock == 1)
//									{
//										XL902MeasureAutoSet_Lock = 0;
//										//停止计量动作
//										XL902MeasureState_Set(XL902MeasureStop);
//									}
//								}
//							}
							if((AutoModeCheck()) == 1)
							{
								if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
								{
									CBMSState->flag_Link = 0;
									if(XL902MeasureAutoSet_Lock == 1)
									{
										XL902MeasureAutoSet_Lock = 0;
										//停止计量动作
										XL902MeasureState_Set(XL902MeasureStop);
									}
								}
							}else
							{
								if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
								{
									CBMSState->flag_Link = 0;
								}
							}

#else
							CBMSState->flag_Link = 0;
#endif

						}else if(spCBMS->SCRM.recognize == BMS_Recognized)
						{
							//此处可以进行某些特定动作，例如采样任务开启
#ifdef XL902MeasureAuto_CBMS
//							if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
//							{
//								CBMSState->flag_Link = 1;
//								if((AutoModeCheck()) == 1)
//								{
//									if(XL902MeasureAutoSet_Lock == 0)
//									{
//										XL902MeasureAutoSet_Lock = 1;
//										//计量开始动作
//										XL902MeasureState_Set(XL902MeasureRunning);
//									}
//								}
//							}

							if((AutoModeCheck()) == 1)
							{
								if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
								{
									CBMSState->flag_Link = 1;
									if(XL902MeasureAutoSet_Lock == 0)
									{
										XL902MeasureAutoSet_Lock = 1;
										//计量开始动作
										XL902MeasureState_Set(XL902MeasureRunning);
									}
								}
							}else
							{
								if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
								{
									CBMSState->flag_Link = 1;
								}
							}

#else
							CBMSState->flag_Link = 1;
#endif

						}

#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"recognize %x.\n"
										"Charger_Num %x.\n"
										"Charger_AreaCoding[0] %x.\n",
										count,
										spCBMS->SCRM.recognize,
										spCBMS->SCRM.Charger_Num,
										spCBMS->SCRM.Charger_AreaCoding[0]);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif
					break;
				case BRM://BMS和车辆辨别报文

#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"BMSCommunication_Ver[0] %x.\n"
										"BMSCommunication_Ver[1] %x.\n"
										"BMSCommunication_Ver[2] %x.\n"
										"BAT_Type %x.\n"
										"BAT_Capacity %x.\n"
										"BAT_Voltage %x.\n"
										"BAT_Manufacturer[0] %x.\n"
										"BAT_Manufacturer[1] %x.\n"
										"BAT_Manufacturer[2] %x.\n"
										"BAT_Manufacturer[3] %x.\n"
										"BAT_SeriesNum[0] %x.\n"
										"BAT_SeriesNum[1] %x.\n"
										"BAT_SeriesNum[2] %x.\n"
										"BAT_SeriesNum[3] %x.\n"
										"BAT_ProductionYear %x.\n"
										"BAT_ProductionMonth %x.\n"
										"BAT_ProductionDay %x.\n"
										"BAT_ChargeNum[0] %x.\n"
										"BAT_ChargeNum[1] %x.\n"
										"BAT_ChargeNum[2] %x.\n"
										"BAT_Own %x.\n"
										"Reserve %x.\n"
										"VIN[0] %x.\n"
										"VIN[1] %x.\n"
										"VIN[2] %x.\n"
										"VIN[3] %x.\n"
										"VIN[4] %x.\n"
										"VIN[5] %x.\n"
										"VIN[6] %x.\n"
										"VIN[7] %x.\n"
										"VIN[8] %x.\n"
										"VIN[9] %x.\n"
										"VIN[10] %x.\n"
										"VIN[11] %x.\n"
										"VIN[12] %x.\n"
										"VIN[13] %x.\n"
										"VIN[14] %x.\n"
										"VIN[15] %x.\n"
										"VIN[16] %x.\n",
										count,
										spCBMS->SBRM.BMSCommunication_Ver[0],
										spCBMS->SBRM.BMSCommunication_Ver[1],
										spCBMS->SBRM.BMSCommunication_Ver[2],
										spCBMS->SBRM.BAT_Type,
										spCBMS->SBRM.BAT_Capacity,
										spCBMS->SBRM.BAT_Voltage,
										spCBMS->SBRM.BAT_Manufacturer[0],
										spCBMS->SBRM.BAT_Manufacturer[1],
										spCBMS->SBRM.BAT_Manufacturer[2],
										spCBMS->SBRM.BAT_Manufacturer[3],
										spCBMS->SBRM.BAT_SeriesNum[0],
										spCBMS->SBRM.BAT_SeriesNum[1],
										spCBMS->SBRM.BAT_SeriesNum[2],
										spCBMS->SBRM.BAT_SeriesNum[3],
										spCBMS->SBRM.BAT_ProductionYear,
										spCBMS->SBRM.BAT_ProductionMonth,
										spCBMS->SBRM.BAT_ProductionDay,
										spCBMS->SBRM.BAT_ChargeNum[0],
										spCBMS->SBRM.BAT_ChargeNum[1],
										spCBMS->SBRM.BAT_ChargeNum[2],
										spCBMS->SBRM.BAT_Own,
										spCBMS->SBRM.Reserve,
										spCBMS->SBRM.VIN[0],
										spCBMS->SBRM.VIN[1],
										spCBMS->SBRM.VIN[2],
										spCBMS->SBRM.VIN[3],
										spCBMS->SBRM.VIN[4],
										spCBMS->SBRM.VIN[5],
										spCBMS->SBRM.VIN[6],
										spCBMS->SBRM.VIN[7],
										spCBMS->SBRM.VIN[8],
										spCBMS->SBRM.VIN[9],
										spCBMS->SBRM.VIN[10],
										spCBMS->SBRM.VIN[11],
										spCBMS->SBRM.VIN[12],
										spCBMS->SBRM.VIN[13],
										spCBMS->SBRM.VIN[14],
										spCBMS->SBRM.VIN[15],
										spCBMS->SBRM.VIN[16]);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif
					break;
				case BCP://动力蓄电池充电参数
					CBMSinfo->flag_update = 1;
					CBMSinfo->CurrentIn_Max = spCBMS->SBCP.ChargeCurrent_Max * 0.1 - 400;	//A
					CBMSinfo->VoltageIn_Max = spCBMS->SBCP.ChargeVoltage_Max * 0.1;			//V
					CBMSinfo->VoltageTotal_Now = spCBMS->SBCP.BAT_Voltage * 0.1;			//V

#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"ChargeVoltage_CellMax %x.\n"
										"ChargeCurrent_Max %x.\n"
										"BAT_NominalPower %x.\n"
										"ChargeVoltage_Max %x.\n"
										"Temperature_Max %x.\n"
										"SOC %x.\n"
										"BAT_Voltage %x.\n",
										count,
										spCBMS->SBCP.ChargeVoltage_CellMax,
										spCBMS->SBCP.ChargeCurrent_Max,
										spCBMS->SBCP.BAT_NominalPower,
										spCBMS->SBCP.ChargeVoltage_Max,
										spCBMS->SBCP.Temperature_Max,
										spCBMS->SBCP.SOC,
										spCBMS->SBCP.BAT_Voltage);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case CTS://充电机发送时间同步信息

#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"Time_Sync[0] %x.\n"
										"Time_Sync[1] %x.\n"
										"Time_Sync[2] %x.\n"
										"Time_Sync[3] %x.\n"
										"Time_Sync[4] %x.\n"
										"Time_Sync[5] %x.\n"
										"Time_Sync[6] %x.\n"
										"Time_Sync[6] %x.\n",
										count,
										spCBMS->SCTS.Time_Sync[0],
										spCBMS->SCTS.Time_Sync[1],
										spCBMS->SCTS.Time_Sync[2],
										spCBMS->SCTS.Time_Sync[3],
										spCBMS->SCTS.Time_Sync[4],
										spCBMS->SCTS.Time_Sync[5],
										spCBMS->SCTS.Time_Sync[6]);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case CML://充电机最大输出能力
					CBMSinfo->flag_update = 1;
					CBMSinfo->VoltageOut_Max = spCBMS->SCML.VoltageOutput_Max * 0.1;		//V
					CBMSinfo->VoltageOut_Min = spCBMS->SCML.VoltageOutput_Min * 0.1;		//V
					CBMSinfo->CurrentOut_Max = spCBMS->SCML.CurrentOutput_Max * 0.1 - 400;	//A
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"VoltageOutput_Max %x.\n"
										"VoltageOutput_Min %x.\n"
										"CurrentOutput_Max %x.\n",
										count,
										spCBMS->SCML.VoltageOutput_Max,
										spCBMS->SCML.VoltageOutput_Min,
										spCBMS->SCML.CurrentOutput_Max);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case BRO://电池充电准备就绪状态
					CBMSState->flag_update = 1;
					if(spCBMS->SBRO.IsBMSReady == BMS_Notready)			//未准备好
					{
						CBMSState->flag_BMSReady = 0;
					}else if(spCBMS->SBRO.IsBMSReady == BMS_Ready)		//准备就绪
					{
						CBMSState->flag_BMSReady = 1;
					}else
					{
						CBMSState->flag_BMSReady = 2;					//无效
					}
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"IsBMSReady %x.\n",
										count,
										spCBMS->SBRO.IsBMSReady);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case CRO://充电机输出准备就绪状态
					CBMSState->flag_update = 1;
					if(CBMSState->flag_ChargerReady == Charger_Notready)		//未准备好
					{
						CBMSState->flag_ChargerReady = 0;
					}else if(CBMSState->flag_ChargerReady == Charger_Ready)		//准备就绪
					{
						CBMSState->flag_ChargerReady = 1;
					}else
					{
						CBMSState->flag_ChargerReady = 2;						//无效
					}
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"IsChargerRaedy %x.\n",
										count,
										spCBMS->SCRO.IsChargerRaedy);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case BCL://电池充电需求
					CBMSState->flag_update = 1;
					CBMSState->Voltage_Need = spCBMS->SBCL.Voltage_Requirement * 0.1;		//V
					CBMSState->Current_Need = spCBMS->SBCL.Current_Requirement * 0.1 - 400;	//A
					CBMSState->flag_Mode = spCBMS->SBCL.Charge_Mode;						//0x01:恒压 0x02：恒流
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"Voltage_Requirement %x.\n"
										"Current_Requirement %x.\n"
										"Charge_Mode %x.\n",
										count,
										spCBMS->SBCL.Voltage_Requirement,
										spCBMS->SBCL.Current_Requirement,
										spCBMS->SBCL.Charge_Mode);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif
					break;
				case BCS://电池充电总状态
					CBMSinfo->flag_update = 1;
					CBMSinfo->Voltage_Measure = spCBMS->SBCS.Voltage_Measured * 0.1;		//V
					CBMSinfo->Current_Measure = spCBMS->SBCS.Current_Measured * 0.1 - 400;	//A
					CBMSinfo->SOC = (int)spCBMS->SBCS.SOC;									//百分比
					CBMSinfo->Min_Remain = spCBMS->SBCS.Time_Remain;						//单位min
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"Voltage_Measured %x.\n"
										"Current_Measured %x.\n"
										"Cell_V_N %x.\n"
										"SOC %x.\n"
										"Time_Remain %x.\n",
										count,
										spCBMS->SBCS.Voltage_Measured,
										spCBMS->SBCS.Current_Measured,
										spCBMS->SBCS.Cell_V_N,
										spCBMS->SBCS.SOC,
										spCBMS->SBCS.Time_Remain);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case CCS://充电机充电状态
					CBMSState->flag_update = 1;
					CBMSState->Voltage_Output = spCBMS->SCCS.Voltage_Output * 0.1;			//V
					CBMSState->Current_Output = spCBMS->SCCS.Current_Output * 0.1 - 400;	//A
					CBMSState->Min_Charge = spCBMS->SCCS.Time_Cumulative;					//min
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"Voltage_Output %x.\n"
										"Current_Output %x.\n"
										"Time_Cumulative %x.\n",
										count,
										spCBMS->SCCS.Voltage_Output,
										spCBMS->SCCS.Current_Output,
										spCBMS->SCCS.Time_Cumulative);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case BSM://动力蓄电池状态信息
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"VHighestCell_Num %x.\n"
										"THighest %x.\n"
										"THighestCell_Num %x.\n"
										"TLowest %x.\n"
										"TLowestCell_Num %x.\n"
										"CellV_State %x.\n"
										"SOC %x.\n"
										"OverC_State %x.\n"
										"OverT_State %x.\n"
										"Insulation_State %x.\n"
										"Connect_State %x.\n"
										"Charge_Allow %x.\n",
										count,
										spCBMS->SBSM.VHighestCell_Num,
										spCBMS->SBSM.THighest,
										spCBMS->SBSM.THighestCell_Num,
										spCBMS->SBSM.TLowest,
										spCBMS->SBSM.TLowestCell_Num,
										spCBMS->SBSM.CellV_State,
										spCBMS->SBSM.SOC,
										spCBMS->SBSM.OverC_State,
										spCBMS->SBSM.OverT_State,
										spCBMS->SBSM.Insulation_State,
										spCBMS->SBSM.Connect_State,
										spCBMS->SBSM.Charge_Allow);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case BMV://单体动力蓄电池电压


					break;
				case BMT://动力蓄电池温度


					break;
				case BSP://动力蓄电池预留报文


					break;
				case BST://BMS中止充电
					//收到该指令时代表充电中止，此处同时可以结束系统的采样任务
					//(2016/7/5)此处有问题，因为该命令发送完毕后还会继续发送充电机确认报文，等于又继续开启
#ifdef XL902MeasureAuto_CBMS
//					if(XL902MeasureAuto_State == XL902MeasureAuto_Open)//此处用于接收到停止报文时避免继续检测握手，造成连接状态错误的现象
//					{
//						XL902MeasureAuto_State = XL902MeasureAuto_Close;
//						if((AutoModeCheck()) == 1)
//						{
//							if(XL902MeasureAutoSet_Lock == 1)
//							{
//								XL902MeasureAutoSet_Lock = 0;
//								//停止计量动作
//								XL902MeasureState_Set(XL902MeasureStop);
//							}
//						}
//					}

					if((AutoModeCheck()) == 1)
					{
						if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
						{
							XL902MeasureAuto_State = XL902MeasureAuto_Close;
							if(XL902MeasureAutoSet_Lock == 1)
							{
								XL902MeasureAutoSet_Lock = 0;
								//停止计量动作
								XL902MeasureState_Set(XL902MeasureStop);
							}
						}
					}else
					{
						if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
						{
							XL902MeasureAuto_State = XL902MeasureAuto_Close;
						}
					}

#endif
					CBMSState->flag_Link = 0;
					CBMSErr->flag_update = 1;
					CBMSErr->BMSStop_Cause = spCBMS->SBST.BMSStopCharge;
					CBMSErr->BMSStopFault_Cause = spCBMS->SBST.BMSStopChargeFault;
					CBMSErr->BMSStopErr_Carse = spCBMS->SBST.BMSStopChargeErr;
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"BMSStopCharge %x.\n"
										"BMSStopChargeFault %x.\n"
										"BMSStopChargeErr %x.\n",
										count,
										spCBMS->SBST.BMSStopCharge,
										spCBMS->SBST.BMSStopChargeFault,
										spCBMS->SBST.BMSStopChargeErr);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case CST://充电机中止充电
					//收到该指令时代表充电中止，此处同时可以结束系统的采样任务
					//(2016/7/5)此处有问题，因为该命令发送完毕后还会继续发送充电机确认报文，等于又继续开启
#ifdef XL902MeasureAuto_CBMS
					if(XL902MeasureAuto_State == XL902MeasureAuto_Open)//此处用于接收到停止报文时避免继续检测握手，造成连接状态错误的现象
					{
						XL902MeasureAuto_State = XL902MeasureAuto_Close;
						if((AutoModeCheck()) == 1)
						{
							if(XL902MeasureAutoSet_Lock == 1)
							{
								XL902MeasureAutoSet_Lock = 0;
								//停止计量动作
								XL902MeasureState_Set(XL902MeasureStop);
							}
						}
					}

					if((AutoModeCheck()) == 1)
					{
						if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
						{
							XL902MeasureAuto_State = XL902MeasureAuto_Close;
							if(XL902MeasureAutoSet_Lock == 1)
							{
								XL902MeasureAutoSet_Lock = 0;
								//停止计量动作
								XL902MeasureState_Set(XL902MeasureStop);
							}
						}
					}else
					{
						if(XL902MeasureAuto_State == XL902MeasureAuto_Open)
						{
							XL902MeasureAuto_State = XL902MeasureAuto_Close;
						}
					}
#endif
					CBMSState->flag_Link = 0;
					CBMSErr->flag_update = 1;
					CBMSErr->CStop_Cause = spCBMS->SCST.ChargerStop;
					CBMSErr->CStopFault_Cause = spCBMS->SCST.ChargerFault;
					CBMSErr->CStopErr_Cause = spCBMS->SCST.ChargerErr;
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"ChargerStop %x.\n"
										"ChargerFault %x.\n"
										"ChargerErr %x.\n",
										count,
										spCBMS->SCST.ChargerStop,
										spCBMS->SCST.ChargerFault,
										spCBMS->SCST.ChargerErr);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case BSD://BMS统计数据
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"SOC %x.\n"
										"BATCellV_Min %x.\n"
										"BATCellV_Max %x.\n"
										"BATT_Min %x.\n"
										"BATT_Max %x.\n",
										count,
										spCBMS->SBSD.SOC,
										spCBMS->SBSD.BATCellV_Min,
										spCBMS->SBSD.BATCellV_Max,
										spCBMS->SBSD.BATT_Min,
										spCBMS->SBSD.BATT_Max);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case CSD://充电机统计数据
					CBMSinfo->flag_update = 1;
					CBMSinfo->Min_Charge = spCBMS->SCSD.Time_Cumulative;		//min
					CBMSinfo->Energy_Total = spCBMS->SCSD.Energy_Output * 0.1;	//KW.h
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"Time_Cumulative %x.\n"
										"Energy_Output %x.\n"
										"Charger_Num %x.\n",
										count,
										spCBMS->SCSD.Time_Cumulative,
										spCBMS->SCSD.Energy_Output,
										spCBMS->SCSD.Charger_Num);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case BEM://BMS错误报文
					CBMSErr->flag_update = 1;
					CBMSErr->Timeout0x00 = spCBMS->SBEM.Recieve_TimeOut1;
					CBMSErr->Timeout0xAA = spCBMS->SBEM.Recieve_TimeOut2;
					CBMSErr->Timeout_syn = spCBMS->SBEM.ChargerTSOUTTO;
					CBMSErr->Timeout_ready = spCBMS->SBEM.R_BeReady_TimeOut;
					CBMSErr->Timeout_state = spCBMS->SBEM.R_SOC_TimeOut;
					CBMSErr->Timeout_stop = spCBMS->SBEM.R_CStop_TimeOut;
					CBMSErr->Timeout_stat = spCBMS->SBEM.R_CInfo_TimeOut;

#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"Recieve_TimeOut1 %x.\n"
										"Recieve_TimeOut2 %x.\n"
										"ChargerTSOUTTO %x.\n"
										"R_BeReady_TimeOut %x.\n"
										"R_SOC_TimeOut %x.\n"
										"R_CStop_TimeOut %x.\n"
										"R_CInfo_TimeOut %x.\n",
										count,
										spCBMS->SBEM.Recieve_TimeOut1,
										spCBMS->SBEM.Recieve_TimeOut2,
										spCBMS->SBEM.ChargerTSOUTTO,
										spCBMS->SBEM.R_BeReady_TimeOut,
										spCBMS->SBEM.R_SOC_TimeOut,
										spCBMS->SBEM.R_CStop_TimeOut,
										spCBMS->SBEM.R_CInfo_TimeOut);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				case CEM://充电机错误报文
					CBMSErr->flag_update = 1;
					CBMSErr->Timeout_recognize = spCBMS->SCEM.R_Identify_TimeOut;
					CBMSErr->Timeout_param = spCBMS->SCEM.R_Parameter_TimeOut;
					CBMSErr->Timeout_readyC = spCBMS->SCEM.R_BeReady_TimeOut;
					CBMSErr->Timeout_stateC = spCBMS->SCEM.R_BATState_TimeOut;
					CBMSErr->Timeout_require = spCBMS->SCEM.R_BATRequire_TimeOut;
					CBMSErr->Timeout_stopC = spCBMS->SCEM.R_BMSStop_TimeOut;
					CBMSErr->Timeout_BMSStat = spCBMS->SCEM.R_BMSInfo_TimeOut;
#ifdef CBMS_DEBUG
					Length_s = sprintf(p_String,
										"Here%d.\n"
										"R_Identify_TimeOut %x.\n"
										"R_Parameter_TimeOut %x.\n"
										"R_BeReady_TimeOut %x.\n"
										"R_BATState_TimeOut %x.\n"
										"R_BATRequire_TimeOut %x.\n"
										"R_BMSStop_TimeOut %x.\n"
										"R_BMSInfo_TimeOut %x.\n",
										count,
										spCBMS->SCEM.R_Identify_TimeOut,
										spCBMS->SCEM.R_Parameter_TimeOut,
										spCBMS->SCEM.R_BeReady_TimeOut,
										spCBMS->SCEM.R_BATState_TimeOut,
										spCBMS->SCEM.R_BATRequire_TimeOut,
										spCBMS->SCEM.R_BMSStop_TimeOut,
										spCBMS->SCEM.R_BMSInfo_TimeOut);
					Uart0_Transmit(p_UART0_Data, p_String, Length_s);		//串口发送接收到的数据
#endif

					break;
				default:

					break;

			}

		}
	}
}


CBMS_state *CBMSState_get(void)
{
	return CBMSState;
}


CBMS_info *CBMSInfo_get(void)
{
	return CBMSinfo;
}


CBMS_ERR *CBMSErr_get(void)
{
	return CBMSErr;
}


/*
 * 函数：J1939数据帧转为应用层数据
 * 描述：	1.J1939数据帧转为应用层数据；
 * 接口参数：	PGNx			对应的PGN号，此处用内部编码，而非PGN的值
 * 			*Buff			接收到的数据缓存区地址，字节流
 * 			ByteNum			数据长度
 * 返回参数：	无
 * 备注：无
 *
 */
static void CBMSFrameConverted(CBMS_PGN PGNx, unsigned char *Buff, unsigned short ByteNum)
{
	short i = 0, j = 0;
	unsigned char byte1 = 0, byte2 = 0;
	unsigned char *pMyBuff = Buff;
	unsigned char Frame_Num = 0;

	switch(PGNx)
	{
		case PGN256:

			spCBMS->SCRM.recognize = Buff_Uchar_Get(pMyBuff, &i);//里面参数有问题
			spCBMS->SCRM.Charger_Num = Buff_Uchar_Get(pMyBuff, &i);
			for(j = 0; j < 6; j++)
			{
				spCBMS->SCRM.Charger_AreaCoding[j] = Buff_Uchar_Get(pMyBuff, &i);
			}
			break;

		case PGN512:
			spCBMS->SBRM.BMSCommunication_Ver[0] = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBRM.BMSCommunication_Ver[1] = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBRM.BMSCommunication_Ver[2] = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_Type = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_Capacity = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_Voltage = Buff_Ushort_Get(pMyBuff, &i);
			for(j = 0; j < 4; j++)
			{
				spCBMS->SBRM.BAT_Manufacturer[j] = Buff_Char_Get(pMyBuff, &i);
			}
			for(j = 0; j < 4; j++)
			{
				spCBMS->SBRM.BAT_SeriesNum[j] = Buff_Char_Get(pMyBuff, &i);
			}
			spCBMS->SBRM.BAT_ProductionYear = Buff_Char_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_ProductionMonth = Buff_Char_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_ProductionDay = Buff_Char_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_ChargeNum[0] = Buff_Char_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_ChargeNum[1] = Buff_Char_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_ChargeNum[2] = Buff_Char_Get(pMyBuff, &i);
			spCBMS->SBRM.BAT_Own = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBRM.Reserve = Buff_Uchar_Get(pMyBuff, &i);
			for(j = 0; j < 17; j++)
			{
				spCBMS->SBRM.VIN[j] = Buff_Uchar_Get(pMyBuff, &i);
			}

			break;
		case PGN1536:
			spCBMS->SBCP.ChargeVoltage_CellMax = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCP.ChargeCurrent_Max = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCP.BAT_NominalPower = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCP.ChargeVoltage_Max = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCP.Temperature_Max = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBCP.SOC = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCP.BAT_Voltage = Buff_Ushort_Get(pMyBuff, &i);

			break;
		case PGN1792:
			for(j = 0; j < 7; j++)
			{
				spCBMS->SCTS.Time_Sync[j] = Buff_Uchar_Get(pMyBuff, &i);
			}

			break;
		case PGN2048:
			spCBMS->SCML.VoltageOutput_Max = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCML.VoltageOutput_Min = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCML.CurrentOutput_Max = Buff_Ushort_Get(pMyBuff, &i);
			if(BMSVision == CBMSVision2015)
			{
				spCBMS->SCML.CurrentOutput_Min = Buff_Ushort_Get(pMyBuff, &i);
			}

			break;
		case PGN2304:
			spCBMS->SBRO.IsBMSReady = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN2560:
			spCBMS->SCRO.IsChargerRaedy = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN4096:
			spCBMS->SBCL.Voltage_Requirement = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCL.Current_Requirement = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCL.Charge_Mode = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN4352:
			spCBMS->SBCS.Voltage_Measured = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCS.Current_Measured = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCS.Cell_V_N = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBCS.SOC = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBCS.Time_Remain = Buff_Ushort_Get(pMyBuff, &i);

			break;
		case PGN4608:
			spCBMS->SCCS.Voltage_Output = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCCS.Current_Output = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCCS.Time_Cumulative = Buff_Ushort_Get(pMyBuff, &i);
			if(BMSVision == CBMSVision2015)
			{
				byte1 = Buff_Ushort_Get(pMyBuff, &i);
				spCBMS->SCCS.ChargeAllow = (byte1 >> 0) & 0x03;
			}

			break;
		case PGN4864:
			spCBMS->SBSM.VHighestCell_Num = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSM.THighest = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSM.THighestCell_Num = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSM.TLowest = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSM.TLowestCell_Num = Buff_Uchar_Get(pMyBuff, &i);
			//以下为位域操作，先去除数据
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSM.CellV_State = (byte1 >> 0) & 0x03;
			spCBMS->SBSM.SOC = (byte1 >> 2) & 0x03;
			spCBMS->SBSM.OverC_State = (byte1 >> 4) & 0x03;
			spCBMS->SBSM.OverT_State = (byte1 >> 6) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSM.Insulation_State = (byte1 >> 0) & 0x03;
			spCBMS->SBSM.Connect_State = (byte1 >> 2) & 0x03;
			spCBMS->SBSM.Charge_Allow = (byte1 >> 4) & 0x03;

			break;
		case PGN5376:
			for(j = 0; j < ByteNum; j++)
			{
				spCBMS->SBMV.CellBAT_V[j] = Buff_Ushort_Get(pMyBuff, &i);
			}

			break;
		case PGN5632:
			for(j = 0; j < ByteNum; j++)
			{
				spCBMS->SBMT.CellBAT_T[j] = Buff_Uchar_Get(pMyBuff, &i);
			}

			break;
		case PGN5888:
			for(j = 0; j < ByteNum; j++)
			{
				spCBMS->SBSP.BAT_Reserve[j] = Buff_Uchar_Get(pMyBuff, &i);
			}

			break;
		case PGN6400:
			spCBMS->SBST.BMSStopCharge = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBST.BMSStopChargeFault = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBST.BMSStopChargeErr = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN6656:
			spCBMS->SCST.ChargerStop = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SCST.ChargerFault = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCST.ChargerErr = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN7168:
			spCBMS->SBSD.SOC = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSD.BATCellV_Min = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBSD.BATCellV_Max = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SBSD.BATT_Min = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBSD.BATT_Max = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN7424:
			spCBMS->SCSD.Time_Cumulative = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCSD.Energy_Output = Buff_Ushort_Get(pMyBuff, &i);
			spCBMS->SCSD.Charger_Num = Buff_Uchar_Get(pMyBuff, &i);

			break;
		case PGN7680:
			//以下为位域操作，先取出数据
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBEM.Recieve_TimeOut1 = (byte1 >> 0) & 0x03;
			spCBMS->SBEM.Recieve_TimeOut2 = (byte1 >> 2) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBEM.ChargerTSOUTTO = (byte1 >> 0) & 0x03;
			spCBMS->SBEM.R_BeReady_TimeOut = (byte1 >> 2) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBEM.R_SOC_TimeOut = (byte1 >> 0) & 0x03;
			spCBMS->SBEM.R_CStop_TimeOut = (byte1 >> 2) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SBEM.R_CInfo_TimeOut = (byte1 >> 0) & 0x03;

			break;
		case PGN7936:
			//以下为位域操作，先取出数据
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SCEM.R_Identify_TimeOut = (byte1 >> 0) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SCEM.R_Parameter_TimeOut = (byte1 >> 0) & 0x03;
			spCBMS->SCEM.R_BeReady_TimeOut = (byte1 >> 2) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SCEM.R_BATState_TimeOut = (byte1 >> 0) & 0x03;
			spCBMS->SCEM.R_BATRequire_TimeOut = (byte1 >> 2) & 0x03;
			spCBMS->SCEM.R_BMSStop_TimeOut = (byte1 >> 4) & 0x03;
			byte1 = Buff_Uchar_Get(pMyBuff, &i);
			spCBMS->SCEM.R_BMSInfo_TimeOut = (byte1 >> 0) & 0x03;

			break;

		default:
			break;
	}
	//将缓存区清零
	memset(Buff, 0, ByteNum);


}

/*
 * 函数：缓存中提取无符号整型数据
 * 描述：	1.将char型的缓存中的前四个字节转换为无符号整型，同时对其指针偏移进行处理；
 * 		2.内部
 * 接口参数：	*Buff 			需要提取的缓存
 * 			*i				数组偏移量指针
 * 返回参数：	unsigned int	返回的无符号整型
 * 备注：
 *
 */
static unsigned int Buff_Uint_Get(unsigned char *Buff, short *i)
{
	unsigned int Data;
	//大端
	Data = ((unsigned int)Buff[(*i)++]);
	Data |= ((unsigned int)Buff[(*i)++] << 8);
	Data |= ((unsigned int)Buff[(*i)++] << 16);
	Data |= ((unsigned int)Buff[(*i)++] << 24);

	return Data;
}
/*
 * 函数：缓存中提取无符号短整型数据
 * 描述：	1.将char型的缓存中的前两个字节转换为无符号短整型，同时对其指针偏移进行处理；
 * 		2.内部
 * 接口参数：	*Buff 			需要提取的缓存
 * 			*i				数组偏移量指针
 * 返回参数：	unsigned short	返回的无符号短整型
 * 备注：
 *
 */
static unsigned short Buff_Ushort_Get(unsigned char *Buff, short *i)
{
	unsigned short Data;
	//大端
	Data = ((unsigned short)Buff[(*i)++]);
	Data |= ((unsigned short)Buff[(*i)++] << 8);

	return Data;
}
/*
 * 函数：缓存中提取无符号字符型数据
 * 描述：	1.将char型的缓存中的前一个字节转换为无符号字符型，同时对其指针偏移进行处理；
 * 		2.内部
 * 接口参数：	*Buff 			需要提取的缓存
 * 			*i				数组偏移量指针
 * 返回参数：	unsigned char	返回的无符号字符型
 * 备注：
 *
 */
static unsigned char Buff_Uchar_Get(unsigned char *Buff, short *i)
{
	unsigned char Data;

	Data = ((unsigned char)Buff[(*i)++]);

	return Data;
}
/*
 * 函数：缓存中提取字符型型数据
 * 描述：	1.将char型的缓存中的前一个字节转换为字符型，同时对其指针偏移进行处理；
 * 		2.内部
 * 接口参数：	*Buff 			需要提取的缓存
 * 			*i				数组偏移量指针
 * 返回参数：	char			返回的字符型
 * 备注：
 *
 */
static char Buff_Char_Get(unsigned char *Buff, short *i)
{
	char Data;

	Data = ((char)Buff[(*i)++]);

	return Data;
}

/*
 * 函数：提取PGN内部编码
 * 描述：	1.对CAN_RX_BUF结构的结构体提取其对应的内部编码，该编码用于后期处理；
 * 		2.内部
 * 接口参数：	PGNIn 输入的PGN号
 * 返回参数：	CBMS_PGN格式的内部编号，用于作为数组入口
 * 备注：
 *
 */
static CBMS_PGN PGN_Get(unsigned int PGNIn)
{
	unsigned int Buff = 0;
	short i;

	for(i = 0; i < PGN_NUM; i++)
	{
		if(PGNIn == CBMS_Unit[i].PGN)
		{
			break;
		}
	}

	return (CBMS_PGN)i;
}

static void TestData_ini(void)
{

//	CBMSState->flag_Link = 0;
//	CBMSState->flag_Mode = 0;
//	CBMSState->Voltage_Need = 200;
//	CBMSState->Current_Need = 20;
//	CBMSState->Voltage_Output = 200;
//	CBMSState->Current_Output = 20;
//	CBMSState->Min_Charge = 200;
//
//	CBMSinfo->CurrentIn_Max = 400;
//	CBMSinfo->VoltageIn_Max = 750;
//	CBMSinfo->VoltageTotal_Now = 500;
//	CBMSinfo->VoltageOut_Max = 750;
//	CBMSinfo->VoltageOut_Min = 10;
//	CBMSinfo->CurrentOut_Max = 400;
//	CBMSinfo->Voltage_Measure = 200;
//	CBMSinfo->Current_Measure = 200;
//	CBMSinfo->SOC = 50;
//	CBMSinfo->Min_Remain = 60;
//	CBMSinfo->Min_Charge = 60;
//	CBMSinfo->Energy_Total = 10;

	CBMSState->flag_Link = 0;
	CBMSState->flag_Mode = 0;
	CBMSState->Voltage_Need = 0;
	CBMSState->Current_Need = 0;
	CBMSState->Voltage_Output = 0;
	CBMSState->Current_Output = 0;
	CBMSState->Min_Charge = 0;

	CBMSinfo->CurrentIn_Max = 0;
	CBMSinfo->VoltageIn_Max = 0;
	CBMSinfo->VoltageTotal_Now = 0;
	CBMSinfo->VoltageOut_Max = 0;
	CBMSinfo->VoltageOut_Min = 0;
	CBMSinfo->CurrentOut_Max = 0;
	CBMSinfo->Voltage_Measure = 0;
	CBMSinfo->Current_Measure = 0;
	CBMSinfo->SOC = 0;
	CBMSinfo->Min_Remain = 0;
	CBMSinfo->Min_Charge = 0;
	CBMSinfo->Energy_Total = 0;


}

void CBMSVision_Set(CBMSVision vision)
{
	BMSVision = vision;
}

void CBMSinfo_flag_Link_UnSet(void)
{
	CBMSState->flag_Link = 0;
	XL902MeasureAutoSet_Lock = 0;
}


#ifdef XL902MeasureAuto_CBMS
void XL902MeasureAutoState_Set(XL902MeasureAutoState state)
{
	XL902MeasureAuto_State = state;
//	if(state == XL902MeasureAuto_Close)
//	{
//		XL902MeasureAutoSet_Lock = 0;
//	}
}
#endif



