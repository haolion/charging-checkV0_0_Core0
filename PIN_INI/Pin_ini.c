/*
 * 功能：引脚中断及服务函数
 * 版本：V1.0
 * 描述：ad7793的脉冲输入中断
 * 公司：深圳市星龙科技股份有限公司
 * 作者：张颖豪
 * 日期：2015/7/3
 * 备注：
 */
#include "Pin_ini.h"
#include <services/gpio/adi_gpio.h>
#include <stdint.h>


/* memory for two callbacks */
#define GPIO_MEMORY_SIZE (ADI_GPIO_CALLBACK_MEM_SIZE*2u)

void ad7793_RDY_Int_Init(void)
{
	ADI_GPIO_RESULT result;
	static uint8_t gpioMemory[ADI_GPIO_CALLBACK_MEM_SIZE];
	uint32_t gpioMaxCallbacks;
    /* init the GPIO service */
	result = adi_gpio_Init(
			(void*)gpioMemory,
			GPIO_MEMORY_SIZE,
			&gpioMaxCallbacks);

	/* set GPIO input */
	result = adi_gpio_SetDirection(
			ADI_GPIO_PORT_G,
	    ADI_GPIO_PIN_11,
	    ADI_GPIO_DIRECTION_INPUT);

	/* set edge sense mode (PORT G is connected to Pin Interrupt 5) */
	result = adi_gpio_SetPinIntEdgeSense(
			ADI_GPIO_PIN_INTERRUPT_5,
	    ADI_GPIO_PIN_11,
	    ADI_GPIO_SENSE_RISING_EDGE);

	/* register pinint 0 callback */
	result = adi_gpio_RegisterCallback(
			ADI_GPIO_PIN_INTERRUPT_5,
	    ADI_GPIO_PIN_11,
	    pinIntCallback,
	    (void*)0);

	/* set pin interrupt mask */
	result = adi_gpio_EnablePinInterruptMask(
			ADI_GPIO_PIN_INTERRUPT_5,
	    ADI_GPIO_PIN_11,
	    true);

}

/*
 * GPIO event Callback function
 */
static void pinIntCallback(ADI_GPIO_PIN_INTERRUPT ePinInt, uint32_t PinIntData,  void *pCBParam)
{
	switch(ePinInt)
	{
	case ADI_GPIO_PIN_INTERRUPT_5:
		/* push button 1 */
		if (PinIntData & ADI_GPIO_PIN_11)
		{
			/* toggle LED 1 */
			adi_gpio_Toggle(ADI_GPIO_PORT_E, ADI_GPIO_PIN_14);
//			printf("pinIntG11");
		}
		break;
//	case ADI_GPIO_PIN_INTERRUPT_3:
//		/* push button 2 */
//		if (PinIntData & ADI_GPIO_PIN_1)
//		{
//			/* toggle LED 2 */
//			adi_gpio_Toggle(ADI_GPIO_PORT_G, ADI_GPIO_PIN_15);
//		}
//		break;
	default:
		break;
	}

	/* reset the exit counter */
//	count = 0u;
}
