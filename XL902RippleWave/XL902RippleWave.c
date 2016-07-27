/*
 * XL902RippleWave.c
 *
 *  Created on: 2016-5-4
 *      Author: 豪_lion
 */
#include "XL902RippleWave.h"


//static AD7608PDataFrame XL902ADSADBuff;
static int XL902ADSampleRate;	//全局，采样率，用作状态保存供给外部使用
static int XL902ADSampleDot;	//全局，每周波采样点数，用作状态保存供给外部使用
//static AD7608PDataFrame* ADCurrentData;
static char XL902ADSampleState = 0;//0代表不在采样状态，1代表正在采样
//static char PulseFistTimeSet = 1;
//static int numtest = 0;
static AD7608PDataFrame XL902RWSADBuff;
//static float *UDataBuff = NULL, *IDataBuff = NULL;
//static int SampleRate;
//static volatile unsigned int DataBuffOffset = 0;
static float URipplePersent = 0, IRipplePersent = 0, URippleA = 0, IRippleA = 0;//纹波系数和纹波幅度
static float RippleCoefficient[XL902Gears_Num];
section("sdram_bank0") static float UWaveDataRead[RippleWaveDotCount];
section("sdram_bank0") static float UWaveDataWrite[RippleWaveDotCount];
section("sdram_bank0") static float IWaveDataRead[RippleWaveDotCount];
section("sdram_bank0") static float IWaveDataWrite[RippleWaveDotCount];
static int SemEnable = 1;
static int counttest = 0;




