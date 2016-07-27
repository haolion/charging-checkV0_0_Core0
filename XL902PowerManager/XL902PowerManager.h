/*
 * XL902PowerManager.h
 *
 *  Created on: 2016-4-5
 *      Author: 豪_lion
 */

#ifndef XL902POWERMANAGER_H_
#define XL902POWERMANAGER_H_

#include "../AD7608/AD7608P.h"

#define XL902PowerManager_debug

//此处利用AD7608作为电池的电量检测
#define XL902PowerManager_AD7608
//用于采样平均的点数
#define XL902PowerSamTimes	10
//电池到电池检测口的压降
//#define XL902PowerSamCheck	0.95


typedef enum XL902PowerManagerResult
{
	XL902PowerManagerTrue,
	XL902PowerManagerFalse
}XL902PowerManagerResult;

enum
{
	SOC100 = 0,
	SOC95,
	SOC90,
	SOC85,
	SOC80,
	SOC75,
	SOC70,
	SOC65,
	SOC60,
	SOC55,
	SOC50,
	SOC45,
	SOC40,
	SOC35,
	SOC30,
	SOC25,
	SOC20,
	SOC15,
	SOC10,
	SOC5,
	SOC0,
};

typedef struct XL902BatInf
{
	char XL902SOC;
	float XL902BATU;
}XL902BatInf;


XL902PowerManagerResult XL902PowerManagerIni(void);
float XL902BatVCheck(void);//返回数据为电池电压
int XL902PowerLeft(void);//返回数据为百分比


#endif /* XL902POWERMANAGER_H_ */
