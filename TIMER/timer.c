///*
// * timer.c
// *
// *  Created on: 2015-3-17
// *      Author: hql
// */
//
//#include <services/tmr/adi_tmr.h>
//#include <services/pwr/adi_pwr.h>
//#include <services/gpio/adi_gpio.h>
//#include <stdio.h>
//#include "timer.h"
//#include "os.h"
//
//uint8_t Timer5Memory[ADI_TMR_MEMORY];
//static ADI_TMR_HANDLE   ghTimer5;
//extern OS_SEM g_Sem_7793_Run;
//#define TIMER5	5
//void timer_init(void)
//{
//	uint32_t   fsysclk,fsclk0,fsclk1;
//	adi_pwr_GetSystemFreq(&fsysclk,&fsclk0,&fsclk1);
//	adi_tmr_Open(TIMER5,Timer5Memory,ADI_TMR_MEMORY,Timer5Handler,NULL,&ghTimer5);
//	adi_tmr_SetClkInSource(ghTimer5,ADI_TMR_CLKIN_SYSCLK);
//	if((adi_tmr_SetMode(ghTimer5,ADI_TMR_MODE_CONTINUOUS_PWMOUT))!= ADI_TMR_SUCCESS)
//	{
//		printf("set timer mode error\n");
//	}
//
//	adi_tmr_SetPeriod(ghTimer5,fsclk0);
//	adi_tmr_SetDelay(ghTimer5,0);
//	adi_tmr_SetWidth(ghTimer5,fsclk0/2);
//
//
//	if(adi_tmr_SetIRQMode(ghTimer5,ADI_TMR_IRQMODE_PERIOD) != ADI_TMR_SUCCESS)
//	{
//		printf("set timer IRQmode error\n");
//	}
////	if(adi_tmr_Enable(ghTimer5,true)!= ADI_TMR_SUCCESS)
////	{
////		printf("timer start error\n");
////	}
//
//}
//
//static void Timer5Handler(void *pCBParam, uint32_t Event, void *pArg)
//{
//	OS_ERR err;
//	switch(Event)
//	    {
//	        case ADI_TMR_EVENT_DATA_INT:
//	            //gbTimerInt = true;
////	        	adi_gpio_Toggle(ADI_GPIO_PORT_G,ADI_GPIO_PIN_12);
////	        	OSSemPost(&g_Sem_7793_Run, OS_OPT_POST_1, &err);
//	        	asm("nop;");
//	            break;
//
//	        default:
//	            //asm("emuexcpt;");
//	            break;
//	    }
//
//	    return;
//}



/*
 * timer.c
 *
 *  Created on: 2015-3-17
 *      Author: hql
 */

#include <services/tmr/adi_tmr.h>
#include <services/pwr/adi_pwr.h>
#include <services/gpio/adi_gpio.h>
#include <stdio.h>
#include "timer.h"

static uint8_t* TimerMemory[8];
static ADI_TMR_HANDLE   ghTimer[8];
//static void Timer0Handler(void *pCBParam, uint32_t Event, void *pArg);

