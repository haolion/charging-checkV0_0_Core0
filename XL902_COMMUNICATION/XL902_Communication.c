/*
 * 功能：XL902串口协议解析
 * 版本：V1.0
 * 描述：运行于XL803M-609V1.0信号板平台上
 * 公司：深圳市星龙科技股份有限公司
 * 作者：张颖豪
 * 日期：2015/5/28
 * 备注：无
 */
#include "XL902_Communication.h"

static float* WaveTestData;
static char RipperDataSendBack = 0;//用于调用纹波读取时判断是进行采样还是向上位机发送数据
static char /*flag_EPOEN = 0,*/flag_EPIEN = 0;//默认情况下电能脉冲输出输入都不使能
section("sdram_bank0") static unsigned char tmpbuf[1024*400];		//300K的缓存长度
static unsigned int jj;
static ERROR_CODE FLASHResult = NO_ERR;
static char flag_RunAuto;


static void downSoftAck(unsigned short FlagRespond, unsigned short FlagRight, unsigned short FlameNum);

void XL902_Communication_ini(void)
{
	int testi;
	//从FLASH中读出
	FLASHResult = pc28f128P33_Read((unsigned short*)&Command_Q_Send, FLASH_Param_ADDR, (sizeof(Command_Param))/2);
	if(FLASHResult != NO_ERR)
	{
		Command_Q_Send.flag_first = 0;
		FLASHResult = NO_ERR;
	}


	testi = sizeof(Command_Param);

	if(Command_Q_Send.flag_first != (char)0x22)//此时表示FLASH 中未初始化
	{
		Command_Q_Send.flag_first = 0x22;
		Command_Q_Send.Command_type = Frame_ERR;
		Command_Q_Send.SAMRATE = 16.7;
		Command_Q_Send.CONST = 3600;
		Command_Q_Send.CURRENTGEAR = 0;
		Command_Q_Send.CURRENTRANGE = 200;
		Command_Q_Send.CHARGETYPE = 0;
		Command_Q_Send.LOADTYPE = 0;
		Command_Q_Send.MEASUREMODE = 0;
		Command_Q_Send.MEASURETYPE = 0;
		Command_Q_Send.METERCONST = 3600;
		Command_Q_Send.PRICE = 1;
		memcpy(Command_Q_Send.PRODUCT,"AOTEXUN",strlen("AOTEXUN")+1);
		Command_Q_Send.VOLTAGEGEAR = 0;
		Command_Q_Send.VOLTAGERANGE = 750;
		Command_Q_Send.CURRENT_L1 = 31.4;
		Command_Q_Send.VOLTAGE_L1 = 750;
		Command_Q_Send.ENERGY_L1 = 0.024;
		Command_Q_Send.PE = 0;
		Command_Q_Send.FE = 0;
		//写入FLASH
		FLASHResult = FlashData_write(FLASH_Param_ADDR, FLASH_Param_RP_ORRSET, (unsigned short*)&Command_Q_Send,
				(sizeof(Command_Param))/2);
		if(FLASHResult != NO_ERR)
		{
			Command_Q_Send.flag_first = 0;
			FLASHResult = NO_ERR;
		}
	}

	if(Command_Q_Send.MEASURETYPE == 0)
	{
		flag_RunAuto = 1;//此时代表自动模式
	}else
	{
		flag_RunAuto = 0;//此时代表非自动模式
	}

	Command_Length_arrange(Command_Str, Command_arrange, Command_Num);		//从长指令到短指令进行排列，方便下面判断
	Command_Length_arrange(XL902CommandSP, XL902CommandSP_arrange, XL902CommandSP_num);


	//test
	WaveTestData = heap_calloc(3, 256, sizeof(float));
	for(int i = 0; i < 256; i++)
	{
		WaveTestData[i] = 10*sin(2*Pi*i/256);
	}

}



/*! Enters critical region */
#define ENTER_CRITICAL_REGION()  (adi_osal_EnterCriticalRegion())
/*! Exit critical region */
#define EXIT_CRITICAL_REGION()   (adi_osal_ExitCriticalRegion())
//获取帧数据，此处获取到的是串口传来的帧数据，假如利用网口进行传输，可适当修改该函数
void UART_Frame_Get(void* ConsumerArg)
{
	OS_ERR err;
	SerialReceiveMode mode;
	SoftwareDownloadFrame *DownloadFrame;
    UART_Data_Struct *Rx_Data;
    static int framlen;
    static bool loadStarted = 0;//1表示下载过程中
    static unsigned char crc16;
    unsigned short *pint16u;
    unsigned char *pint8u;
    int i, j, k;
    Rx_Data = ConsumerArg;
//    static char cominucationMode = 0;//默认ASCII码形式
#ifdef Debug_Hao
    printf("UART_Frame_Get has be ready.\n");
#endif
    while(1)
    {
    	if( Rx_Data->flag )
    	{
    		mode = ReceiveMode_Get();
    		//加入程序下载兼容条件判断
    		if((mode == FreeMode) || (mode == ASCIIMode))
    		{
        		while( Rx_Data->flag )		//超时判断
        		{
        		 	Rx_Data->flag = 0;
        			OSTimeDlyHMSM(0u,0u,0u,100u,OS_OPT_TIME_HMSM_STRICT,&err);

        		}
        		*( Rx_Data->p_Buff_Write++ ) = '#';	//置结束位，方便后面程序判断
        		if(Rx_Data->p_Buff_Write > Rx_Data->p_Buff_end)
        		{
        			Rx_Data->flag_Buff_Over = 1;
        		}
        		Rx_Data->Num_Frame++;		//帧数量加1
        		ReceiveMode_Set(FreeMode);
        		//发送信号量
        		OSSemPost(&g_Sem_Frame, OS_OPT_POST_1, &err);
    		}else if(mode == SoftwareDownloaded)
    		{
				ReceiveMode_Set(FreeMode);//设置会空闲模式，串口可以接收下一帧
    			j = 0;
    			DownloadFrame = SoftwareDownloadFrameGet_p();
    			pint16u = (unsigned short*)DownloadFrame->Buff;
    			pint8u = DownloadFrame->Buff;
    			pint8u += 2;
    			if(*pint16u == 0x1975)//开始下载,做一些处理，比如提示信息、终止别的线程等等
    			{
    				//开始下载时停止采样任务，避免过程出错
    				XL902MeasureState_Set(XL902MeasureStop);
    				crc16 = 0;
    				framlen = *(++pint16u);
					if(framlen==0)		//老版本下发的是0
						framlen=128;
					loadStarted = 1;	//表示下载过程中
					crc16 = 0;
					jj = 0;	//长度计数
					//应答
					OSTimeDly(500u,OS_OPT_TIME_DLY,&err);
					downSoftAck(0x1975,0x0000,0);//应答
    			}else if(*pint16u == 0x4567)//下载数据
    			{

    				if(loadStarted)
    				{
    					crc16 = 0;
    					crc16 += 0x45;
    					crc16 += 0x67;
    					crc16 += *(pint8u++);
    					crc16 += *(pint8u++);
    					k=((INT32U)(*(++pint16u)))*framlen;	//帧序号
    				}
					for(i = 0; i < framlen; i++)//先把数据写入内存
					{
						crc16 += *(pint8u);
						*(tmpbuf+k+i) = *(pint8u++);
					}
					j = *(pint8u++);
					j |= (*(pint8u++)) << 8;
					if(crc16 == (j))
					{
						downSoftAck(0x4567,0,*pint16u);
						jj += framlen;
					}
					else
					{

						downSoftAck(0x4567,1998,*pint16u);
					}
					//应答

    			}else if(*pint16u == 0x1013)//下载结束
    			{
					OSTimeDly(500u,OS_OPT_TIME_DLY,&err);
					downSoftAck(0x1013,0,crc16);//应答

					OSTimeDly(500u,OS_OPT_TIME_DLY,&err);

					FormatProgramDisk();
					jj=(unsigned int)((float)(jj)/2.0+0.5);
					if(pc28f128P33_Write((unsigned short*)&tmpbuf,FLASH_START_ADDR,jj)==NO_ERR)
					{
						*pREG_RCU0_CTL |= 0x01;	//重启

					}
					else
					{
						downSoftAck(0x4567,0x8000,crc16);//应答,写错误
						//VDK_Sleep(500);
						OSTimeDly(500u,OS_OPT_TIME_DLY,&err);
						loadStarted = 0;

					}

    			}
    		}

    	}
    	OSTimeDlyHMSM(0u,0u,0u,200u,OS_OPT_TIME_HMSM_STRICT,&err);		//挂起200ms

    }

}

