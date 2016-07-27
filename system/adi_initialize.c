/*
** adi_initialize.c source file generated on 七月 3, 2015 at 14:46:00.
**
** Copyright (C) 2000-2015 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically. You should not modify this source file,
** as your changes will be lost if this source file is re-generated.
*/

#include <sys/platform.h>
#ifdef __ADI_HAS_SEC__
#include <services/int/adi_sec.h>
#endif

#include "adi_initialize.h"
#include "stdint.h"


extern int32_t adi_mcapi_Init(void);
extern int32_t adi_initpinmux(void);
extern int32_t adi_OS_Init(void);

int32_t adi_initComponents(void)
{
	int32_t result = 0;

#ifdef __ADI_HAS_SEC__
	result = adi_sec_Init();
#endif

	Myadi_gpio_Init();

	if (result == 0) {
		result = adi_initpinmux(); /* auto-generated code (order:0) */
	}

	if (result == 0) {
		result = adi_OS_Init(); /* auto-generated code (order:3) */
	}

	if (result == 0) {
		result = adi_mcapi_Init(); /* auto-generated code (order:6) */
	}

	return result;
}

int32_t Myadi_gpio_Init(void)		//此处根据程序中用到的外部callback函数的多少进行内存分配,具体修改MYADI_GPIO_CALLBACK_MEM_SIZE
{
	int32_t result = 0;
	static uint8_t gpioMemory[MYADI_GPIO_CALLBACK_MEM_SIZE];
	static uint32_t gpioMaxCallbacks;

    /* init the GPIO service */
	result = adi_gpio_Init(
			(void*)gpioMemory,
			MYADI_GPIO_CALLBACK_MEM_SIZE,
			&gpioMaxCallbacks);

	return result;
}
