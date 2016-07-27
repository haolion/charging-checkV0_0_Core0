///*
// * timer.h
// *
// *  Created on: 2015-3-17
// *      Author: hql
// */
//
//#ifndef TIMER_H_
//#define TIMER_H_
//
//void timer_init(void);
//static void Timer5Handler(void *pCBParam, uint32_t Event, void *pArg);
//
//#endif /* TIMER_H_ */


#ifndef TIMER_H_
#define TIMER_H_


#include <services/tmr/adi_tmr.h>
#include "os.h"
#include "../XL902RippleWave/XL902RippleWave.h"

//#define Timer_Debug

typedef enum TimerResult
{
	Timer_True,
	Timer_False
}TimerResult;

typedef enum Timerx
{
	Timer0 = 0,			//
	Timer1,				//
	Timer2,				//XL902用于档位控制
	Timer3,				//XL902纹波采样定时器
	Timer4,				//XL902电能计算时钟基准
	Timer5,
	Timer6,				//
	Timer7				//
}Timerx;


TimerResult timer_init(Timerx x, int SampleRate, void* const pCBParam);//初始化定时器
TimerResult timer_open(Timerx x);//打开定时器
TimerResult timer_close(Timerx x);//关闭定时器
static void Timer5Handler(void *pCBParam, uint32_t Event, void *pArg);

extern void Timer3Handler(void *pCBParam, uint32_t Event, void *pArg);
extern void Timer1Handler(void *pCBParam, uint32_t Event, void *pArg);
extern void Timer2Handler(void *pCBParam, uint32_t Event, void *pArg);
//extern void Timer6Handler(void *pCBParam, uint32_t Event, void *pArg);

#endif /* TIMER_H_ */