//此处接口传输为符合帧结构的结构体，为处理通信、功能控制的主程序
void XL902_Communication(void* ConsumerArg)		//处理指令
{
	OS_ERR err;
	UART_Data_Struct *Frame_Communication;
	Type_Command Command;
//	printf("XL902_Communication has be ready.\n");
	Frame_Communication = ConsumerArg;
	XL902_Communication_ini();//初始化

	XL902PowerManagerIni();//电池电量检测的初始化
	XL902PowerLeft();//开机自己读取一次，因为总存在第一次读取错误的现象

#ifdef Debug_Hao
		Uart0_Transmit(Frame_Communication, "READY\n", strlen("READY\n"));
#endif
	while(1)
	{
		OSSemPend(&g_Sem_Frame,0u, OS_OPT_PEND_BLOCKING, NULL, &err);		//信号量,获取到帧数据时往下走
#ifdef Debug_Hao
		//printf("Receive a Frame.\n");
		Uart0_Transmit(Frame_Communication, "Receive a Frame.\n", strlen("Receive a Frame.\n"));
#endif
		while(Frame_Communication->Num_Frame != 0)
		{
			//此处避免某些特殊情况造成读指针超前写指针，使程序出问题。
			if(Frame_Communication->p_Buff_Read > Frame_Communication->p_Buff_Write)
			{
				if(Frame_Communication->p_Buff_Write - Frame_Communication->p_Buff_Start > 10)
				{
					Frame_Communication->Num_Frame = 0;
					Frame_Communication->p_Buff_Read = Frame_Communication->p_Buff_Write;
					break;
				}
			}
			Command = Command_Get( Frame_Communication );			//指令获取
			switch( Command )
			{
				case	Frame_ERR:
					Point_Move_To( Frame_Communication, '#' );					//帧以#结束
					//回复上位机
					Uart0_Transmit(Frame_Communication, "ERR;\r\n", strlen("ERR;\r\n"));
//					downSoftAck(0x1975,0x0000,0);//应答
	#ifdef Debug_Hao
					Uart0_Transmit(Frame_Communication, "Frame_ERR\n", strlen("Frame_ERR\n"));
	#endif
					break;
				case	WorkMode_Set:
					FuctionWorkMode_Set(Frame_Communication);
	//				OSSemPost(&g_Sem_7793_Run, OS_OPT_POST_1, &err);
					break;
				case	WorkMode_Read:
					FuctionWorkMode_Read(Frame_Communication);
					break;
				case	Param_Set:

					FuctionParam_Set(Frame_Communication);
					break;
				case	Param_Read:

					FuctionParam_Read(Frame_Communication);
					break;
				case	Range_Set://设置档位量程

					FuctionRange_Set(Frame_Communication);

					break;
				case	MeasureMode_Set:
					FuctionMMode_Set(Frame_Communication);
					break;
				case	Range_Read:
					FuctionRange_Read(Frame_Communication);

					break;
				case	Measurement_Read:
					FuctionM_Read(Frame_Communication);

					break;
				case	Measure_Begin:
					FuctionM_Begin(Frame_Communication);

					break;
				case	Measure_End:
					FuctionM_End(Frame_Communication);

					break;
				case	State_EMErr:
					FuctionState_EMErr(Frame_Communication);

					break;
				case	PulseOut_Set:

					break;
				case	PulseIn_Set:
					FuctionPulseIn_Set(Frame_Communication);
					break;
				case	Version:
					FuctionMI_Read(Frame_Communication);
					break;
				case	CalibrationFactor_Write:
					FuctionCF_Write(Frame_Communication);
					break;
				case	CalibrationFactor_Read:
					FuctionCF_Read(Frame_Communication);
					break;
				case ZeroCompensation_Write:
					FuctionZC_Write(Frame_Communication);
					break;
				case ZeroCompensation_Read:
					FuctionZC_Read(Frame_Communication);
					break;
				case	MI_Read:
					FuctionMI_Read(Frame_Communication);
					break;
				case	MI_Write:

					break;
				case	State_Read:
					FuctiongState_Read(Frame_Communication);
					break;
				case	Data_Read:
					FuctiongData_Read(Frame_Communication);
					break;
				case	Error_Read:
					FuctiongError_Read(Frame_Communication);
					break;
				case	BATS_Read:
	#ifdef Debug_Hao
					Uart0_Transmit(Frame_Communication, "RCS\n", strlen("RCS\n"));
	#endif
					FuctiongBATS_Read(Frame_Communication);

					break;
				case	Ripple_Read:
	#ifdef Debug_Hao
					Uart0_Transmit(Frame_Communication, "RRF\n", strlen("RRF\n"));
	#endif
					FuctiongRipple_Read(Frame_Communication);
					break;
				case	SwitchRange_Read:
					FuctiongSwitchRange_Read(Frame_Communication);
					break;
				case	wave_read:
					Fuctiongwave_Read(Frame_Communication);
					break;
				case	RippleCalibration_Write:
					FuctionRF_Write(Frame_Communication);
					break;
				case	RippleCalibration_Read:
					FuctionRF_Read(Frame_Communication);
					break;
				case	BMSVision_Set:
					FuctionBMSVision_Set(Frame_Communication);
					break;
				case	BMSAutoState_Set:
					FuctionBMSAutoState_Set(Frame_Communication);
					break;
				case	BoardReStart:
					FuctionBoardReStart(Frame_Communication);
					break;
				default:
					Point_Move_To( Frame_Communication, '#' );					//帧以#结束
					break;

			}

		}



	}

}


Type_Command Command_Get(UART_Data_Struct *Frame)
{
	Type_Command Command;
	char *p;
	int i;

	if(Frame->Num_Frame > 0)
	{
		Frame->Num_Frame--;
		if(Frame->p_Buff_Read > Frame->p_Buff_end)			//指针越界处理
		{
			Frame->p_Buff_Read = Frame->p_Buff_Start;

		}
////越界处理，此处因为增加缓存节点后缓存内存，可以取消异常处理
//		for(i = 0; i < Command_Length_Max; i++)							//假如指令刚好有一部分分别在链状结构的开头和结尾，此时为了防止误判断，
//		{        																//利用缓存数值进行缓存处理。
//			if((Frame->p_Buff_Read + i) > Frame->p_Buff_end)
//			{
//				Command_Buff[i] = *(Frame->p_Buff_Read + i - Frame->p_Buff_end + Frame->p_Buff_Start);
//
//			}else
//			{
//				Command_Buff[i] = *(Frame->p_Buff_Read + i);
//
//			}
//
//		}

		for(i = 0; i < Command_Num; i++)
		{
			p = Command_Str[ Command_arrange[i] ];
			if( Command_CMP(Frame->p_Buff_Read, p ) == 0)		//此处可能出现环形节点误判断
			{
				Frame->p_Buff_Read = Frame->p_Buff_Read + strlen( p );
				return (Type_Command)Command_arrange[ i ];

			}
		}
	}


	return Frame_ERR;
}


int XL902SPInstruct_Get(UART_Data_Struct *Frame)
{
	Type_Command Command;
	char *p;
	int i;

	for(i = 0; i < XL902CommandSP_num; i++)
	{
		p = XL902CommandSP[ XL902CommandSP_arrange[i] ];
		if( Command_CMP(Frame->p_Buff_Read, p ) == 0)
		{
			Frame->p_Buff_Read = Frame->p_Buff_Read + strlen( p );
			return XL902CommandSP_arrange[ i ];
		}
	}
	i = -1;

	return i;
}

//命令判断，假如源内存卡与目标字符串匹配，返回1，否则返回0
int Command_CMP(char *src, char *deststr)
{
	int r;
	r = memcmp(src, deststr, strlen(deststr));
	return r;
}


