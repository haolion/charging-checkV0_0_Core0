
#ifndef XL902SWITCH_H
#define XL902SWITCH_H

#include <services/gpio/adi_gpio.h>
#include "../soft_update/Flash/Flash_API.h"
#include "../UARTx/UARTx.h"
#include "../src/Global_define.h"
#include "../TIMER/timer.h"

//外部变量
//串口控制信息结构体
extern UART_Data_Struct UART0_Data;//串口结构体，用于缓存

//define the GPIO
#define SER_Port	ADI_GPIO_PORT_B
#define RCLK_Port	ADI_GPIO_PORT_B
#define SRCLK_Port	ADI_GPIO_PORT_B
#define SRCLR_Port	ADI_GPIO_PORT_B

#define SER_Pin		ADI_GPIO_PIN_12
#define RCLK_Pin	ADI_GPIO_PIN_11
#define SRCLK_Pin	ADI_GPIO_PIN_10
#define SRCLR_Pin	ADI_GPIO_PIN_9

//define the state
#define XL902SWITCH_DEBUG
typedef enum
{
	XL902Switch_True,
	XL902Switch_False
}XL902SWITCH_RESULT;



//define the param
typedef enum
{
	U1A = 0,
	U1B,
	I1A,
	I1B,
	I1C
//	U200V,
//	U100V,
//	U45V,
//	U15V
}XL902SWITCH_DATA;
#define XL902Gears_Num	5	//档位数量
#define XL902Channal_Num	2//通道的数量
#define URMax		U1A	//定义最大电压档位
#define IRMax		I1B	//定义最大电流档位
#define XL902CF_A	0		//幅度校准系数
#define XL902CF_Q	1		//角度校准系数(直流下该参数可忽略)
//量程，由以下修改，暂时不使用配置文件方式,
#ifdef XL902_200A
#define U1ARange	750		//V
#define U1BRange	450		//V
#define I1ARange	220		//A 200A规格时该档位没用
#define I1BRange	200		//A
#define I1CRange	50		//A
#define USwitchMax	U1A
#define ISwitchMax	I1B
#define UrangeMax	U1ARange
#define UrangeMin	U1BRange
#define IrangeMax	I1BRange
#define IrangeMin	I1CRange
#else
#define U1ARange	750		//V
#define U1BRange	450		//V
#define I1ARange	400		//A
#define I1BRange	200		//A
#define I1CRange	80		//A
#define USwitchMax	U1A
#define ISwitchMax	I1A
#define UrangeMax	U1ARange
#define UrangeMin	U1BRange
#define IrangeMax	I1ARange
#define IrangeMin	I1CRange
#endif

#define RangeMargin	10		//此处余量为了防止切换点的乒乓效应，假如采样器为积分采样，此处需要设大点，假如是采样保持的采样器，可以设置小点
							//没校准时此处裕值需要大点，后面修正
#define URangeMarginUp		0.1
#define URangeMarginDown	10
#define IRangeMarginUp		0.1
#define	IRangeMarginDown	10

typedef enum XL902ChannalType
{
	XL902Channal_U,
	XL902Channal_I
}XL902ChannalType;

#define XL902SWITCHDELAY_TIME	6		//此处时间为x乘以2ns


//用于管理继电器切换的时钟频率
#define XL902TimeTick	1000		//1ms中断一次
//用于继电器保护时间
#define XL902SwitchProtectTime	25	//继电器保护时间，单位ms
//定义档位状态结构体
typedef struct XL902SwitchState
{
	int CloseTime;				//此处将登记需要关闭档位的时刻
	XL902SWITCH_DATA gear;
	char flag_switching;		//装换中状态位
}XL902SwitchState;

#define SRCLK(x)	( (x) ? (adi_gpio_Set(SRCLK_Port , SRCLK_Pin)):(adi_gpio_Clear(SRCLK_Port , SRCLK_Pin)))
#define SER(x)		( (x) ? (adi_gpio_Set(SER_Port , SER_Pin)):(adi_gpio_Clear(SER_Port , SER_Pin)))
#define RCLK(x)		( (x) ? (adi_gpio_Set(RCLK_Port , RCLK_Pin)):(adi_gpio_Clear(RCLK_Port , RCLK_Pin)))
#define SRCLR(x)	( (x) ? (adi_gpio_Set(SRCLR_Port , SRCLR_Pin)):(adi_gpio_Clear(SRCLR_Port , SRCLR_Pin)))


XL902SWITCH_RESULT XL902Switch_ini(void);
XL902SWITCH_RESULT XL902SwitchData_set(XL902SWITCH_DATA gear);
XL902SWITCH_RESULT XL902SwitchDataSet_protect(XL902SWITCH_DATA gear);
XL902SWITCH_RESULT XL902SwitchAF_ini(float *UA_CF,float *IA_CF, float *UQ_CF, float *IQ_CF);
XL902SWITCH_RESULT XL902SwitchRange_Max(void);
char XL902USwitchingCheck(void);
char XL902ISwitchingCheck(void);
void Timer2Handler(void *pCBParam, uint32_t Event, void *pArg);


XL902SWITCH_DATA CurrentISwitchRead(void);
XL902SWITCH_DATA CurrentUSwitchRead(void);
int* XL902SwitchRangeReand(void);
//校准系数
float UCF_GetA(char s);
float UCF_GetQ(char s);
float ICF_GetA(char s);
float ICF_GetQ(char s);
XL902SWITCH_RESULT XL902FlashCF_Write(float A_CF,float Q_CF, XL902SWITCH_DATA range);
void XL902FlashCF_Read(float *A_CF,float *Q_CF, XL902SWITCH_DATA range);
XL902SWITCH_RESULT XL902FlashCF_Reset(void);
//零位校准
XL902SWITCH_RESULT XL902SwitchZC_ini(float *UZC,float *IZC);
XL902SWITCH_RESULT XL902FlashZC_Write(float ZF,XL902SWITCH_DATA range);
void XL902FlashZC_Read(float *ZC, XL902SWITCH_DATA range);
XL902SWITCH_RESULT XL902FlashZC_Reset(void);
float ZC_Get(char s);
//纹波校准
XL902SWITCH_RESULT XL902SwitchRF_ini(void);
XL902SWITCH_RESULT XL902FlashRF_Write(float RF,XL902SWITCH_DATA range);
void XL902FlashRF_Read(float *RF, XL902SWITCH_DATA range);
XL902SWITCH_RESULT XL902FlashRF_Reset(void);

//自动校准
XL902SWITCH_RESULT XL902SwitchAuto(float value, float *A_CF,float *Q_CF, XL902ChannalType type);
XL902SWITCH_RESULT XL902SwitchAuto_ZC(float value, float *A_CF,float *Q_CF, float *ZC,
										XL902SWITCH_DATA *Gear, XL902ChannalType type);
XL902SWITCH_RESULT XL902SwitchAuto_protect(float value, float *A_CF,float *Q_CF, float *ZC,
										XL902SWITCH_DATA *Gear, XL902ChannalType type);


#endif







