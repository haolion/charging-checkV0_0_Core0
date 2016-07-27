/*
 * 功能：采样
 * 版本：V1.0
 * 描述：采用AD7793进行采样并进行电能计量，用于充电机现场检定装置的采样模块
 * 公司：深圳市星龙科技股份有限公司
 * 作者：张颖豪
 * 日期：2015/7/3
 * 备注：无
 */
#include "Task_adc.h"

static CircularQ_Float BuffLoop_U, BuffLoop_I;
static CircularQ_Float *p_BuffLoop_U, *p_BuffLoop_I;
static float M_I, M_U, M_E, M_P, M_T;
static float UA_CF, IA_CF, UQ_CF, IQ_CF;//校准系数
static float Coefficient[XL902Gears_Num];//存储增益参数的计算结果，用于加速

//全局
static char flagSwitch_Auto = 0;
static int MeasureDataGet_count = 0;
static XL902MeasureState MeasureRunning = XL902MeasureStop;//1表示测量运行中，0表示不在测量运行中
static int XL902TimeCount = 0;//时间统计个数 ，运行时间 = 采样周期 * 时间统计个数

#ifdef ad7793Clock_Internal
static volatile float M_I_Current = 0,M_U_Current = 0;//用于7793内部时钟时功率计算

#endif

/*
 * 函数:AD7793连续采样任务
 * 描述：通过接收信号量g_Sem_Uart_ad7793,启动ad7793
 * 接口参数：无
 * 返回：无
 * 备注：
 */