//将命令指令按照长度从长到短进行排列，此处为了配合上面的指令识别，指令由长度较长的开始判断，防止某些指令头部相同，长度不一样引起误判断
void Command_Length_arrange(char** src, char *array_arrange, int num)
{
	int i, k, l, j=0;
	char Length_arrange[num];
	for(i = 0; i < num; i++)
	{
		Length_arrange[i] = strlen( *(src+i) );

	}
	for(i = 0; i < num; i++)		//时间复杂度i*i
	{
		k = 0;
		for(j = 0;  j< num; j++)
		{
			if(Length_arrange[k] < Length_arrange[j])
			{
				k = j;
			}
		}
		Length_arrange[k] = 0;
		*(array_arrange + i) = k;
	}

}
//将指针移动到以Signal_End结束的下一个地址，加入异常处理
XL902_CResult Point_Move_To(UART_Data_Struct *Command_Frame, char Signal_End)
{
	char count = 0;

	if(Signal_End != '#')
	{
		while(*(Command_Frame->p_Buff_Read) != Signal_End)
		{
	//		Uart0_Transmit(Frame_Communication_Data, Frame_Communication_Data->p_Buff_Read, 1);
			Command_Frame->p_Buff_Read++;
			count ++;
			if((*(Command_Frame->p_Buff_Read) == '#') || (count > 100))
			{
				Command_Frame->p_Buff_Read -= count;
				return XL902CR_False;
			}

		}
		Command_Frame->p_Buff_Read += 1;
	}else
	{
		while(*(Command_Frame->p_Buff_Read) != Signal_End)
		{
	//		Uart0_Transmit(Frame_Communication_Data, Frame_Communication_Data->p_Buff_Read, 1);
			Command_Frame->p_Buff_Read++;
			count ++;
			if(count > 100)
			{
				Command_Frame->p_Buff_Read -= count;
				return XL902CR_False;
			}

		}
		Command_Frame->p_Buff_Read += 1;
	}


	return XL902CR_True;
}


void FuctionWorkMode_Set(UART_Data_Struct *Frame_Command)
{

	Point_Move_To( Frame_Command, '#' );					//帧以#结束

//	SwitchAutoState(SwitchAuto_Enable);						//开启自动档位控制

	//指令答复
	Uart0_Transmit(Frame_Command, "MTACK\r\n", strlen("MTACK\r\n"));

}

void FuctionWorkMode_Read(UART_Data_Struct *Frame_Command)
{
	int Str_Length;
	char Str_Buff[512];
	Str_Length = sprintf(Str_Buff,
			"RSM\r\n"
			"DCCHARGEMETER\r\n"
			"RSMACK\r\n"
			);

	Uart0_Transmit(Frame_Command, Str_Buff, Str_Length);
	Point_Move_To( Frame_Command, '#' );					//帧以#结束

}
//参数设定函数
void FuctionParam_Set(UART_Data_Struct *Frame_Command)
{
	Type_XL902CommandSP Command;
	XL902_CResult result;

	//提取参数
	Command_Q_Send.Command_type = Param_Set;
	result = Point_Move_To(Frame_Command, '\n');		//将指针移动到指令类型
	if(result == XL902CR_False)
	{
		return;
	}
	while(*Frame_Command->p_Buff_Read != '#')
	{
		Command = (Type_XL902CommandSP)XL902SPInstruct_Get(Frame_Command);
		result = Point_Move_To(Frame_Command, ';');		//将指针移动到指令类型
		if(result == XL902CR_False)
		{
			return;
		}
		switch(Command)
		{
		case Instruct_ERR:
			break;
		case SAMRATE:
			Command_Q_Send.SAMRATE = String_to_Float(Frame_Command->p_Buff_Read);
			break;
		case PRODUCT:
			strcpy(Command_Q_Send.PRODUCT, String_Get(Frame_Command->p_Buff_Read));
			break;
		case CHARGETYPE:
			Command_Q_Send.CHARGETYPE = String_to_Int(Frame_Command->p_Buff_Read);
			break;
		case LOADTYPE:
			Command_Q_Send.LOADTYPE = String_to_Int(Frame_Command->p_Buff_Read);
			break;
		case VOLTAGERANGE:
			Command_Q_Send.VOLTAGERANGE = String_to_Float(Frame_Command->p_Buff_Read);
			break;
		case CURRENTRANGE:
			Command_Q_Send.CURRENTRANGE = String_to_Float(Frame_Command->p_Buff_Read);
			break;
		case METERCONST:
			Command_Q_Send.METERCONST = String_to_Float(Frame_Command->p_Buff_Read);
			break;
		case CONST:
			Command_Q_Send.CONST = String_to_Int(Frame_Command->p_Buff_Read);
			break;
		case PRICE:
			Command_Q_Send.PRICE = String_to_Float(Frame_Command->p_Buff_Read);
			break;
		case MEASURETYPE:
			Command_Q_Send.MEASURETYPE = String_to_Int(Frame_Command->p_Buff_Read);
			//默认自动模式
			switch(Command_Q_Send.MEASURETYPE)
			{
				case 0:		//自动(BMS自动监听)
//					SwitchAutoState(SwitchAuto_Enable);
					flag_RunAuto = 1;
					break;
				case 1:		//手动(此时档位换挡也需要手动)
//					SwitchAutoState(SwitchAuto_Disable);
					flag_RunAuto = 0;
					break;
				case 2:		//半自动(没有BMS监听情况下使用)
//					SwitchAutoState(SwitchAuto_Enable);
					flag_RunAuto = 0;
					 break;
				default:
					break;

			}
			break;
		default:
			break;
		}
		result = Point_Move_To(Frame_Command, '\n');		//将指针移动到指令类型
		if(result == XL902CR_False)
		{
			return;
		}
	}
	Point_Move_To( Frame_Command, '#' );	//帧以#结束
//
//
//	Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
//	Command_Q_Send.SAMRATE = String_to_Float(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, '\n');		//将指针移动到指令类型
//	Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
//	strcpy(Command_Q_Send.PRODUCT, String_Get(Frame_Command->p_Buff_Read));
//	Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
//	Command_Q_Send.CHARGETYPE = String_to_Int(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
//	Command_Q_Send.LOADTYPE = String_to_Int(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
//	Command_Q_Send.VOLTAGERANGE = String_to_Float(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
//	Command_Q_Send.CURRENTRANGE = String_to_Float(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
//	Command_Q_Send.METERCONST = String_to_Float(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
//	Command_Q_Send.CONST = String_to_Int(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
//	Command_Q_Send.PRICE = String_to_Float(Frame_Command->p_Buff_Read);
//	Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
//	Command_Q_Send.MEASURETYPE = String_to_Int(Frame_Command->p_Buff_Read);
//	Point_Move_To( Frame_Command, '#' );	//帧以#结束

	//写入FLASH
	FLASHResult = FlashData_write(FLASH_Param_ADDR, FLASH_Param_RP_ORRSET, (unsigned short*)&Command_Q_Send,
			(sizeof(Command_Param))/2);
	if(FLASHResult != NO_ERR)
	{
		Command_Q_Send.flag_first = 0;
		FLASHResult = NO_ERR;
	}
	//回复上位机
	Uart0_Transmit(Frame_Command, "SPACK\r\n", strlen("SPACK\r\n"));

	//发送消息队列通知响应任务进行处理

}

void FuctionParam_Read(UART_Data_Struct *Frame_Command)
{
	int Str_Length;
	char Str_Buff[512];
	Str_Length = sprintf(Str_Buff,
			"RP\r\n"
			"SAMRATE;%f\r\n"
			"PRODUCT;%s\r\n"
			"CHARGETYPE;%d\r\n"
			"LOADTYPE;%d\r\n"
			"VOLTAGERANGE;%.1f\r\n"
			"CURRENTRANGE;%.1f\r\n"
			"METERCONST;%d\r\n"
			"CONST;%d\r\n"
			"PRICE;%.2f\r\n"
			"MEASURETYPE;%d\r\n"
			"RPACK\r\n",
			Command_Q_Send.SAMRATE,
			Command_Q_Send.PRODUCT,
			Command_Q_Send.CHARGETYPE,
			Command_Q_Send.LOADTYPE,
			Command_Q_Send.VOLTAGERANGE,
			Command_Q_Send.CURRENTRANGE,
			Command_Q_Send.METERCONST,
			Command_Q_Send.CONST,
			Command_Q_Send.PRICE,
			Command_Q_Send.MEASURETYPE);

	Point_Move_To( Frame_Command, '#' );

	Uart0_Transmit(Frame_Command, Str_Buff, Str_Length);

}

