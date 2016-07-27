/*
 * AD7608P.h
 *
 *  Created on: 2016-2-26
 *      Author: 豪_lion
 */

#ifndef AD7608P_H_
#define AD7608P_H_
#include <services/gpio/adi_gpio.h>
#include "../DataStruct/CircularQueue.h"
#include "string.h"
#include "stdlib_bf.h"
#include "os.h"

#define XL903Switch		//使用了XL903Switch的换挡驱动时需要该定义，用于档位标记
#define Debug_AD7608P

#ifdef Debug_AD7608P
#include "../UARTx/UARTx.h"
extern UART_Data_Struct UART0_Data;//串口结构体，用于缓存
#endif

typedef enum AD7608PResult
{
	AD7608P_True = 1,
	AD7608P_False = 0
}AD7608PResult;

#define AD7608DataBus_PORT	ADI_GPIO_PORT_F
#define AD7608_CS_PORT		ADI_GPIO_PORT_E
#define AD7608_OK_PORT		ADI_GPIO_PORT_E
#define AD7608_FD_PORT		ADI_GPIO_PORT_E
#define AD7608_CONVSTA_PORT	ADI_GPIO_PORT_E
#define AD7608_CONVSTB_PORT	ADI_GPIO_PORT_D
#define AD7608_RD_PORT		ADI_GPIO_PORT_D
#define AD7608_RST_PORT		ADI_GPIO_PORT_E
#define AD7608_RANGE_PORT	ADI_GPIO_PORT_E
#define AD7608_ADOS0_PORT	ADI_GPIO_PORT_E
#define AD7608_ADOS1_PORT	ADI_GPIO_PORT_E
#define AD7608_ADOS2_PORT	ADI_GPIO_PORT_E

#define AD7608DataBus_PIN	0x0000FFFFu		//16位并口
#define AD7608_CS_PIN		ADI_GPIO_PIN_3
#define AD7608_OK_PIN		ADI_GPIO_PIN_4
#define AD7608_FD_PIN		ADI_GPIO_PIN_0
#define AD7608_CONVSTA_PIN	ADI_GPIO_PIN_1
#define AD7608_CONVSTB_PIN	ADI_GPIO_PIN_12
#define AD7608_RD_PIN		ADI_GPIO_PIN_15
#define AD7608_RST_PIN		ADI_GPIO_PIN_2
#define AD7608_RANGE_PIN	ADI_GPIO_PIN_5
#define AD7608_ADOS0_PIN	ADI_GPIO_PIN_9
#define AD7608_ADOS1_PIN	ADI_GPIO_PIN_8
#define AD7608_ADOS2_PIN	ADI_GPIO_PIN_7

#define AD7608POS0(x)	( (x) ? (adi_gpio_Set(AD7608_ADOS0_PORT , AD7608_ADOS0_PIN)):(adi_gpio_Clear(AD7608_ADOS0_PORT , AD7608_ADOS0_PIN)))
#define AD7608POS1(x)	( (x) ? (adi_gpio_Set(AD7608_ADOS1_PORT , AD7608_ADOS1_PIN)):(adi_gpio_Clear(AD7608_ADOS1_PORT , AD7608_ADOS1_PIN)))
#define AD7608POS2(x)	( (x) ? (adi_gpio_Set(AD7608_ADOS2_PORT , AD7608_ADOS2_PIN)):(adi_gpio_Clear(AD7608_ADOS2_PORT , AD7608_ADOS2_PIN)))
#define AD7608PRANGE(x)	( (x) ? (adi_gpio_Set(AD7608_RANGE_PORT , AD7608_RANGE_PIN)):(adi_gpio_Clear(AD7608_RANGE_PORT , AD7608_RANGE_PIN)))
#define AD7608PRST(x)	( (x) ? (adi_gpio_Set(AD7608_RST_PORT , AD7608_RST_PIN)):(adi_gpio_Clear(AD7608_RST_PORT , AD7608_RST_PIN)))
#define AD7608PCONVSTA(x)	( (x) ? (adi_gpio_Set(AD7608_CONVSTA_PORT , AD7608_CONVSTA_PIN)):(adi_gpio_Clear(AD7608_CONVSTA_PORT , AD7608_CONVSTA_PIN)))
#define AD7608PCONVSTB(x)	( (x) ? (adi_gpio_Set(AD7608_CONVSTB_PORT , AD7608_CONVSTB_PIN)):(adi_gpio_Clear(AD7608_CONVSTB_PORT , AD7608_CONVSTB_PIN)))
#define AD7608PCS(x)	( (x) ? (adi_gpio_Set(AD7608_CS_PORT , AD7608_CS_PIN)):(adi_gpio_Clear(AD7608_CS_PORT , AD7608_CS_PIN)))
#define AD7608PRD(x)	( (x) ? (adi_gpio_Set(AD7608_RD_PORT , AD7608_RD_PIN)):(adi_gpio_Clear(AD7608_RD_PORT , AD7608_RD_PIN)))


//AD7608状态定义
typedef enum AD7608POSRatio
{
	AD7608POSRNone,
	AD7608POSR2,
	AD7608POSR4,
	AD7608POSR8,
	AD7608POSR16,
	AD7608POSR32,
	AD7608POSR64
}AD7608POSRatio;

typedef enum AD7608PRange
{
	AD7608PRange5V,
	AD7608PRange10V
}AD7608PRange;

typedef enum AD7608PSamMode
{
	AD7608PSam_BOTH,		//两路同步采样
	AD7608PSam_AB,			//先采A后采B,用于某些场合需要移相
	AD7608PSam_BA,			//先采B后采A,用于某些场合需要移相
}AD7608PSamMode;

#define AD7608_BuffNum		100				//缓存单位的数量
#define AD7608_BuffLength	8				//每个缓存长度，里面为8个浮点数
//以下结构体中的数据保存的为上一次采样值，档位保存的为当前值。
typedef struct AD7608PDataFrame
{
	int data[8];		//每次采样保存8个通道数据
	char gear[8];			//当前数据对应的档位
	int range;			//当前7608的range档位
}AD7608PDataFrame;

AD7608PResult AD7608P_ini(void);
AD7608PResult AD7608PDataGet(AD7608PDataFrame *DataBuff);//从获取的FIFO中读取数据，包括档位值
AD7608PResult AD7608POSRatioSet(AD7608POSRatio OSRatio);//设置过采样率
AD7608PResult AD7608PRangeSet(AD7608PRange Range);//设置输入检测幅度，有5V和10V可以选择
void AD7608PReset(void);//复位
AD7608PResult AD7608PSample(AD7608PSamMode mode,float phase);//采样，目前只有双通道同步采样，phase参数无用
int AD7608PBuffNum(void);//获取FIFO中的数据长度
void AD7608PGearRegiser(char channal, char gear);//当前通道的档位状态
void AD7608PFrameAddToFIFO(void);//将采样数据移入FIFO
void AD7608PStop(void);//停止
int AD7608PRangeRead(void);//获取档位幅度值，为芯片内部PGA,与外部档位区分
AD7608PDataFrame* AD7608PCurrentDataGet(void);//获取当前采样值
AD7608PResult AD7608PSampleboth(void);//双通道同步采样
void AD7608PSamDotBegin(void);//开始同步采样脉冲
AD7608PResult AD7608PSamDateGet(void);//同步脉冲之后的数据获取，和AD7608PSamDotBegin一起使用时与AD7608PSample功能相同，构成不同

#endif /* AD7608P_H_ */