void Sem_Control_ad7793(void* arg)
{
	OS_ERR err;
	ADI_GPIO_RESULT result;
	uint32_t GPIO_Data;
	unsigned short i = 0;
	unsigned long ad7793_2_Data = 0;
	unsigned long ad7793_3_Data = 0;
	float UValue, IValue, UZC, IZC, CoefficientU1, CoefficientI1;
	uint8_t Uart_Buff_Send1[3],text;
	char Str_AD[100];
	unsigned char Len_Str_AD = 0;
	int j = 0;
	double ii;
	char flag_USwitchChange = 0, flag_ISwitchChange = 0;
	XL902SWITCH_RESULT switchResult;
	XL902SWITCH_DATA U1GearData, I1GearData, U1GearData_1, I1GearData_1;
	XL902SWITCH_RESULT XL902SwitchResult;
	float U_test = 0, I_test = 0;
//	UART_Data_Struct *ghUart0 = arg;

	p_BuffLoop_U = &BuffLoop_U;
	p_BuffLoop_I = &BuffLoop_I;
	CircularQF_init(p_BuffLoop_U);
	CircularQF_init(p_BuffLoop_I);

	//档位控制初始化
	XL902SwitchResult = XL902Switch_ini();
	if(XL902SwitchResult == XL902Switch_False)
	{
		Uart0_Transmit(&UART0_Data, "XL902SwitchGpio_ini False\n", strlen("XL902SwitchGpio_ini False\n"));
	}
	//最高档位
	U1GearData = URMax;
	I1GearData = IRMax;
	//初始化校准系数，从FLASH中读出
	switchResult = XL902SwitchAF_ini(&UA_CF, &UQ_CF, &IA_CF, &IQ_CF);
	if(switchResult == XL902Switch_False)
	{
		Uart0_Transmit(&UART0_Data, "XL902SwitchAF_ini false",  strlen("XL902SwitchAF_ini false"));
	}
	//初始化零位补偿系数，从FLASH中读出
	switchResult = XL902SwitchZC_ini(&UZC, &IZC);
	if(switchResult == XL902Switch_False)
	{
		Uart0_Transmit(&UART0_Data, "XL902SwitchZC_ini false",  strlen("XL902SwitchZC_ini false"));
	}
//	ad7793Init();
	ad7793Continue_Init();		//默认任务不开启
//	ad7793WrComReg(AD7793_CS2, CR_READ_STATUS);
//	printf("ad7793 has be ready.\n");
	//此处由内存读入校准系数

	ii = pow(2 , 23);
	//以下先对过程需要用到的一些系数进行计算，避免过程浪费计算资源
	Coefficient[U1A] = CoefficientU1_U1A;
	Coefficient[U1B] = CoefficientU1_U1B;
	Coefficient[I1A] = CoefficientI1_I1A;
	Coefficient[I1B] = CoefficientI1_I1B;
	Coefficient[I1C] = CoefficientI1_I1C;

	OSTimeDly(20u,OS_OPT_TIME_DLY,&err);
	while(1)
	{
		OSSemPend(&g_Sem_7793_Run,0u, OS_OPT_PEND_BLOCKING, NULL, &err);
//		switch(U1GearData)
//		{
//			case U1A:
//				CoefficientU1 = CoefficientU1_U1A;
//				break;
//			case U1B:
//				CoefficientU1 = CoefficientU1_U1B;
//				break;
//			default:
//				break;
//		}
//		switch(I1GearData)
//		{
//			case I1A:
//				CoefficientI1 = CoefficientI1_I1A;
//				break;
//			case I1B:
//				CoefficientI1 = CoefficientI1_I1B;
//				break;
//			case I1C:
//				CoefficientI1 = CoefficientI1_I1C;
//				break;
//			default:
//				break;
//		}
		CoefficientU1 = Coefficient[U1GearData];
		CoefficientI1 = Coefficient[I1GearData];

		j ++;
		XL902TimeCount ++;//运行时间

//由于前面只用一个中断进行响应，而且利用了AD7793_CS3作为中断源，所以此处先对AD7793_CS3进行读取，避免异常
		ad7793_Convert_Continue_Both(AD7793_CS3, AD7793_CS2, &(ad7793_3_Data), &(ad7793_2_Data));
		if(flag_USwitchChange == 0)
		{
			UValue = (ad7793_2_Data/ii - 1)*VrefU1_ADR441*UA_CF/CoefficientU1 + UZC;//计算模拟值,存在档位时需要进行计算
			U_test += UValue;
		}else
		{
			flag_USwitchChange--;
		}

		CircularQF_Push(p_BuffLoop_U, UValue);				//电压
//		if(j == 17)
//		{
////			j = 0;
//			U_test /= 17;
//#ifdef Debug_adcTask
//			Len_Str_AD = sprintf(Str_AD, "U1:%.6fv\n", U_test);
//			Uart0_Transmit(&UART0_Data, Str_AD,  Len_Str_AD);
////			printf("%s",Str_AD);
//#endif
//			U_test = 0;
//		}

		if(flag_ISwitchChange == 0)
		{
			IValue = (ad7793_3_Data/ii - 1)*VrefI1_ADR441*IA_CF/CoefficientI1 + IZC;//计算模拟值，存在档位时需要进行计算
			I_test += IValue;
		}else
		{
			flag_ISwitchChange--;
		}
		CircularQF_Push(p_BuffLoop_I, IValue);				//电流
		if(j == 17)
		{
			j = 0;
			I_test /= 17;
			U_test /= 17;
#ifdef Debug_adcTask
//			Len_Str_AD = sprintf(Str_AD, "I1:%.6fA\n", IValue);
//			Len_Str_AD = sprintf(Str_AD, "%.6f\n", I_test);
			Len_Str_AD = sprintf(Str_AD, "%.6f\t%.6f\n", U_test, I_test);
			Uart0_Transmit(&UART0_Data, Str_AD, Len_Str_AD);
//			printf("%s",Str_AD);
#endif
			I_test = 0;
			U_test = 0;
		}

		//自动换挡程序
		if(flagSwitch_Auto == 1)
		{
			//以下进行量程比较，超过则换挡
			U1GearData_1 = U1GearData;
			I1GearData_1 = I1GearData;
			XL902SwitchAuto_protect(UValue, &UA_CF, &UQ_CF, &UZC, &U1GearData, XL902Channal_U);
			XL902SwitchAuto_protect(IValue, &IA_CF, &IQ_CF, &IZC, &I1GearData, XL902Channal_I);
			if(U1GearData_1 != U1GearData)			//档位切换时保持几次值不变，为了防止乒乓效应
			{
				flag_USwitchChange = XL902SwitchDelay_time;				//此处为保留次数，具体根据采样率以及继电器的开关时间决定
			}
			if(I1GearData_1 != I1GearData)			//档位切换时保持几次值不变，为了防止乒乓效应
			{
				flag_ISwitchChange = XL902SwitchDelay_time;				//此处为保留次数，具体根据采样率以及继电器的开关时间决定
			}
		}else
		{
			I1GearData = CurrentISwitchRead();
			IA_CF = ICF_GetA(I1GearData);
			IZC = ZC_Get(I1GearData);
			U1GearData = CurrentUSwitchRead();
			UA_CF = UCF_GetA(U1GearData);
			UZC = ZC_Get(U1GearData);
		}
#ifdef ad7793Clock_Internal
		CircularQF_Poll(p_BuffLoop_U, &M_U);
		CircularQF_Poll(p_BuffLoop_I, &M_I);
		M_U_Current = M_U;
		M_I_Current = M_I;
#else
		OSSemPost(&g_Sem_Energy_Measurement, OS_OPT_POST_1, &err);
#endif
//		if(j == 100)
//		{
//			/* clear pin interrupt mask */
//			adi_gpio_EnablePinInterruptMask(
//					ADI_GPIO_PIN_INTERRUPT_2,
//					ADI_GPIO_PIN_2,
//					false);
////			adi_int_EnableInt(INTR_PINT2_BLOCK, false);
//			ADC_Value = (ad7793_3_Data/pow(2 , 23)-1)*2480*6/4;//计算模拟值，存在档位是需要进行计算
//			Len_Str_AD = sprintf(Str_AD, "AD channal2:%.2fmv\n", ADC_Value);
//	//		adi_uart_Write(ghUart, Str_AD,  Len_Str_AD);
//			printf("%s",Str_AD);
//			while(1);
//
//		}

	}
}