void FuctionRange_Set(UART_Data_Struct *Frame_Command)//设置量程
{
	char *pBuffGears_string;
	char XL902GearsNo;
	int i;
	XL902_CResult result;

	//提取参数
	Command_Q_Send.Command_type = Range_Set;

	result = Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
	if(result == XL902CR_True)
	{
		pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
		XL902GearsNo = XL902Gears_num;
		for(i = 0; i < XL902Gears_num; i++)
		{
			if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
			{
				XL902GearsNo = i;
				break;
			}
		}
		if(XL902GearsNo != XL902Gears_num)
		{
			Command_Q_Send.RANGE = XL902GearsNo;
			SwitchAutoState(SwitchAuto_Disable);	//自动转换到手动换挡

			//回复上位机
			Uart0_Transmit(Frame_Command, "SRACK", strlen("SRACK"));
			//根据通道和档位进行档位控制
			XL902SwitchDataSet_protect((XL902SWITCH_DATA)XL902GearsNo);

		}else
		{
			//错误信息打印
			Uart0_Transmit(Frame_Command, "XL902Gears_num ERR", strlen("XL902Gears_num ERR"));
		}

	}else
	{
		//错误信息打印
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}

	Point_Move_To( Frame_Command, '#' );					//帧以#结束

}
//
//void FuctionCRange_Set(UART_Data_Struct *Frame_Command)
//{
//	//提取参数
//	Command_Q_Send.Command_type = CurrentRange_Set;
//
//	Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
//	Command_Q_Send.CURRENTRANGE = String_to_Int(Frame_Command->p_Buff_Read);
//	Point_Move_To( Frame_Command, '#' );					//帧以#结束
//
//	//回复上位机
//	Uart0_Transmit(Frame_Command, "CRACK", strlen("CRACK"));
//
//	switch(Command_Q_Send.CURRENTRANGE)
//	{
//		case 400:
//			XL902SwitchData_set(I400A);
//			break;
//		case 200:
//			XL902SwitchData_set(I200A);
//			break;
//		case 80:
//			XL902SwitchData_set(I80A);
//			break;
//		default:
//			break;
//
//	}
//	//发送消息队列通知响应任务进行处理
//}

void FuctionRange_Read(UART_Data_Struct *Frame_Command)
{
	int Str_Length;
	char Str_Buff[512];
	XL902SWITCH_DATA ISwitch, USwitch;


	ISwitch = CurrentISwitchRead();
	USwitch = CurrentUSwitchRead();

	Str_Length = sprintf(Str_Buff,
			"RR\r\n"
			"RANGE;%s\r\n"
			"RANGE;%s\r\n"
			"RRACK\r\n",
			*(ppsXL902Gears + USwitch),
			*(ppsXL902Gears + ISwitch)
			);

	Point_Move_To( Frame_Command, '#' );

	Uart0_Transmit(Frame_Command, Str_Buff, Str_Length);
}

//void FuctionCRange_Read(UART_Data_Struct *Frame_Command)
//{
//	int Str_Length;
//	char Str_Buff[512];
//	Str_Length = sprintf(Str_Buff,
//			"RCR"
//			"CURRENTRANGE;%.1f\r\n"
//			"RCRACK\r\n",
//			Command_Q_Send.CURRENTRANGE
//			);
//
//	Point_Move_To( Frame_Command, '#' );
//
//	Uart0_Transmit(Frame_Command, Str_Buff, Str_Length);
//}

void FuctionMMode_Set(UART_Data_Struct *Frame_Command)
{
	XL902_CResult result;

	Command_Q_Send.Command_type = MeasureMode_Set;
	result = Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
	if(result == XL902CR_True)
	{
		Command_Q_Send.MEASURETYPE = String_to_Int(Frame_Command->p_Buff_Read);
		Point_Move_To( Frame_Command, '#' );					//帧以#结束

		switch(Command_Q_Send.MEASURETYPE)
		{
			case 0:		//自动
//				SwitchAutoState(SwitchAuto_Enable);
				break;
			case 1:		//手动
				SwitchAutoState(SwitchAuto_Disable);
				break;
			case 2:		//半自动

				 break;
			default:
				break;

		}

		//回复上位机
		Uart0_Transmit(Frame_Command, "MTACK\r\n", strlen("MTACK\r\n"));
	}else
	{
		//回复上位机
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}


	//发送消息队列通知响应任务进行处理
}
//读取测量值，当前只有4个参数，具体参数意义参照通信协议
void FuctionM_Read(UART_Data_Struct *Frame_Command)
{
	char Buff[6][15];
	char Buff_String[100];
	int String_Length;
	float test;
	float P, T;

	MeasureData_Get(&Command_Q_Send.VOLTAGE_L1, &Command_Q_Send.CURRENT_L1, &Command_Q_Send.ENERGY_L1, &P, &T);

	test = (float)EnergyPulse_Get();
	test = test / Command_Q_Send.METERCONST;

//	Float_to_StringEx(Command_Q_Send.VOLTAGE_L1, Buff[0]);
//	Float_to_StringEx(Command_Q_Send.CURRENT_L1, Buff[1]);
//	Float_to_StringEx(Command_Q_Send.ENERGY_L1, Buff[2]);
////	Float_to_StringEx(Command_Q_Send.PRICE, Buff[3]);
//	Float_to_StringEx(test, Buff[3]);

////test
//	Float_to_StringEx(230, Buff[0]);
//	Float_to_StringEx(230, Buff[1]);
//	Float_to_StringEx(229.98, Buff[2]);
////	Float_to_StringEx(Command_Q_Send.PRICE, Buff[3]);
//	Float_to_StringEx(10.02, Buff[3]);
//	Float_to_StringEx(101, Buff[4]);

#ifdef Debug_ComputerSoft
	Float_to_StringEx(750.0, Buff[0]);
	Float_to_StringEx(200.0, Buff[1]);
	Float_to_StringEx(229.98, Buff[2]);
	Float_to_StringEx(229.98, Buff[3]);
	Float_to_StringEx(101.0, Buff[4]);
	Float_to_StringEx(10000.0, Buff[5]);
//	Float_to_StringEx(60.0, Buff[6]);
//	Float_to_StringEx(229.98, Buff[7]);
//	Float_to_StringEx(1.02, Buff[8]);
#else
	Float_to_StringEx(Command_Q_Send.VOLTAGE_L1, Buff[0]);
	Float_to_StringEx(Command_Q_Send.CURRENT_L1, Buff[1]);
	Float_to_StringEx(Command_Q_Send.ENERGY_L1, Buff[2]);
	Float_to_StringEx(test, Buff[3]);
	Float_to_StringEx(T, Buff[4]);
	Float_to_StringEx(P, Buff[5]);
#endif

	String_Length = sprintf(Buff_String,
							"ME\r\n"
							"U1;%s\r\n"
							"I1;%s\r\n"
							"W1;%s\r\n"
							"W1C;%s\r\n"
							"Time;%s\r\n"
							"P1;%s\r\n"
							"MEACK\r\n",
							Buff[0],
							Buff[1],
							Buff[2],
							Buff[3],
							Buff[4],
							Buff[5]
							);
	Point_Move_To( Frame_Command, '#' );

	Uart0_Transmit(Frame_Command, Buff_String, String_Length);


}
void FuctionM_Begin(UART_Data_Struct *Frame_Command)
{

	Point_Move_To( Frame_Command, '#' );

	Uart0_Transmit(Frame_Command, "EBACK\r\n", strlen("EBACK\r\n"));
	//计量开始动作
	XL902MeasureAutoState_Set(XL902MeasureAuto_Open);
	XL902MeasureState_Set(XL902MeasureRunning);

}

void FuctionM_End(UART_Data_Struct *Frame_Command)
{

	Point_Move_To( Frame_Command, '#' );

	Uart0_Transmit(Frame_Command, "EEACK\r\n", strlen("EEACK\r\n"));
	//发送消息队列或者信号量通知响应任务进行处理
	XL902MeasureState_Set(XL902MeasureStop);
	XL902MeasureAutoState_Set(XL902MeasureAuto_Close);
	CBMSinfo_flag_Link_UnSet();

}

void FuctionState_EMErr(UART_Data_Struct *Frame_Command)
{

	char Buff[2][15];
	char Buff_String[100];
	int String_Length;

//	Float_to_StringEx(Command_Q_Send.PE, Buff[0]);
//	Float_to_StringEx(Command_Q_Send.FE, Buff[1]);

	Float_to_StringEx(0.001, Buff[0]);
	Float_to_StringEx(0.001, Buff[1]);

	String_Length = sprintf(Buff_String,
							"ES\r\n"
							"PE;%s\r\n"
							"TE;%s\r\n"
							"ESACK\r\n",
							Buff[0],
							Buff[1]
							);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);

	Point_Move_To( Frame_Command, '#' );


	//发送消息队列或者信号量通知响应任务进行处理

}

void FuctionPulseIn_Set(UART_Data_Struct *Frame_Command)
{

	char Buff[2][15];
	char Buff_String[100];
	int String_Length;
	XL902_CResult result;
	char state;

	result = Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
	if(result == XL902CR_True)
	{
		//脉冲输出控制标准，其它任务以该标准为开关
		flag_EPIEN = String_to_Int(Frame_Command->p_Buff_Read);
		if(flag_EPIEN == 0)		//关闭脉冲输入中断
		{
			EnergyPulseCount_Enable(0);
		}else					//打开脉冲输出中断
		{
			EnergyPulseCount_Enable(1);
		}
		String_Length = sprintf(Buff_String,
								"EPIACK\r\n"
								);
		Uart0_Transmit(Frame_Command, Buff_String, String_Length);
	}

	Point_Move_To( Frame_Command, '#' );

}

