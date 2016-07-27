#include "EnergyPulse.h"


static unsigned int EnergyPulse_Num;


//以下对电能脉冲端口进行初始话，利用管口中断进行电能处理。
//入口参数为电能表的电能脉冲常数
//管口中断时记得在#include "adi_initialize.h"文件中的中断内存分配处进行添加
EnergyPulseResult EnergyPulseCount_Ini(void)
{
	ADI_GPIO_RESULT result;

	EnergyPulse_Num = 0;

	result = adi_gpio_PinInterruptAssignment(
			ADI_GPIO_PIN_INTERRUPT_5,
			ADI_GPIO_PIN_ASSIGN_BYTE_1,
			ADI_GPIO_PIN_ASSIGN_PDL_PINT2);
	if(result != ADI_GPIO_SUCCESS)
	{
		return EnergyPulseFalse;
	}

	/* set GPIO input */
	result = adi_gpio_SetDirection(
			ADI_GPIO_PORT_G,
	    ADI_GPIO_PIN_8,
	    ADI_GPIO_DIRECTION_INPUT);
	if(result != ADI_GPIO_SUCCESS)
	{
		return EnergyPulseFalse;
	}

	/* set edge sense mode (PORT G is connected to Pin Interrupt 5) */
	result = adi_gpio_SetPinIntEdgeSense(
			ADI_GPIO_PIN_INTERRUPT_5,
	    ADI_GPIO_PIN_8,
	    ADI_GPIO_SENSE_RISING_EDGE);
	if(result != ADI_GPIO_SUCCESS)
	{
		return EnergyPulseFalse;
	}

	/* register pinint 0 callback */
	result = adi_gpio_RegisterCallback(
			ADI_GPIO_PIN_INTERRUPT_5,
	    ADI_GPIO_PIN_8,
	    EnergyPulse_Count,
	    (void*)0);
	if(result != ADI_GPIO_SUCCESS)
	{
		return EnergyPulseFalse;
	}

	/* set pin interrupt mask */
	result = adi_gpio_EnablePinInterruptMask(
			ADI_GPIO_PIN_INTERRUPT_5,
	    ADI_GPIO_PIN_8,
	    true);
	if(result != ADI_GPIO_SUCCESS)
	{
		return EnergyPulseFalse;
	}

	return EnergyPulseTrue;

}

EnergyPulseResult EnergyPulseCount_Enable(char state)
{
	ADI_GPIO_RESULT result;

	if(state == 0)
	{
	/* set pin interrupt mask */
		result = adi_gpio_EnablePinInterruptMask(
				ADI_GPIO_PIN_INTERRUPT_5,
			    ADI_GPIO_PIN_8,
			    false);
	}else
	{
		//复位数据
		EnergyPulse_Num = 0;
		/* set pin interrupt mask */
		result = adi_gpio_EnablePinInterruptMask(
				ADI_GPIO_PIN_INTERRUPT_5,
			    ADI_GPIO_PIN_8,
			    true);
	}

	if(result != ADI_GPIO_SUCCESS)
	{
		return EnergyPulseFalse;
	}

	return EnergyPulseTrue;
}


/*
 * GPIO event Callback function
 */
static void EnergyPulse_Count(ADI_GPIO_PIN_INTERRUPT ePinInt, uint32_t PinIntData,  void *pCBParam)
{
	switch(ePinInt)
	{
	case ADI_GPIO_PIN_INTERRUPT_5:

		if (PinIntData & ADI_GPIO_PIN_8)
		{
			EnergyPulse_Num++;
		}

		break;
	default:
		break;
	}

}

//外部可以利用该函数对当前的脉冲个数进行读取
unsigned int EnergyPulse_Get(void)
{

	return EnergyPulse_Num;
}

void EnergyPulseReset(void)
{
	EnergyPulse_Num = 0;

}


EnergyPulseResult EnergyPulseOut_Ini(void)
{
	ADI_GPIO_RESULT result;

	result = adi_gpio_SetDirection(
			    	ADI_GPIO_PORT_E,
			    	ADI_GPIO_PIN_14,
			    	ADI_GPIO_DIRECTION_OUTPUT);

	if(result != ADI_GPIO_SUCCESS)
	{
		return EnergyPulseTrue;
	}

	return EnergyPulseTrue;
}


