/*
 * XL902RippleWave.h
 *
 *  Created on: 2016-5-4
 *      Author: 豪_lion
 */

#ifndef XL902RIPPLEWAVE_H_
#define XL902RIPPLEWAVE_H_

#include "../AD7608/AD7608P.h"
#include "../SWITCH/XL902Switch.h"
#include "../DataStruct/Data_conversion.h"
#include "../src/Task_adc.h"
#include "../XL902_COMMUNICATION/XL902_Communication.h"

extern UART_Data_Struct UART0_Data;//串口结构体，用于缓存

//以下为前端电路的增益部分，跟硬件相关,其它部分的增益在task_adc.h文件内
#define RippleGainU1_Resister		1//150.0/152.0				//前端输入分压增益
#define RippleGainI1_Resister		1//150.0/152.0				//前端输入分压增益
#define RippleGainU1_AD620			11							//AD620的增益
#define RippleGainI1_AD620			11							//AD620的增益
//以下为可用于代入计算的整体增益系数
#define RippleCoefficientU1_U1A	(GainU1_750V * RippleGainU1_Resister * RippleGainU1_AD620)
#define RippleCoefficientU1_U1B	(GainU1_450V * RippleGainU1_Resister * RippleGainU1_AD620)

#define RippleCoefficientI1_I1A	(ConvertRatioI1*NumOfTurnsI1_I1A*RSampleI1_205S*GainI1_AD2277*RippleGainU1_Resister*RippleGainU1_AD620)
#define RippleCoefficientI1_I1B	(ConvertRatioI1*NumOfTurnsI1_I1B*RSampleI1_205S*GainI1_AD2277*RippleGainU1_Resister*RippleGainU1_AD620)
#define RippleCoefficientI1_I1C	(ConvertRatioI1*NumOfTurnsI1_I1C*RSampleI1_205S*GainI1_AD2277*RippleGainU1_Resister*RippleGainU1_AD620)

#define XL902RW_AD7608P

typedef enum XL902RWResult
{
	XL902RW_True,
	XL902RW_False
}XL902RWResult;

#define XL902U1Channal	4
#define XL902I1Channal	6


////定义纹波的采样点数
//#define RippleWaveDot		256			//相当与12.8KHz,此时处理器处理不过来
////定义需要进行纹波计算的点数
//#define RippleWaveDotCount	767		//此处对应直流采样16.7Hz的采样长度

//定义纹波的采样点数
#define RippleWaveDot		512			//相当与25.6KHz,此时处理器处理不过来
//定义需要进行纹波计算的点数
#define RippleWaveDotCount	1533		//此处对应直流采样16.7Hz的采样长度

////定义纹波的采样点数
//#define RippleWaveDot		2560		//相当与128KHz,此时处理器处理不过来
////定义需要进行纹波计算的点数
//#define RippleWaveDotCount	7665	//此处对应直流采样16.7Hz的采样长度

////定义纹波的采样点数
//#define RippleWaveDot		1024		//相当与51.2KHz
////定义需要进行纹波计算的点数
//#define RippleWaveDotCount	3066	//此处对应直流采样16.7Hz的采样长度

typedef struct XL902RWSampleData
{
	float U1;
	float I1;
}XL902RWSampleData;

extern OS_SEM g_Sem_RippleWave;

//纹波采样任务
void Sem_Control_XL902Ripple(void* arg);
XL902RWResult XL902RippleWave_ini(int SampleDot);
XL902RWResult XL902RWSample_stop(void);
XL902RWResult XL902RWSampleDot_set(int SampleDot);
//返回目前FIFO的长度，同时返回最终校准后的数据
int XL902RWSampleDataGet(XL902RWSampleData* data);
XL902RWResult XL902RWSample_run(void);
//外部读取纹波信息接口，输入参数为电压纹波系数，电流纹波系数，电压纹波幅度，电流纹波幅度的地址
void XL902RippleInf_get(float *U_persent, float *I_persent, float *U_A, float *I_A);
//外度读取纹波波形数据接口，入口参数为需要输出的点数
float* XL902RippleWaveGet_U(int dotnum);
float* XL902RippleWaveGet_I(int dotnum);
void Timer3Handler(void *pCBParam, uint32_t Event, void *pArg);

#endif /* XL902RIPPLEWAVE_H_ */