void FuctionBMSAutoState_Set(UART_Data_Struct *Frame_Command)
{

	char Buff[2][15];
	char Buff_String[100];
	int String_Length;
	XL902_CResult result;
	char state;

	result = Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
	if(result == XL902CR_True)
	{
		state = String_to_Int(Frame_Command->p_Buff_Read);
		if(state == 0)		//关闭BMS自动模式
		{
			CBMSinfo_flag_Link_UnSet();
			XL902MeasureAutoState_Set(XL902MeasureAuto_Close);
			//此时需要关闭采样，同时将链接修改为断开，不然假如充电机非常规关闭时，可能会导致第二次测出现问题
			XL902MeasureState_Set(XL902MeasureStop);



		}else				//打开BMS自动模式
		{
			XL902MeasureAutoState_Set(XL902MeasureAuto_Open);
		}
		String_Length = sprintf(Buff_String,
								"BSACK\r\n"
								);
		Uart0_Transmit(Frame_Command, Buff_String, String_Length);
	}

	Point_Move_To( Frame_Command, '#' );

}

void FuctionBoardReStart(UART_Data_Struct *Frame_Command)
{

	char Buff[2][15];
	char Buff_String[100];
	int String_Length;



	String_Length = sprintf(Buff_String,
								"BRSACK\r\n"
								);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);

	*pREG_RCU0_CTL |= 0x01;	//重启

	Point_Move_To( Frame_Command, '#' );

}


void FuctionBMSVision_Set(UART_Data_Struct *Frame_Command)
{

	char Buff[2][15];
	char Buff_String[100];
	int String_Length;
	XL902_CResult result;
	char state;
#ifdef BMS_Listen_Save
	int i;
	unsigned int MyPGN;
#endif

	result = Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
	if(result == XL902CR_True)
	{
		//版本选择
		flag_EPIEN = String_to_Int(Frame_Command->p_Buff_Read);
		if(flag_EPIEN == 1)		//表示选择新国标
		{
			CBMSVision_Set(CBMSVision2015);
#ifdef BMS_Listen_Save
			//此处目前用于保存监听BMS的数据
			//数组0存放接收到的数据个数
			Buf_BMS_Listen_Save[0].Mailbox_ID0 = Buf_BMS_Listen_Save_Count;
			//将数据存储到FLASH中
			//写入FLASH
			FLASHResult = FlashData_write(FLASH_BMSListen_ADDR, FLASH_BMSListen_ORRSET, (unsigned short*)Buf_BMS_Listen_Save,
					(sizeof(CAN_RX_BUF)) * Buf_BMS_Listen_Save_Count / 2);
			if(FLASHResult != NO_ERR)
			{
				FLASHResult = NO_ERR;
			}
#endif

		}else					//表示选择老国标
		{
			CBMSVision_Set(CBMSVision2011);

		}
		String_Length = sprintf(Buff_String,
								"BVSACK\r\n"
								);
		Uart0_Transmit(Frame_Command, Buff_String, String_Length);
	}

	Point_Move_To( Frame_Command, '#' );

}

void FuctionCF_Write(UART_Data_Struct *Frame_Command)
{
	ERROR_CODE Result = NO_ERR;
	COMMAND_STRUCT CmdBuffer;
	XL902SWITCH_RESULT XL902SwitchResult;
	char Buff_String[100];
	int String_Length;
	int mancode, devcode;
	int i;
	float fCFData[2];
	unsigned int testData[2];
	char *pBuffGears_string;
	char XL902GearsNo;
	XL902_CResult result;

	Command_Q_Send.Command_type = CalibrationFactor_Write;
	result = Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
	if(result == XL902CR_True)
	{
		if((strcmp(Password, String_Get(Frame_Command->p_Buff_Read)) == 0))
		{
			Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
			pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
			XL902GearsNo = XL902Gears_num;
			for(i = 0; i < XL902Gears_num; i++)
			{
				if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
				{
					XL902GearsNo = i;
					break;
				}
			}
			if(XL902GearsNo != XL902Gears_num)
			{
				Command_Q_Send.RANGE = XL902GearsNo;
			}else
			{
				//错误信息打印
				Uart0_Transmit(Frame_Command, "XL902Gears_num ERR\r\n", strlen("XL902Gears_num ERR\r\n"));
			}
			Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
			Command_Q_Send.A_ADJUST = String_to_Float(Frame_Command->p_Buff_Read);
			Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
			Command_Q_Send.Q_ADJUST = String_to_Float(Frame_Command->p_Buff_Read);

			//发送消息队列或者信号量通知响应任务进行处理

			//回复上位机
			Uart0_Transmit(Frame_Command, "ADWACK\r\n", strlen("ADWACK\r\n"));
		}else
		{
			//回复上位机
			Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));

		}

		//将获取的参数存进FLASH
		fCFData[0] = Command_Q_Send.A_ADJUST;
		fCFData[1] = Command_Q_Send.Q_ADJUST;

		XL902SwitchResult = XL902FlashCF_Write(Command_Q_Send.A_ADJUST,Command_Q_Send.Q_ADJUST, (XL902SWITCH_DATA)Command_Q_Send.RANGE);
	}else
	{
		//回复上位机
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}

	Point_Move_To( Frame_Command, '#' );					//帧以#结束

}

void FuctionCF_Read(UART_Data_Struct *Frame_Command)
{
	char Buff[2][15];
	char Buff_String[100];
	int String_Length;
	unsigned short i = 0, j = 0;
	COMMAND_STRUCT CmdBuffer;
	float fCFData[2];
	char *pBuffGears_string;
	char XL902GearsNo;
	XL902_CResult result;

	//获取通道及档位编号
	result = Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
	if(result == XL902CR_True)
	{
		pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
		XL902GearsNo = XL902Gears_num;
		for(i = 0; i < XL902Gears_num; i++)
		{
			if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
			{
				XL902GearsNo = i;
				break;
			}
		}
		if(XL902GearsNo != XL902Gears_num)
		{
			Command_Q_Send.RANGE = XL902GearsNo;



			//等待系数更新
			XL902FlashCF_Read(&Command_Q_Send.A_ADJUST, &Command_Q_Send.Q_ADJUST, (XL902SWITCH_DATA)Command_Q_Send.RANGE);

			Float_to_StringEx(Command_Q_Send.A_ADJUST, Buff[0]);
			Float_to_StringEx(Command_Q_Send.Q_ADJUST, Buff[1]);

			String_Length = sprintf(Buff_String,
									"ADR\r\n"
									"RANGE;%s\r\n"
									"L1:A;%s\r\n"
									"L1:Q;%s\r\n"
									"ADRACK\r\n",
									*(ppsXL902Gears + Command_Q_Send.RANGE),
									Buff[0],
									Buff[1]
										);
			Uart0_Transmit(Frame_Command, Buff_String, String_Length);

		}else
		{
			//错误信息打印
			Uart0_Transmit(Frame_Command, "XL902Gears_num ERR\r\n", strlen("XL902Gears_num ERR\r\n"));
		}
	}else
	{
		//错误信息打印
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}
	Point_Move_To( Frame_Command, '#' );					//帧以#结束
}


void FuctionZC_Write(UART_Data_Struct *Frame_Command)
{
	ERROR_CODE Result = NO_ERR;
	COMMAND_STRUCT CmdBuffer;
	XL902SWITCH_RESULT XL902SwitchResult;
	char Buff_String[100];
	int String_Length;
	int mancode, devcode;
	int i;
	char *pBuffGears_string;
	char XL902GearsNo;
	XL902_CResult result;

	Command_Q_Send.Command_type = ZeroCompensation_Write;
	result = Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
	if(result == XL902CR_True)
	{
		if((strcmp(Password, String_Get(Frame_Command->p_Buff_Read)) == 0))
		{
			Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
			pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
			XL902GearsNo = XL902Gears_num;
			for(i = 0; i < XL902Gears_num; i++)
			{
				if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
				{
					XL902GearsNo = i;
					break;
				}
			}
			if(XL902GearsNo != XL902Gears_num)
			{
				Command_Q_Send.RANGE = XL902GearsNo;
			}else
			{
				//错误信息打印
				Uart0_Transmit(Frame_Command, "XL902Gears_num ERR\r\n", strlen("XL902Gears_num ERR\r\n"));
			}
			Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
			Command_Q_Send.ZC = String_to_Float(Frame_Command->p_Buff_Read);

			//发送消息队列或者信号量通知响应任务进行处理

			//回复上位机
			Uart0_Transmit(Frame_Command, "ZDWACK\r\n", strlen("ZDWACK\r\n"));
		}else
		{
			//回复上位机
			Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));

		}

		//将获取的参数存进FLASH
		XL902SwitchResult = XL902FlashZC_Write(Command_Q_Send.ZC, (XL902SWITCH_DATA)Command_Q_Send.RANGE);

	}else
	{
		//回复上位机
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}

	Point_Move_To( Frame_Command, '#' );					//帧以#结束

}

