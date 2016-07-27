/*
** adi_initialize.h
**
** Copyright (C) 2000-2013 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically. You should not modify this source file,
** as your changes will be lost if this source file is re-generated.
*/

#ifndef __ADI_COMPONENT_INIT_H__
#define __ADI_COMPONENT_INIT_H__
#include <stdint.h>
#include <services/gpio/adi_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MYADI_GPIO_CALLBACK_MEM_SIZE	ADI_GPIO_CALLBACK_MEM_SIZE*2	//此处根据程序中用到的外部callback函数的多少进行内存分配
																		//修改时只需修改后面乘数的数量
//定义系统时钟信息
#ifndef MYCLK_IFO
#define MYCLK_IFO
#define myCLKIN         (20 * MHZTOHZ)   /* should corresponding to the fact input osc. */
#define myCORE_MAX      (400 * MHZTOHZ)
#define mySYSCLK_MAX    (200 * MHZTOHZ)
#define mySCLK_MAX      (200 * MHZTOHZ)
#define myVCO_MIN       (72     * MHZTOHZ)
#endif

/* Declare "adi_initComponents()" */
int32_t adi_initComponents(void);
int32_t Myadi_gpio_Init(void);		//此处根据程序中用到的外部callback函数的多少进行内存分配

#ifdef __cplusplus
}
#endif

#endif /* __ADI_COMPONENT_INIT_H__ */