//纹波采样任务
void Sem_Control_XL902Ripple(void* arg)
{
	OS_ERR err;
//	AD7608PResult result;
	XL902SWITCH_DATA gearU, gearI;
	AD7608PRange Range;
	int CurrentRange;
	XL902RWResult RWResult;
	volatile char XL902ISwitching;
	XL902RWSampleData ADSampleData;
	char Flag_FirstTime = 1;
	XL902RWSampleData RWSampleData;
	float UMax = 0,UMin = 0,IMax = 0,IMin = 0;
	int count = 0;
	float UDC, IDC;
	XL902SWITCH_RESULT SWResult;
#ifdef XL902ADS_Debug
	char Buff[7][15];
	char Buff_String[100];
	int String_Length;
#endif



	//初始化任务
	//设置采样的初始化，默认采样率周波2560点,相当与128KHz
	RWResult = XL902RippleWave_ini(RippleWaveDot);
	if(RWResult == XL902RW_False)
	{
		//发送错误报告
#ifdef XL902SamMeterTask_degug
		Uart0_Transmit(&UART0_Data, "XL902ADSample_ini false\n",  strlen("XL902ADSample_ini false\n"));
#endif
	}

	SWResult = XL902SwitchRF_ini();
	if(SWResult == XL902Switch_False)
	{
		//发送错误报告
#ifdef XL902SamMeterTask_degug
		Uart0_Transmit(&UART0_Data, "XL902SwitchRF_ini false\n",  strlen("XL902SwitchRF_ini false\n"));
#endif
	}

	while(1)
	{
		//接收采样中断的信号量
		OSSemPend(&g_Sem_RippleWave,0u, OS_OPT_PEND_BLOCKING, NULL, &err);
		//此时表示已经采集到RippleWaveDotCount个数据，任务完成，关闭定时器
		if(count > RippleWaveDotCount)
		{
			counttest = 0;
			count = 0;
			SemEnable = 0;
			//关闭采样任务
			XL902RWSample_stop();
			//获取直流采样值
			MeasureData_Get(&UDC, &IDC, NULL, NULL, NULL);
			//获取纹波大小
			URippleA = (UMax - UMin) / 2;
			IRippleA = (IMax - IMin) / 2;
			//计算纹波系数
			URipplePersent = URippleA / UDC * 100;
			IRipplePersent = IRippleA / IDC * 100;

//			//将写Buff中的数据转移到读Buff中，提供外部读取接口
//			memcpy(UWaveDataRead, UWaveDataWrite, RippleWaveDotCount * sizeof(float));
//			memcpy(IWaveDataRead, IWaveDataWrite, RippleWaveDotCount * sizeof(float));
			//调用上位机发送函数发送结果
			FuctiongRipple_Read(&UART0_Data);
		}
		if(SemEnable == 1)
		{
			//进行采样任务,登记相应档位，同时将获取的数据存入FIFO中
			{
				AD7608PSamDateGet();
				//获取当前档位
				gearU = CurrentUSwitchRead();
				gearI = CurrentISwitchRead();
				CurrentRange = AD7608PRangeRead();
				if(CurrentRange == 5)
				{
					Range = AD7608PRange5V;
				}else
				{
					Range = AD7608PRange10V;
				}
				AD7608PGearRegiser(XL902I1Channal, (char)gearI);//登记通道当前档位
				AD7608PGearRegiser(XL902U1Channal, (char)gearU);//登记通道当前档位
				AD7608PFrameAddToFIFO();//移入FIFO序列
			}


			//获取其实际测量值
			XL902RWSampleDataGet(&RWSampleData);

			if(count == 0)		//第一次数据丢弃，因为该数据不可用
			{
				//因为采集到的信号通过了高通滤波，依此采样值为正负值，此处初始值为0刚好是中间值，可用于比较大小
				UMax = 0;
				UMin = 0;
				IMax = 0;
				IMin = 0;
			}else
			if(count <= RippleWaveDotCount)		//此时将获取的数据进行数据比较
			{
				UWaveDataWrite[count] = RWSampleData.U1;
				IWaveDataWrite[count] = RWSampleData.I1;
				//每次都比较大小
				if(RWSampleData.U1 > UMax)
				{
					UMax = RWSampleData.U1;
				}
				else if(RWSampleData.U1 < UMin)
				{
					UMin = RWSampleData.U1;
				}

				if(RWSampleData.I1 > IMax)
				{
					IMax = RWSampleData.I1;
				}
				else if(RWSampleData.I1 < IMin)
				{
					IMin = RWSampleData.I1;
				}

			}
	//		//此时表示已经采集到RippleWaveDotCount个数据，任务完成，关闭定时器
	//		if(count > RippleWaveDotCount)
	//		{
	//			count = 0;
	//			//关闭采样任务
	//			XL902RWSample_stop();
	//			//获取直流采样值
	//			MeasureData_Get(&UDC, &IDC, NULL);
	//			//获取纹波大小
	//			URippleA = (UMax - UMin) / 2;
	//			IRippleA = (IMax - IMin) / 2;
	//			//计算纹波系数
	//			URipplePersent = URippleA / UDC * 100;
	//			IRipplePersent = IRippleA / IDC * 100;
	//			//调用上位机发送函数发送结果
	//			FuctiongRipple_Read(&UART0_Data);
	//		}else
			{
				count ++;
			}

		}


	}

}

XL902RWResult XL902RippleWave_ini(int SampleDot)
{
	XL902RWResult XL902result;
#ifdef XL902RW_AD7608P
	AD7608PResult result;
	AD7608PResult ADResult;

	//初始化
	result = AD7608P_ini();
	if(result == AD7608P_False)
	{
		return XL902RW_False;
	}

	//设置7608内部档位为10V档
	ADResult = AD7608PRangeSet(AD7608PRange10V);
	if(ADResult == AD7608P_False)
	{
		//发送错误报告
#ifdef XL902RW_degug
		Uart0_Transmit(&UART0_Data, "AD7608PRangeSet false\n",  strlen("AD7608PRangeSet false\n"));
#endif
	}

	XL902result = XL902RWSampleDot_set(SampleDot);
	if(XL902result == XL902RW_False)
	{
		return XL902RW_False;
	}

	//以下先对过程需要用到的一些系数进行计算，避免过程浪费计算资源
	RippleCoefficient[U1A] = RippleCoefficientU1_U1A;
	RippleCoefficient[U1B] = RippleCoefficientU1_U1B;
	RippleCoefficient[I1A] = RippleCoefficientI1_I1A;
	RippleCoefficient[I1B] = RippleCoefficientI1_I1B;
	RippleCoefficient[I1C] = RippleCoefficientI1_I1C;
//
//	//计算用于档位比较的比较值
//	XL902ADSwitchCompareDateCal();
#else

#endif
	return XL902RW_True;
}