void FuctionZC_Read(UART_Data_Struct *Frame_Command)
{
	char Buff[2][15];
	char Buff_String[100];
	int String_Length;
	unsigned short i = 0, j = 0;
	COMMAND_STRUCT CmdBuffer;
	float fCFData[2];
	char *pBuffGears_string;
	char XL902GearsNo;
	XL902_CResult result;

	//获取通道及档位编号
	result = Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
	if(result == XL902CR_True)
	{
		pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
		XL902GearsNo = XL902Gears_num;
		for(i = 0; i < XL902Gears_num; i++)
		{
			if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
			{
				XL902GearsNo = i;
				break;
			}
		}
		if(XL902GearsNo != XL902Gears_num)
		{
			Command_Q_Send.RANGE = XL902GearsNo;
		}else
		{
			//错误信息打印
			Uart0_Transmit(Frame_Command, "XL902Gears_num ERR\r\n", strlen("XL902Gears_num ERR\r\n"));
		}
		//等待系数更新
		XL902FlashZC_Read(&Command_Q_Send.ZC, (XL902SWITCH_DATA)Command_Q_Send.RANGE);

		Float_to_StringEx(Command_Q_Send.ZC, Buff[0]);

		String_Length = sprintf(Buff_String,
								"ZCDR\r\n"
								"RANGE;%s\r\n"
								"L1;%s\r\n"
								"ZCDRACK\r\n",
								*(ppsXL902Gears + Command_Q_Send.RANGE),
								Buff[0]
									);
		Uart0_Transmit(Frame_Command, Buff_String, String_Length);

	}else
	{
		//回复上位机
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}

	Point_Move_To( Frame_Command, '#' );					//帧以#结束

}

void FuctionMI_Read(UART_Data_Struct *Frame_Command)
{
	char Buff_String[200];
	int String_Length;
#ifdef BMS_Listen_Save
	int i;
	unsigned int MyPGN;

			//此处目前用于发送监听BMS的数据
			//从FLASH中读出全部需要读取的数量
			FLASHResult = pc28f128P33_Read((unsigned short*)Buf_BMS_Listen_Save, FLASH_BMSListen_ADDR,
					(sizeof(CAN_RX_BUF))/ 2);
			if(FLASHResult != NO_ERR)
			{
				Command_Q_Send.flag_first = 0;
				FLASHResult = NO_ERR;
			}
			Buf_BMS_Listen_Save_Count = Buf_BMS_Listen_Save[0].Mailbox_ID0;
			//根据上方得到的数量从FLASH中读出对应数据
			FLASHResult = pc28f128P33_Read((unsigned short*)Buf_BMS_Listen_Save, FLASH_BMSListen_ADDR,
					(sizeof(CAN_RX_BUF)) * Buf_BMS_Listen_Save_Count / 2);
			if(FLASHResult != NO_ERR)
			{
				Command_Q_Send.flag_first = 0;
				FLASHResult = NO_ERR;
			}

			for(i = 1; i < Buf_BMS_Listen_Save_Count; i++)
			{
				MyPGN = (unsigned int)(Buf_BMS_Listen_Save[i].Mailbox_ID1 & 0x00FF) << 8;//提取PGN
				String_Length = sprintf(Buff_String,
										"PGN:%d\t"
										"%x\t"
										"%x\t"
										"%x\t"
										"%x\t"
										"%x\t"
										"%x\r\n",
										MyPGN,
										Buf_BMS_Listen_Save[i].Mailbox_ID0,
										Buf_BMS_Listen_Save[i].Length,
										Buf_BMS_Listen_Save[i].Data0,
										Buf_BMS_Listen_Save[i].Data1,
										Buf_BMS_Listen_Save[i].Data2,
										Buf_BMS_Listen_Save[i].Data3
										);
				Uart0_Transmit(Frame_Command, Buff_String, String_Length);
			}
#endif
	//获取通道及档位编号
	Point_Move_To( Frame_Command, '#' );					//帧以#结束



	//发送模块信息
	String_Length = sprintf(Buff_String,
							"RMD\r\n"
							"SOFTVER;%s\r\n"
							"HARDVER;%s\r\n"
							"SN;%s\r\n"
							"PRODUCTIONDATA;%s\r\n"
							"DEBUGMAN;%s\r\n"
							"TESTMAN;%s\r\n"
							"CUSTOMER;%s\r\n"
							"RMDACK\r\n",
							"V1.0",
							"V1.0",
							"111111",
							__DATE__,
							"ZYH",
							"ZYH",
							"ZYH"
								);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);
}

void FuctiongState_Read(UART_Data_Struct *Frame_Command)
{
	CBMS_state *CBMSState;
	char Buff_String[250];
	int String_Length;

	//获取通道及档位编号
	Point_Move_To( Frame_Command, '#' );					//帧以#结束

	CBMSState = CBMSState_get();

	String_Length = sprintf(Buff_String,
							"RS\r\n"
							"STATE;%d\r\n"
							"CHARGEMODE;%d\r\n"
							"VOLTAGENEED;%f\r\n"
							"CURRENCENEED;%f\r\n"
							"VOLTAGEOUT;%f\r\n"
							"CURRENCEOUT;%f\r\n"
							"CHARGETIME;%f\r\n"
							"RSACK\r\n",
//							str_flagLink[CBMSState->flag_Link],
//							str_flagMode[CBMSState->flag_Mode],
							CBMSState->flag_Link,
							CBMSState->flag_Mode,
							CBMSState->Voltage_Need,
							CBMSState->Current_Need,
							CBMSState->Voltage_Output,
							CBMSState->Current_Output,
							CBMSState->Min_Charge
								);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);

}

void FuctiongData_Read(UART_Data_Struct *Frame_Command)
{
	CBMS_info *CBMSInfo;
	char Buff_String[250];
	int String_Length;

	//获取通道及档位编号
	Point_Move_To( Frame_Command, '#' );					//帧以#结束

	CBMSInfo = CBMSInfo_get();

	String_Length = sprintf(Buff_String,
							"RD\r\n"
							"CURRENCEINMAX;%f\r\n"
							"VOLTAGEINMAX;%f\r\n"
							"VOLTAGEBAT;%f\r\n"
							"VOLTAGEOUTMAX;%f\r\n"
							"VOLTAGEOUTMIN;%f\r\n"
							"CURRENCEOUTMAX;%f\r\n"
							"VOLTAGEMEASURE;%f\r\n"
							"CURRENCEMEASURE;%f\r\n"
							"SOC;%d\r\n"
							"TIMEREMAIN;%d\r\n"
							"TIMECHARGE;%d\r\n"
							"POWER;%d\r\n"
							"RDACK\r\n",
							CBMSInfo->CurrentIn_Max,
							CBMSInfo->VoltageIn_Max,
							CBMSInfo->VoltageTotal_Now,
							CBMSInfo->VoltageOut_Max,
							CBMSInfo->VoltageOut_Min,
							CBMSInfo->CurrentOut_Max,
							CBMSInfo->Voltage_Measure,
							CBMSInfo->Current_Measure,
							CBMSInfo->SOC,
							CBMSInfo->Min_Remain,
							CBMSInfo->Min_Charge,
							CBMSInfo->Energy_Total
								);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);
}


void FuctiongError_Read(UART_Data_Struct *Frame_Command)
{

	//获取通道及档位编号
	Point_Move_To( Frame_Command, '#' );					//帧以#结束
}

void FuctiongBATS_Read(UART_Data_Struct *Frame_Command)
{
	unsigned char buff[2];
	char Buff_String[250];
	int String_Length;
	unsigned char *pbuff;
	pbuff = buff;
	int BatLeft;

	//获取电池电量
	Point_Move_To( Frame_Command, '#' );					//帧以#结束
#ifndef Debug_ComputerSoft
	BatLeft = XL902PowerLeft();
#else
	BatLeft = 50;
#endif
	String_Length = sprintf(Buff_String,
							"RBAT\r\n"
							"LEFT;%d\r\n"
							"RBATACK\r\n",
							BatLeft
								);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);
}