////直流标准器用
//void Sem_Control_ad7793(void* arg)
//{
//	OS_ERR err;
//	ADI_GPIO_RESULT result;
//	uint32_t GPIO_Data;
//	unsigned short i = 0;
//	unsigned long ad7793_2_Data = 0;
//	unsigned long ad7793_3_Data = 0;
//	float UValue, IValue, UZC, IZC, CoefficientU1, CoefficientI1;
//	uint8_t Uart_Buff_Send1[3],text;
//	char Str_AD[100];
//	unsigned char Len_Str_AD = 0;
//	int j = 0;
//	double ii;
//	char flag_USwitchChange = 0, flag_ISwitchChange = 0;
//	XL902SWITCH_RESULT switchResult;
//	XL902SWITCH_DATA U1GearData, I1GearData, U1GearData_1, I1GearData_1;
//	XL902SWITCH_RESULT XL902SwitchResult;
////	UART_Data_Struct *ghUart0 = arg;
//
//	p_BuffLoop_U = &BuffLoop_U;
//	p_BuffLoop_I = &BuffLoop_I;
//	CircularQF_init(p_BuffLoop_U);
//	CircularQF_init(p_BuffLoop_I);
//
//	//档位控制初始化
//	XL902SwitchResult = XL902Switch_ini();
//	if(XL902SwitchResult == XL902Switch_False)
//	{
//		Uart0_Transmit(&UART0_Data, "XL902SwitchGpio_ini False\n", strlen("XL902SwitchGpio_ini False\n"));
//	}
//	//最高档位
//	U1GearData = URMax;
//	I1GearData = IRMax;
//	//初始化校准系数，从FLASH中读出
//	switchResult = XL902SwitchAF_ini(&UA_CF, &UQ_CF, &IA_CF, &IQ_CF);
//	if(switchResult == XL902Switch_False)
//	{
//		Uart0_Transmit(&UART0_Data, "XL902SwitchAF_ini false",  strlen("XL902SwitchAF_ini false"));
//	}
//	//初始化零位补偿系数，从FLASH中读出
//	switchResult = XL902SwitchZC_ini(&UZC, &IZC);
//	if(switchResult == XL902Switch_False)
//	{
//		Uart0_Transmit(&UART0_Data, "XL902SwitchZC_ini false",  strlen("XL902SwitchZC_ini false"));
//	}
////	ad7793Init();
//	ad7793Continue_Init();		//默认任务不开启
////	ad7793WrComReg(AD7793_CS2, CR_READ_STATUS);
////	printf("ad7793 has be ready.\n");
//	//此处由内存读入校准系数
//
//	ii = pow(2 , 24);
//
//	OSTimeDly(20u,OS_OPT_TIME_DLY,&err);
//	while(1)
//	{
//		OSSemPend(&g_Sem_7793_Run,0u, OS_OPT_PEND_BLOCKING, NULL, &err);
////		switch(U1GearData)
////		{
////			case U1A:
////				CoefficientU1 = CoefficientU1_U1A;
////				break;
////			case U1B:
////				CoefficientU1 = CoefficientU1_U1B;
////				break;
////			default:
////				break;
////		}
////		switch(I1GearData)
////		{
////			case I1A:
////				CoefficientI1 = CoefficientI1_I1A;
////				break;
////			case I1B:
////				CoefficientI1 = CoefficientI1_I1B;
////				break;
////			case I1C:
////				CoefficientI1 = CoefficientI1_I1C;
////				break;
////			default:
////				break;
////		}
//
//		j ++;
//
////由于前面只用一个中断进行响应，而且利用了AD7793_CS3作为中断源，所以此处先对AD7793_CS3进行读取，避免异常
//		ad7793_Convert_Continue_Both(AD7793_CS3, AD7793_CS2, &(ad7793_3_Data), &(ad7793_2_Data));
//		if(flag_USwitchChange == 0)
//		{
//			UValue = (ad7793_2_Data/ii - 1)*VrefU1_ADR441*UA_CF/CoefficientU1 + UZC;//计算模拟值,存在档位时需要进行计算
//		}else
//		{
//			flag_USwitchChange--;
//		}
//
////		CircularQF_Push(p_BuffLoop_U, UValue);				//电压
//		if(j == 17)
//		{
//////			j = 0;
////#ifdef Debug_adcTask
////			Len_Str_AD = sprintf(Str_AD, "U1:%.6fv\n", UValue);
////			Uart0_Transmit(&UART0_Data, Str_AD,  Len_Str_AD);
//////			printf("%s",Str_AD);
////#endif
//		}
//
////		if(flag_ISwitchChange == 0)
////		{
//			IValue = (ad7793_3_Data/ii)*VrefI1_ADR441 / 128 + IZC;//计算模拟值，存在档位时需要进行计算
////		}else
////		{
////			flag_ISwitchChange--;
////		}
////		CircularQF_Push(p_BuffLoop_I, IValue);				//电流
//		if(j == 4)
//		{
//			j = 0;
//#ifdef Debug_adcTask
//			Len_Str_AD = sprintf(Str_AD, "%.7f\r\n", IValue);
//			Uart0_Transmit(&UART0_Data, Str_AD,  Len_Str_AD);
////			printf("%s",Str_AD);
//#endif
//		}
//		XL902FlashZC_Read(&IZC, U1A);
////		//自动换挡程序
////		if(0)
////		{
////			//以下进行量程比较，超过则换挡
////			U1GearData_1 = U1GearData;
////			I1GearData_1 = I1GearData;
////			XL902SwitchAuto_ZC(UValue, &UA_CF, &UQ_CF, &UZC, &U1GearData, XL902Channal_U);
////			XL902SwitchAuto_ZC(IValue, &IA_CF, &IQ_CF, &IZC, &I1GearData, XL902Channal_I);
////			if(U1GearData_1 != U1GearData)			//档位切换时保持几次值不变，为了防止乒乓效应
////			{
////				flag_USwitchChange = XL902SwitchDelay_time;				//此处为保留次数，具体根据采样率以及继电器的开关时间决定
////			}
////			if(I1GearData_1 != I1GearData)			//档位切换时保持几次值不变，为了防止乒乓效应
////			{
////				flag_ISwitchChange = XL902SwitchDelay_time;				//此处为保留次数，具体根据采样率以及继电器的开关时间决定
////			}
////		}else
////		{
////			I1GearData = CurrentISwitchRead();
////			IA_CF = ICF_GetA(I1GearData);
////			IZC = ZC_Get(I1GearData);
////			U1GearData = CurrentUSwitchRead();
////			UA_CF = UCF_GetA(U1GearData);
////			UZC = ZC_Get(U1GearData);
////		}
//
//
////		OSSemPost(&g_Sem_Energy_Measurement, OS_OPT_POST_1, &err);
////		if(j == 100)
////		{
////			/* clear pin interrupt mask */
////			adi_gpio_EnablePinInterruptMask(
////					ADI_GPIO_PIN_INTERRUPT_2,
////					ADI_GPIO_PIN_2,
////					false);
//////			adi_int_EnableInt(INTR_PINT2_BLOCK, false);
////			ADC_Value = (ad7793_3_Data/pow(2 , 23)-1)*2480*6/4;//计算模拟值，存在档位是需要进行计算
////			Len_Str_AD = sprintf(Str_AD, "AD channal2:%.2fmv\n", ADC_Value);
////	//		adi_uart_Write(ghUart, Str_AD,  Len_Str_AD);
////			printf("%s",Str_AD);
////			while(1);
////
////		}
//
//	}
//}