XL902RWResult XL902RWSample_run(void)
{

#ifdef XL902RW_AD7608P
	TimerResult result;
	//开启定时器，启动采样
	result = timer_open(Timer3);
	if(result == Timer_False)
	{
		return XL902RW_False;
	}
	SemEnable = 1;
//	XL903ADSampleFirstTime = 1;	//停止后第一次采样标志置位
	XL902ADSampleState = 1;		//正在进行采样任务标志
//	PulseFistTimeSet = 1;		//脉冲设置第一次标志
	//复位FIFO（不需要）

#else

#endif

	return XL902RW_True;
}


XL902RWResult XL902RWSampleDot_set(int SampleDot)
{
	TimerResult result;
	XL902RWResult XL902result;

	XL902result = XL902RWSample_stop();
	if(XL902result == XL902RW_False)
	{
		return XL902RW_False;
	}
	//保存过程信息
	XL902ADSampleRate = SampleDot * 50;//50Hz的周波
	XL902ADSampleDot = SampleDot;

	if(XL902ADSampleRate > 200000)		//AD最高采样率200K
	{
		return XL902RW_False;
	}

	//定义采样定时器
	result = timer_init(Timer3, XL902ADSampleRate, NULL);		//目前采样率12.8K
	if(result == Timer_False)
	{
		return XL902RW_False;
	}

	return XL902RW_True;
}

XL902RWResult XL902RWSample_stop(void)
{

#ifdef XL902RW_AD7608P
	TimerResult result;
	//开启定时器，启动采样
	result = timer_close(Timer3);
	if(result == Timer_False)
	{
		return XL902RW_False;
	}

	//此时需要把档位都跳到最大
//	XL902SwitchMaxSet_protect();
	XL902ADSampleState = 0;		//正在进行采样任务标志
//	AD7608PReset();
#else

#endif

	return XL902RW_True;
}