void FuctiongRipple_Read(UART_Data_Struct *Frame_Command)
{
	char Buff[4][15];
	char Buff_String[250];
	int String_Length;
	XL902RWResult RWResult;
	float UPersent, IPersent, UA, IA;
	XL902MeasureState MeasureRunningState;

	MeasureRunningState = XL902MeasureState_Read();//获取当前是否在采样任务中，假如不在采样任务则不处理。
	if(MeasureRunningState == XL902MeasureRunning)
	{
		//可以采用相互调用的方法进行设计，等待纹波采集完毕后再调用回该函数进行信息发送
		if(RipperDataSendBack == 0)//==0时表示此时调用的目的是进行纹波采样
		{
			RipperDataSendBack = 1;
			//调用接口令纹波运行
			RWResult = XL902RWSample_run();
			if(RWResult == XL902RW_False)
			{
				//发送错误

			}
			Point_Move_To( Frame_Command, '#' );					//帧以#结束
		}else
		{
			RipperDataSendBack = 0;
			//获取转换之后的数据
			XL902RippleInf_get(&UPersent, &IPersent, &UA, &IA);
			//数据装换，此处暂时为测试数据,未测试
			Float_to_StringEx(UPersent, Buff[0]);
			Float_to_StringEx(UA, Buff[1]);
			Float_to_StringEx(IPersent, Buff[2]);
			Float_to_StringEx(IA, Buff[3]);

			String_Length = sprintf(Buff_String,
									"RRF\r\n"
									"L1RF;%s\r\n"
									"L1RA;%s\r\n"
									"L2RF;%s\r\n"
									"L2RA;%s\r\n"
									"RRFACK\r\n",
									Buff[0],
									Buff[1],
									Buff[2],
									Buff[3]
										);


			Uart0_Transmit(Frame_Command, Buff_String, String_Length);
		}
	}else
	{
		Point_Move_To( Frame_Command, '#' );					//帧以#结束
	}

}

void FuctiongSwitchRange_Read(UART_Data_Struct *Frame_Command)
{
	int *p_data;
	char Buff_String[250];
	int String_Length;

	//获取相应数据
	p_data = XL902SwitchRangeReand();
	//数据装换，此处暂时为测试数据,未测试

	Point_Move_To( Frame_Command, '#' );					//帧以#结束

	String_Length = sprintf(Buff_String,
							"RRA\r\n"
							"L1,1;%d\r\n"
							"L1,2;%d\r\n"
							"L2,1;%d\r\n"
							"L2,2;%d\r\n"
							"L2,3;%d\r\n"
							"RRAACK\r\n",
							p_data[0],
							p_data[1],
							p_data[2],
							p_data[3],
							p_data[4]
								);
	Uart0_Transmit(Frame_Command, Buff_String, String_Length);
}


void Fuctiongwave_Read(UART_Data_Struct *Frame_Command)
{
	int *p_data;
	char* pBuff_String1;
	char* pBuff_String2;
	int String_Length;
	XL902_CResult result;
	Type_XL902CommandSP Command;
	int i, j, k;
	char FirstTime = 1;
	float *URippleWave;//电压纹波指针
	float *IRippleWave;//电流纹波指针

	pBuff_String1 = heap_calloc(1, 20480, sizeof(float));
	if(pBuff_String1 == NULL)
	{
		result = Point_Move_To( Frame_Command, '#' );					//帧以#结束
		return;
	}
	pBuff_String2 = heap_calloc(1, 256, sizeof(float));
	if(pBuff_String2 == NULL)
	{
		result = Point_Move_To( Frame_Command, '#' );					//帧以#结束
		return;
	}

	//获取相应数据
	while(*Frame_Command->p_Buff_Read != '#')
	{
		result = Point_Move_To(Frame_Command, ';');		//将指针移动到指令类型
		if(result == XL902CR_False)
		{
			result = Point_Move_To( Frame_Command, '#' );					//帧以#结束
			return;
		}
		Command = (Type_XL902CommandSP)XL902SPInstruct_Get(Frame_Command);
		result = Point_Move_To(Frame_Command, '\n');		//将指针移动到指令类型
		if(result == XL902CR_False)
		{
			result = Point_Move_To( Frame_Command, '#' );					//帧以#结束
			return;
		}
		switch(Command)
		{
		case ChannalU1:
			WaveDataToString(WaveTestData, 256, pBuff_String1);
			if(FirstTime == 1)
			{
				FirstTime = 0;
				String_Length = sprintf(pBuff_String2,
										"KL\r\n"
											);
				Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);
			}
			String_Length = sprintf(pBuff_String2,
									"U1;\r\n"
										);
			Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);

			WaveDataSend(pBuff_String1);

			break;
		case ChannalI1:
			WaveDataToString(WaveTestData, 256, pBuff_String1);
			if(FirstTime == 1)
			{
				FirstTime = 0;
				String_Length = sprintf(pBuff_String2,
										"KL\r\n"
											);
				Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);
			}
			String_Length = sprintf(pBuff_String2,
									"I1;\r\n"
										);
			Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);

			WaveDataSend(pBuff_String1);

			break;
		case ChannalU1R:
			URippleWave = XL902RippleWaveGet_U(WaveDataPointNumSend);
			WaveDataToString(URippleWave, WaveDataPointNumSend, pBuff_String1);
			if(FirstTime == 1)
			{
				FirstTime = 0;
				String_Length = sprintf(pBuff_String2,
										"KL\r\n"
											);
				Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);
			}
			String_Length = sprintf(pBuff_String2,
									"U1R;\r\n"
										);
			Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);

			WaveDataSend(pBuff_String1);
			break;
		case ChannalI1R:
			IRippleWave = XL902RippleWaveGet_I(WaveDataPointNumSend);
			WaveDataToString(IRippleWave, WaveDataPointNumSend, pBuff_String1);
			if(FirstTime == 1)
			{
				FirstTime = 0;
				String_Length = sprintf(pBuff_String2,
										"KL\r\n"
											);
				Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);
			}
			String_Length = sprintf(pBuff_String2,
									"I1R;\r\n"
										);
			Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);
			WaveDataSend(pBuff_String1);
			break;
		default:
			return;
		}
	}

	String_Length = sprintf(pBuff_String2,
							"KLACK\r\n"
								);
	Uart0_Transmit(Frame_Command, pBuff_String2, String_Length);

	result = Point_Move_To( Frame_Command, '#' );					//帧以#结束

	heap_free(1, pBuff_String1);
	heap_free(1, pBuff_String2);

}

void FuctionRF_Write(UART_Data_Struct *Frame_Command)
{
	ERROR_CODE Result = NO_ERR;
	COMMAND_STRUCT CmdBuffer;
	XL902SWITCH_RESULT XL902SwitchResult;
	char Buff_String[100];
	int String_Length;
	int i;
	float fRFData;
	char *pBuffGears_string;
	char XL902GearsNo;
	XL902_CResult result;
	char range;


	result = Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
	if(result == XL902CR_True)
	{
		if((strcmp(Password, String_Get(Frame_Command->p_Buff_Read)) == 0))
		{
			Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
			pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
			XL902GearsNo = XL902Gears_num;
			for(i = 0; i < XL902Gears_num; i++)
			{
				if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
				{
					XL902GearsNo = i;
					break;
				}
			}
			if(XL902GearsNo != XL902Gears_num)
			{
				range = XL902GearsNo;
			}else
			{
				//错误信息打印
				Uart0_Transmit(Frame_Command, "XL902Gears_num ERR\r\n", strlen("XL902Gears_num ERR\r\n"));
			}
			Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
			fRFData = String_to_Float(Frame_Command->p_Buff_Read);

			//发送消息队列或者信号量通知响应任务进行处理

			//回复上位机
			Uart0_Transmit(Frame_Command, "RDWACK\r\n", strlen("ADWACK\r\n"));
		}else
		{
			//回复上位机
			Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));

		}

		//将获取的参数存进FLASH
		XL902SwitchResult = XL902FlashRF_Write(fRFData, (XL902SWITCH_DATA)range);
	}else
	{
		//回复上位机
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}

	Point_Move_To( Frame_Command, '#' );					//帧以#结束

}

