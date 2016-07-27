/*****************************************************************************
 * charging-checkV0_0_Core0.h
 *****************************************************************************/

#ifndef __CHARGING_CHECKV0_0_CORE0_H__
#define __CHARGING_CHECKV0_0_CORE0_H__
#include <ccblkfn.h>
#include "adi_initialize.h"
#include <stdio.h>
#include "os.h"
#include "../CGU/cgu_init.h"
#include <services/gpio/adi_gpio.h>
#include <cdefBF609.h>
#include <stdio.h>
#include <services/int/adi_sec.h>
#include <services/pwr/adi_pwr.h>
#include <services/gpio/adi_gpio.h>
#include "Sem_Task_Uart.h"
#include "../UARTx/UARTx.h"
#include "../TIMER/timer.h"
#include "../PIN_INI/Pin_ini.h"
#include "Task_adc.h"
#include "../INIT_MEMORY/init_memory.h"
#include "../XL902_COMMUNICATION/XL902_Communication.h"
#include "../Charger-BMS_Communication/CBMS_APLayer.h"
#include "../CAN/CAN2.0b.h"
#include "../SWITCH/XL902Switch.h"
#include "../soft_update/Flash/Flash_Init.h"
#include "../SWITCH/XL902Switch.h"
#include "../LTC2943/LTC2943.h"
/* Add your custom header content here */

/*
 * Macros used in the example
 */

/* Example result definitions */
#define FAILED              (-1)
#define PASSED              0

#define NUM_LED_TASKS 1
#define NUM_AD7793_TASKS 1
#define NUM_UART_TASKS	1

/* memory for two callbacks */
#define GPIO_MEMORY_SIZE (ADI_GPIO_CALLBACK_MEM_SIZE*2u)


#define PRODUCER_PRIO ((OS_PRIO)5)
#define CONSUMER_PRIO (PRODUCER_PRIO +2u)
#define CONSUMER2_PRIO (PRODUCER_PRIO +2u)
#define AD7793_PRIO 7u
#define UART_Frame_Get_PRIO	6u
#define XL902_Communication_PRIO	8u
#define LED_WD_PRIO	7u
#define EnergyMeasure_PRIO	8u
#define Pulse_PRIO	7u
#define ConnectCheck_PRIO	4u
#define Can_PRIO	5u
#define CBMS_PRIO	5u
#define Ripple_PRIO	5u


#define TASK_STK_2SIZE ((CPU_STK)0x800u)
#define TASK_STK_SIZE ((CPU_STK)0x400u)

/* Global signals */
extern OS_SEM g_Sem_Uart_1;
extern OS_SEM g_Sem_Uart_2;

void BeepIni(void);
void TaskRunLed(void* LedArg);

#endif /* __UCOS3_UART0_CONTROL_AD7793_CORE0_H__ */