//返回目前FIFO的长度，同时返回最终校准后的数据
int XL902RWSampleDataGet(XL902RWSampleData* data)
{
	AD7608PResult result;
	float i;
	int num;
	float CF;
	float CoefficientU1, CoefficientI1;
	float U_RF, I_RF;

	result = AD7608PDataGet(&XL902RWSADBuff);
	if(result == AD7608P_False)	//错误时返回长度0
	{
		return 0;
	}
////获取校准系数
//	CF = UCF_GetA(XL902RWSADBuff.gear[XL902U1Channal]);
//	if(CF < 0.5)//校准系数小于0.5时可能仪器为校准，此时为了保护仪器，设为0
//	{
//		CF = 1;
//	}
	i = Complement18ToInt32(XL902RWSADBuff.data[XL902U1Channal]);//获取AD转换结果的整型数据
	i = i * XL902RWSADBuff.range / 131072;//获得实际AD测得的幅度
//	//获取当前通道的增益值
//	switch(XL902RWSADBuff.gear[XL902U1Channal])
//	{
//		case U1A:
//			CoefficientU1 = RippleCoefficientU1_U1A;
//			break;
//		case U1B:
//			CoefficientU1 = RippleCoefficientU1_U1B;
//			break;
//		default:
//			break;
//	}
	CoefficientU1 = RippleCoefficient[XL902RWSADBuff.gear[XL902U1Channal]];
	XL902FlashRF_Read(&U_RF, (XL902SWITCH_DATA)XL902RWSADBuff.gear[XL902U1Channal]);
	if(U_RF == 0)	//避免未校准时出现计量值为0
	{
		U_RF = 1;
	}

	//以下增益部分取值交流的增益系数
	data->U1 = i / CoefficientU1 * U_RF;
//	data->U1 = i * XL902Coefficient[XL902RWSADBuff.gear[XL902U1Channal]] * CF;	//此处进行所有校准，提供最终数据,校准采用有效值校准


//	//获取校准系数
//	CF = UCF_GetA(XL902RWSADBuff.gear[XL902I1Channal]);
//	if(CF < 0.5)//校准系数小于0.5时可能仪器为校准，此时为了保护仪器，设为0
//	{
//		CF = 1;
//	}
	i = Complement18ToInt32(XL902RWSADBuff.data[XL902I1Channal]);//获取AD转换结果的整型数据
	i = i * XL902RWSADBuff.range / 131072;//获得实际AD测得的幅度
//	//获取当前通道的增益值
//	switch(XL902RWSADBuff.gear[XL902I1Channal])
//	{
//		case I1A:
//			CoefficientI1 = RippleCoefficientI1_I1A;
//			break;
//		case I1B:
//			CoefficientI1 = RippleCoefficientI1_I1B;
//			break;
//		case I1C:
//			CoefficientI1 = RippleCoefficientI1_I1C;
//			break;
//		default:
//			break;
//	}
	CoefficientI1 = RippleCoefficient[XL902RWSADBuff.gear[XL902I1Channal]];
	XL902FlashRF_Read(&I_RF, (XL902SWITCH_DATA)XL902RWSADBuff.gear[XL902I1Channal]);
	if(I_RF == 0)	//避免未校准时出现计量值为0
	{
		I_RF = 1;
	}

	data->I1 = i / CoefficientI1 * I_RF;
//	data->I1 = i * XL902Coefficient[XL902RWSADBuff.gear[XL902I1Channal]] * CF;	//该值没校准过，校准在采样任务里进行

	num = AD7608PBuffNum();//返回AD接收的BUFF中还有多少数据
	return num;//返回AD接收的BUFF中还有多少数据
}

#ifdef XL902RW_AD7608P
void Timer3Handler(void *pCBParam, uint32_t Event, void *pArg)
{
	OS_ERR err;

	switch(Event)
	    {
	        case ADI_TMR_EVENT_DATA_INT:
//#ifdef XL903ADS_Debug
//	        	adi_gpio_Toggle(ADI_GPIO_PORT_E, ADI_GPIO_PIN_14);
//#endif
//	        	AD7608PSampleboth();//八通道同步采样		//测试将其移进定时器中断中
	        	AD7608PSamDotBegin();
	        	//发送信号量
	        	if(SemEnable == 1)
	        	{
		        	OSSemPost(&g_Sem_RippleWave, OS_OPT_POST_1, &err);
	        	}

	            return;
	        default:

	            return;
	    }

}
#endif
//输入参数为电压纹波系数，电流纹波系数，电压纹波幅度，电流纹波幅度的地址
void XL902RippleInf_get(float *U_persent, float *I_persent, float *U_A, float *I_A)
{
	*U_persent = URipplePersent;
	*I_persent = IRipplePersent;
	*U_A = URippleA;
	*I_A = IRippleA;
}

//外度读取纹波波形数据接口，入口参数为需要输出的点数
float* XL902RippleWaveGet_U(int dotnum)
{
	int i , j, k;

	if(dotnum > RippleWaveDotCount)
	{
		return NULL;
	}
	j = RippleWaveDotCount / dotnum;
	for(i = 1; i < dotnum; i++)
	{
		k = i - 1;
		UWaveDataRead[k] = UWaveDataWrite[i * j];
	}

	return UWaveDataRead;
}

//外度读取纹波波形数据接口，入口参数为需要输出的点数
float* XL902RippleWaveGet_I(int dotnum)
{
	int i , j, k;

	if(dotnum > RippleWaveDotCount)
	{
		return NULL;
	}
	j = RippleWaveDotCount / dotnum;
	for(i = 1; i < dotnum; i++)
	{
		k = i - 1;
		IWaveDataRead[k] = IWaveDataWrite[i * j];
	}
	return IWaveDataRead;
}