void FuctionRF_Read(UART_Data_Struct *Frame_Command)
{
	char Buff[1][15];
	char Buff_String[100];
	int String_Length;
	unsigned short i = 0, j = 0;
	COMMAND_STRUCT CmdBuffer;
	float fRFData;
	char *pBuffGears_string;
	char XL902GearsNo;
	XL902_CResult result;
	char range;

	//获取通道及档位编号
	result = Point_Move_To(Frame_Command, ';');		//将指针移动到数据处
	if(result == XL902CR_True)
	{
		pBuffGears_string = String_Get(Frame_Command->p_Buff_Read);
		XL902GearsNo = XL902Gears_num;
		for(i = 0; i < XL902Gears_num; i++)
		{
			if(strcmp((ppsXL902Gears[i]), pBuffGears_string) == 0)
			{
				XL902GearsNo = i;
				break;
			}
		}
		if(XL902GearsNo != XL902Gears_num)
		{
			range = XL902GearsNo;



			//等待系数更新
			XL902FlashRF_Read(&fRFData, (XL902SWITCH_DATA)range);

			Float_to_StringEx(fRFData, Buff[0]);

			String_Length = sprintf(Buff_String,
									"RDR\r\n"
									"RANGE;%s\r\n"
									"L1:A;%s\r\n"
									"ADRACK\r\n",
									*(ppsXL902Gears + range),
									Buff[0]
										);
			Uart0_Transmit(Frame_Command, Buff_String, String_Length);

		}else
		{
			//错误信息打印
			Uart0_Transmit(Frame_Command, "XL902Gears_num ERR\r\n", strlen("XL902Gears_num ERR\r\n"));
		}
	}else
	{
		//错误信息打印
		Uart0_Transmit(Frame_Command, "ERR;\r\n", strlen("ERR;\r\n"));
	}
	Point_Move_To( Frame_Command, '#' );					//帧以#结束
}



void WaveDataToString(float *data,int DotNum,char *pString)
{
	int i = 0;
	char string[20];
	int length_test;

	//第一次直接给引入的数据赋值，后面进行拼接
	Float_to_StringEx(data[i], string);
//	memcpy(pString, string, strlen(string)+1);
	strcpy(pString, string);
	length_test = strlen(pString);
	strcat(pString, "\r\n");

	for(i = 1; i < DotNum; i++)
	{
		Float_to_StringEx(data[i], string);
		strcat(pString, string);
		strcat(pString, "\r\n");
	}

}


int String_to_Int(char *String_In)
{
	int i;
	char *p_String;
	char count = 0;
	p_String = String_In;

	while(*(String_In) != '\r')//需要加入异常处理
	{
		String_In++;
		count ++;
		if((*(String_In) == '#') || count > 20)
		{
			String_In -= count;
			return 0;
		}
	}
	*String_In = '\0';
	String_In += 2;							//指向下一个参数开头
	sscanf(p_String, "%d", &i);
	return i;
}

float String_to_Float(char *String_In)
{
	float i;
	char *p_String;
	char count = 0;
	p_String = String_In;

	while(*(String_In) != '\r')
	{
		String_In++;
		count ++;
		if((*(String_In) == '#') || count > 20)
		{
			String_In -= count;
			return 0;
		}
	}
	*String_In = '\0';
	String_In += 2;							//指向下一个参数开头
	sscanf(p_String, "%f", &i);
	return i;
}
//指数表示的字符串转换为浮点型，以下暂时没有加入对于0等非指数形式数据的异常处理，所以接入数据必须依照2.3000E+2这种形式
float StringEx_to_Float(char *String_In)	//指数转浮点数
{
	float i;
	int j;
	char *p_String;
	char count = 0;
	p_String = String_In;

	while(*(String_In) != 'E')
	{
		String_In++;
		count ++;
		if((*(String_In) == '#') || count > 20)
		{
			String_In -= count;
			return 0;
		}
	}
	*String_In = '\0';
	String_In ++;							//指向下一个参数开头
	sscanf(p_String, "%f", &i);

	while(*(String_In) != '\r')
	{
		String_In++;

	}
	*String_In = '\0';
	String_In += 2;							//指向下一个参数开头
	sscanf(p_String, "%d", &j);

	i = i * pow(10, j);

	return i;
}

void Float_to_StringEx(float src,char *dst)
{
	int i = 0;
	int flag = 0;
	float f;
	f = src;

	if(f < 0)
	{
		f *= -1;
		flag = 1;
	}

	if(f == 0)
	{
		asm("nop;");

	}
	else if(f < 1)
	{
		while(f < 1)
		{
			i--;
			f *= 10;

		}

	}
	else if(f >= 1)
	{
		while(f >= 10)
		{
			i++;
			f /= 10;

		}

	}

	if(flag == 1)
	{

		f *= -1;
	}

	sprintf(dst,
			"%.4fE%d",
			f,i
			);
}

char *String_Get(char *String_In)
{
	char *p_String;
	char count = 0;
	p_String = String_In;

	while(*(String_In) != '\r')
	{
		String_In++;
		count ++;
		if((*(String_In) == '#') || count > 20)
		{
			String_In -= count;
			return NULL;
		}

	}
	*String_In = '\0';
	String_In += 2;							//指向下一个参数开头

	return p_String;

}

void WaveDataSend(char* str)
{
	int String_Length, i, j, k;
	char* pBuff_String2;
	char* pBuff_String3;

	pBuff_String2 = heap_calloc(1, 256, sizeof(float));
	if(pBuff_String2 == NULL)
	{
		return;
	}
	pBuff_String3 = heap_calloc(1, 256, sizeof(float));
	if(pBuff_String3 == NULL)
	{
		return;
	}

	String_Length = strlen(str);
	j = String_Length / 200;
	k = String_Length % 200;

	for(i = 0; i <j; i++)
	{
		memcpy(pBuff_String3, &(str[i*200]), 200);
		pBuff_String3[201] = '\0';
		String_Length = sprintf(pBuff_String2,
								"%s",
								pBuff_String3
									);
		Uart0_Transmit(&UART0_Data, pBuff_String2, String_Length);
	}
	if(k != 0)	//不是200的整数倍时
	{
		memcpy(pBuff_String3, &(str[i*200]), k);
		pBuff_String3[k] = '\0';
		String_Length = sprintf(pBuff_String2,
								"%s",
								pBuff_String3
									);
		Uart0_Transmit(&UART0_Data, pBuff_String2, String_Length);
	}

	heap_free(1, pBuff_String3);
	heap_free(1, pBuff_String2);
}

static void downSoftAck(unsigned short FlagRespond, unsigned short FlagRight, unsigned short FlameNum)
{
	char Buff_String[200];
	int String_Length;
	unsigned short flameLength = 14;
	unsigned char buff[15];
	unsigned char i = 0, j = 0, k = 0;
	unsigned char sum = 0;

//	buff[i++] = 0x68;
//	buff[i++] = (unsigned char)(flameLength & 0xff);
//	sum += buff[i];
//	buff[i++] = (unsigned char)((flameLength >> 8) & 0xff);
//	sum += buff[i];
//	buff[i++] = 0x68;
//	sum += buff[i];
//	buff[i++] = 0x00;
//	sum += buff[i];
//	buff[i++] = 0x55;
//	sum += buff[i];
//	buff[i++] = (unsigned char)(FlagRespond & 0xff);
//	sum += buff[i];
//	buff[i++] = (unsigned char)((FlagRespond >> 8) & 0xff);
//	sum += buff[i];
//	buff[i++] = (unsigned char)(FlagRight & 0xff);
//	sum += buff[i];
//	buff[i++] = (unsigned char)((FlagRight >> 8) & 0xff);
//	sum += buff[i];
//	buff[i++] = (unsigned char)(FlameNum & 0xff);
//	sum += buff[i];
//	buff[i++] = (unsigned char)((FlameNum >> 8) & 0xff);
//	sum += buff[i];
//	buff[i++] = sum;
//	buff[i++] = 0x16;

	buff[i++] = 0x68;
	buff[i++] = 0;
	buff[i++] = 0;
	buff[i++] = 0x68;
	buff[i++] = 0x01;
	buff[i++] = 0x55;
	buff[i++] = (unsigned char)(FlagRespond & 0xff);
	buff[i++] = (unsigned char)((FlagRespond >> 8) & 0xff);
	buff[i++] = (unsigned char)(FlagRight & 0xff);
	buff[i++] = (unsigned char)((FlagRight >> 8) & 0xff);
	buff[i++] = (unsigned char)(FlameNum & 0xff);
	buff[i++] = (unsigned char)((FlameNum >> 8) & 0xff);

	sum = 0;
	for(j = 4; j < i; j++)
		sum += buff[j];

	buff[i++] = sum;
	buff[i++] = 0x16;

	buff[1] = i;
	buff[2] = 0;
	//发送模块信息
	for(k = 0; k < i; k++)
	{
		Uart0_Transmit(&UART0_Data, &buff[k], 1);
	}



}


char AutoModeCheck(void)
{
	return flag_RunAuto;
}