TimerResult timer_init(Timerx x, int SampleRate, void* const pCBParam)
{
	uint32_t   fsysclk,fsclk0,fsclk1;
	int timerperiod;
	//获取系统时钟
	adi_pwr_GetSystemFreq(&fsysclk,&fsclk0,&fsclk1);
	//分配内存
	TimerMemory[x] = heap_calloc(1, sizeof(unsigned char), ADI_TMR_MEMORY);

	switch(x)
	{
//	case Timer0:
//		adi_tmr_Open(x,TimerMemory[x],ADI_TMR_MEMORY,Timer0Handler,pCBParam,&ghTimer[x]);
//		break;
	case Timer1:
		adi_tmr_Open(x,TimerMemory[x],ADI_TMR_MEMORY,Timer1Handler,pCBParam,&ghTimer[x]);
		break;
	case Timer2:
		adi_tmr_Open(x,TimerMemory[x],ADI_TMR_MEMORY,Timer2Handler,pCBParam,&ghTimer[x]);
		break;
	case Timer3:
		adi_tmr_Open(x,TimerMemory[x],ADI_TMR_MEMORY,Timer3Handler,pCBParam,&ghTimer[x]);
		break;
	case Timer4:
		adi_tmr_Open(x,TimerMemory[x],ADI_TMR_MEMORY,Timer4Handler,pCBParam,&ghTimer[x]);
		break;
//	case Timer6:
//		adi_tmr_Open(x,TimerMemory[x],ADI_TMR_MEMORY,Timer6Handler,pCBParam,&ghTimer[x]);
//		break;
	default:
		return Timer_False;
	}
	adi_tmr_SetClkInSource(ghTimer[x],ADI_TMR_CLKIN_SYSCLK);
	if((adi_tmr_SetMode(ghTimer[x],ADI_TMR_MODE_CONTINUOUS_PWMOUT))!= ADI_TMR_SUCCESS)
	{
		printf("set timer mode error\n");
	}

	timerperiod = fsclk0/SampleRate;

	adi_tmr_SetPeriod(ghTimer[x],timerperiod);
	adi_tmr_SetDelay(ghTimer[x],0);
	adi_tmr_SetWidth(ghTimer[x],timerperiod/2);


	if(adi_tmr_SetIRQMode(ghTimer[x],ADI_TMR_IRQMODE_PERIOD) != ADI_TMR_SUCCESS)
	{
		printf("set timer%d IRQmode error\n", x);
	}
//	if(adi_tmr_Enable(ghTimer[x],true)!= ADI_TMR_SUCCESS)
//	{
//		printf("timer start error\n");
//	}

	return Timer_True;
}

TimerResult timer_open(Timerx x)
{
	if(adi_tmr_Enable(ghTimer[x],true)!= ADI_TMR_SUCCESS)
	{
#ifdef Timer_Debug
		printf("timer start error\n");
#endif
		return Timer_False;
	}
	return Timer_True;
}

TimerResult timer_close(Timerx x)
{
	if(adi_tmr_Enable(ghTimer[x],false)!= ADI_TMR_SUCCESS)
	{
#ifdef Timer_Debug
		printf("timer start error\n");
#endif
		return Timer_False;
	}
	return Timer_True;
}


//static void Timer0Handler(void *pCBParam, uint32_t Event, void *pArg)
//{
//	OS_ERR err;
//	switch(Event)
//	    {
//	        case ADI_TMR_EVENT_DATA_INT:
//
//	            break;
//
//	        default:
//
//	            break;
//	    }
//
//	    return;
//}

static void Timer1Handler(void *pCBParam, uint32_t Event, void *pArg)
{
	OS_ERR err;
	switch(Event)
	    {
	        case ADI_TMR_EVENT_DATA_INT:

	            break;

	        default:

	            break;
	    }

	    return;
}

//static void Timer2Handler(void *pCBParam, uint32_t Event, void *pArg)
//{
//	OS_ERR err;
//	switch(Event)
//	    {
//	        case ADI_TMR_EVENT_DATA_INT:
//
//	            break;
//
//	        default:
//
//	            break;
//	    }
//
//	    return;
//}

//static void Timer3Handler(void *pCBParam, uint32_t Event, void *pArg)
//{
//	OS_ERR err;
//	switch(Event)
//	    {
//	        case ADI_TMR_EVENT_DATA_INT:
//
//	            break;
//
//	        default:
//
//	            break;
//	    }
//
//	    return;
//}

//static void Timer4Handler(void *pCBParam, uint32_t Event, void *pArg)
//{
//	OS_ERR err;
//	switch(Event)
//	    {
//	        case ADI_TMR_EVENT_DATA_INT:
//
//	            break;
//
//	        default:
//
//	            break;
//	    }
//
//	    return;
//}

static void Timer5Handler(void *pCBParam, uint32_t Event, void *pArg)
{
	OS_ERR err;
	switch(Event)
	    {
	        case ADI_TMR_EVENT_DATA_INT:
	            //gbTimerInt = true;
//	        	adi_gpio_Toggle(ADI_GPIO_PORT_G,ADI_GPIO_PIN_12);
//	        	OSSemPost(&g_Sem_7793_Run, OS_OPT_POST_1, &err);
	        	asm("nop;");
	            break;

	        default:
	            //asm("emuexcpt;");
	            break;
	    }

	    return;
}



//static void Timer7Handler(void *pCBParam, uint32_t Event, void *pArg)
//{
//	OS_ERR err;
//	switch(Event)
//	    {
//	        case ADI_TMR_EVENT_DATA_INT:
//
//	            break;
//
//	        default:
//
//	            break;
//	    }
//
//	    return;
//}



