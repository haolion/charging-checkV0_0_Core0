#ifndef TASK_ADC_H
#define TASK_ADC_H

#include "../AD7793_SPI/ad7793_SPI.h"
#include "os.h"
#include <services/gpio/adi_gpio.h>
#include <drivers\uart\adi_uart.h>
#include "stdio.h"
#include "math.h"
#include "../DataStruct/CircularQueue.h"
#include "../EnergyPulse/EnergyPulse.h"
#include "../UARTx/UARTx.h"
#include "../soft_update/Flash/pc28f128p33.h"
#include "../soft_update/Flash/Flash_Init.h"
#include "../SWITCH/XL902Switch.h"
#include "Global_define.h"

//调试声明
//#define Debug_adcTask

//老样机，7793采用自己的内部时钟时
#define ad7793Clock_Internal

//LEM传感器的接入方向，可能存在正反
//#define DCIN205S_Positive

//硬件增益信息
#ifdef DCIN205S_Positive
#define ConvertRatioI1_205S	(1.0/1000)			//传感器转换比
#define ConvertRatioI1_405S	(1.0/1500)			//传感器转换比
#else
#define ConvertRatioI1_205S	(-1.0/1000)			//传感器转换比
#define ConvertRatioI1_405S	(-1.0/1500)			//传感器转换比
#endif
#ifdef XL902_200A
#define NumOfTurnsI1_I1C	4					//50A档绕的圈数
#define NumOfTurnsI1_I1B	1					//200A档绕的圈数
#define NumOfTurnsI1_I1A	1					//400A档绕的圈数
#define ConvertRatioI1		ConvertRatioI1_205S	//使用205S
#else
#define NumOfTurnsI1_I1C	8					//80A档绕的圈数
#define NumOfTurnsI1_I1B	2					//200A档绕的圈数
#define NumOfTurnsI1_I1A	1					//400A档绕的圈数
#define ConvertRatioI1		ConvertRatioI1_405S	//使用205S
#endif
#define RSampleI1_205S		10					//传感器输出的取样电阻
#define GainI1_AD620		3.495				//取样电阻后的增益(会自激，暂时不用该运放)
#define GainI1_AD2277		4					//取样电阻后的增益
#define GainI1_AD8220		(1.0/6)				//AD7793的前级增益
#define VrefI1_ADR441		2.50				//AD7793的参考芯片电压
#define GainI1_AD7793		1					//AD7793的内部增益
#define GainU1_750V			(1.0/65)			//电压前端增益750V
#define GainU1_450V			(1.0/41)			//电压前端增益450V
#define GainU1_250V			(1.0/15)			//电压前端增益250V
#define GainU1_100V			(1.0/7)				//电压前端增益100V
#define GainU1_45V			(1.0/3)				//电压前端增益45V
#define GainU1_15V			1.0					//电压前端增益15V
#define GainU1_AD7793		1					//AD7793的内部增益
#define VrefU1_ADR441		2.50				//AD7793的参考芯片电压
//#define GainU1_AD8220		(1.0/2)				//AD7793的前级增益，用于测试档位自动切换  测试电压155V
#define GainU1_AD8220		(1.0/6)				//AD7793的前级增益

#define XL902SwitchDelay_time	1				//继电器的动作时间，经测试，16.7Hz采样率时只需跳过一个采样周期

//!!需要提高速度时一下宏定义自己先算好，不然其不会自己优化，只是代替。
//电流增益部分
#ifdef XL902_200A
#define CoefficientI1_I1C (ConvertRatioI1*NumOfTurnsI1_I1C*RSampleI1_205S*GainI1_AD2277*GainI1_AD7793*GainI1_AD8220)
#else
#define CoefficientI1_I1C (ConvertRatioI1*NumOfTurnsI1_I1C*RSampleI1_205S*GainI1_AD2277*GainI1_AD7793*GainI1_AD8220)
#endif
#define CoefficientI1_I1B (ConvertRatioI1*NumOfTurnsI1_I1B*RSampleI1_205S*GainI1_AD2277*GainI1_AD7793*GainI1_AD8220)
#define CoefficientI1_I1A (ConvertRatioI1*NumOfTurnsI1_I1A*RSampleI1_205S*GainI1_AD2277*GainI1_AD7793*GainI1_AD8220)

//电压增益部分
#define CoefficientU1_U1A (GainU1_750V*GainU1_AD8220*GainU1_AD7793)
#define CoefficientU1_U1B (GainU1_450V*GainU1_AD8220*GainU1_AD7793)

typedef enum SwitchAuto
{
	SwitchAuto_Enable = 0,
	SwitchAuto_Disable
}SwitchAuto;

typedef enum XL902MeasureState
{
	XL902MeasureRunning,
	XL902MeasureStop
}XL902MeasureState;

typedef enum
{
	adc_true,
	adc_false
}adc_result;

#define Time_ADC_Sample (1.0/16.7)	//AD采样率

//串口控制信息结构体
extern UART_Data_Struct UART0_Data;//串口结构体，用于缓存
//extern ADI_UART_HANDLE ghUart;
extern OS_SEM g_Sem_7793_Run;
//extern float *p_I_FIFO_Begin, *p_I_FIFO_End;
//extern float *p_U_FIFO_Begin, *p_U_FIFO_End;
extern OS_SEM g_Sem_Energy_Measurement;

extern AD7793_CS AD7793_CS2,AD7793_CS3;

void Sem_Control_ad7793(void* arg);
void Task_Energy_Measurement(void* arg);
void MeasureData_Get(float *U, float *I, float *E, float *P, float *T);//为外部函数提供数据读取接口
adc_result I_CF_Set(float CF);
adc_result U_CF_Set(float CF);
float I_CF_Read(void);
float U_CF_Read(void);
void SwitchAutoState(SwitchAuto en);
void XL902MeasureState_Set(XL902MeasureState state);
XL902MeasureState XL902MeasureState_Read(void);//读取测量状态是否正在运行，运行中返回1
void MeasureData_ini(void);

#ifdef ad7793Clock_Internal
void Timer4Handler(void *pCBParam, uint32_t Event, void *pArg);
#endif

#endif