void Task_Energy_Measurement(void* arg)
{
	OS_ERR err;
#ifdef Debug_adcTask
	float test;
	char Str_AD[100];
	unsigned char Len_Str_AD = 0;

#endif
	unsigned int i = 0, j;
#ifdef ad7793Clock_Internal
	TimerResult result;
#endif
	EnergyPulseCount_Ini();

#ifdef ad7793Clock_Internal
	//定义采样定时器
	result = timer_init(Timer4, 16.7, NULL);		//目前定时器的时钟跟7793的采样率一致，都是16.7
	if(result == Timer_False)
	{
		asm("nop;");
	}
#endif

	while(1)
	{
		OSSemPend(&g_Sem_Energy_Measurement, 0u, OS_OPT_PEND_BLOCKING, NULL, &err);
#ifdef ad7793Clock_Internal
		M_P = M_I_Current * M_U_Current;								//单位W
		M_E = M_E + M_P * Time_ADC_Sample / 3600;		//单位Wh
		M_T = XL902TimeCount / 16.7;

#else
		CircularQF_Poll(p_BuffLoop_U, &M_U);
		CircularQF_Poll(p_BuffLoop_I, &M_I);

		M_P = M_I * M_U;								//单位W
		M_E = M_E + M_P * Time_ADC_Sample / 3600;		//单位Wh
		M_T = XL902TimeCount / 16.7;
#endif

#ifdef Debug_adcTask
		if(M_U > 298)
		{
			MeasureDataGet_count++;		//test
		}else
		{
//			Len_Str_AD = sprintf(Str_AD, "Count:%d\n", MeasureDataGet_count);
//			Uart0_Transmit(&UART0_Data, Str_AD,  Len_Str_AD);
		}
#endif


//		while(p_I_FIFO_Begin != p_I_FIFO_End)	//此处指针不相等说明电流和电压的缓冲区里面有数据
//		{
//			*(p_I_Data + i) = *(++p_I_FIFO_Begin);
//			*(p_U_Data + i) = *(++p_U_FIFO_Begin);
//			i++;
//		}

	}


}

