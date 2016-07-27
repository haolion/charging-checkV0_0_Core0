/*
 * 功能：电能脉冲
 * 版本：V1.0
 * 描述：运行于XL902主板上
 * 公司：深圳市星龙科技股份有限公司
 * 作者：张颖豪
 * 日期：2015/9/30
 * 备注：无
 */
#ifndef	ENERGYPULSE_H
#define ENERGYPULSE_H

#include <services/gpio/adi_gpio.h>


#include <services/gpio/adi_gpio.h>


#define EnergyPulse_debug

typedef enum EnergyPulseResult
{
	EnergyPulseTrue,
	EnergyPulseFalse
}EnergyPulseResult;

#define XL903EnergyPulseOut_Port	ADI_GPIO_PORT_E
#define XL903EnergyPulseOut_Pin		ADI_GPIO_PIN_14


//电能脉冲输入
EnergyPulseResult EnergyPulseCount_Ini(void);
EnergyPulseResult EnergyPulseCount_Enable(char state);//该函数很多地方调用时无效，是否跟timer一起开启中断有关
static void EnergyPulse_Count(ADI_GPIO_PIN_INTERRUPT ePinInt, uint32_t PinIntData,  void *pCBParam);
unsigned int EnergyPulse_Get(void);//外部可以利用该函数对当前的脉冲个数进行读取
void EnergyPulseReset(void);

EnergyPulseResult EnergyPulseOut_Ini(void);
float EnergyPulseOut_Auto(float Energy);//输入为需要发送的电能，返回为脉冲输出后剩下的电能

#endif