void MeasureData_Get(float *U, float *I, float *E, float *P, float *T)
{
//	char Str_AD[100];
	unsigned char Len_Str_AD = 0;

	*U = M_U;
	*I = M_I;
	*E = M_E;
	*P = M_P;
	*T = M_T;

//	Len_Str_AD = sprintf(Str_AD, "Count:%d\n", MeasureDataGet_count);
//	Uart0_Transmit(&UART0_Data, Str_AD,  Len_Str_AD);

}

void MeasureData_ini(void)
{
	M_U = 0;
	M_I = 0;
	M_E = 0;
	M_P = 0;
	M_T = 0;
	XL902TimeCount = 0;
}

void SwitchAutoState(SwitchAuto en)
{
	if(en == SwitchAuto_Disable)
	{
		flagSwitch_Auto = 0;
	}else
	{
		flagSwitch_Auto = 1;
	}

}

void XL902MeasureState_Set(XL902MeasureState state)
{
	if(state == XL902MeasureRunning)
	{
		ad7793InterruptState_Set(ad7793Int_Enable);

		MeasureRunning = XL902MeasureRunning;
		MeasureDataGet_count = 0;		//test
		//下面清零计量值
		MeasureData_ini();
#ifdef ad7793Clock_Internal
		//开启电能计算的时钟定时器
		timer_open(Timer4);
#endif
	}else
	{
		ad7793InterruptState_Set(ad7793Int_Disable);
		//档位换到最高，避免过程没检测换挡导致烧坏
		XL902SwitchRange_Max();
		MeasureRunning = XL902MeasureStop;
		timer_close(Timer4);
	}
}

XL902MeasureState XL902MeasureState_Read(void)
{
	return MeasureRunning;
}

#ifdef ad7793Clock_Internal
void Timer4Handler(void *pCBParam, uint32_t Event, void *pArg)
{
	OS_ERR err;

	switch(Event)
	    {
	        case ADI_TMR_EVENT_DATA_INT:
	        	OSSemPost(&g_Sem_Energy_Measurement, OS_OPT_POST_1, &err);
	            return;
	        default:

	            return;
	    }

}
#endif


