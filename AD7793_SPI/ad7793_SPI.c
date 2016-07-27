#include "../AD7793_SPI/ad7793_SPI.h"
#include "os.h"
#include <services/gpio/adi_gpio.h>
#include <drivers\uart\adi_uart.h>
#include "stdio.h"
#include "math.h"
#include <defbf609.h>
AD7793_CS AD7793_CS2,AD7793_CS3;
/* memory for two callbacks */
#define GPIO_MEMORY_SIZE (ADI_GPIO_CALLBACK_MEM_SIZE*2u)

static void pinDIntCallback(ADI_GPIO_PIN_INTERRUPT ePinInt, uint32_t PinIntData,  void *pCBParam);
//static void pinDIntCallback(unsigned int  iid,void* handlerArg);
//delay sclk numbers of core clk.
void ad7793Delay(char sclk)
{
	int i;
	int j = sclk * 1000;
	
	for(i=0;i<j;i++)
	{
		asm("nop;");
		asm("nop;");
	}
	
}

void ad7793Reset(AD7793_CS cs)
{
	OS_ERR err;

	SpiCS_Set(cs.Spix, cs.SpiCsx);		//select the chip first
	ad7793WriteByte(cs, 0xff);
	ad7793WriteByte(cs, 0xff);
	ad7793WriteByte(cs, 0xff);
	ad7793WriteByte(cs, 0xff);
	ad7793WriteByte(cs, 0xff);
	ad7793WriteByte(cs, 0xff);
	SpiCS_Clear(cs.Spix, cs.SpiCsx);	//deselect the chip
	OSTimeDly(100u,OS_OPT_TIME_DLY,&err);		//waitting reset to be completed
}
//write one byte data to ad7793
//cs:chip select
//byte:the data to be writed via spi bus
//chip select not include,user must enable the chip select first
#ifdef RUNINRAM
section("ad_cal")
#endif
void ad7793WriteByte(AD7793_CS cs, unsigned char byte)
{
	SPIByte_Out(cs.Spix, byte);
}

//write the communications register
//this Subroutine call the ad7793WriteByte 
#ifdef RUNINRAM
section("ad_cal")
#endif
void ad7793WrComReg(AD7793_CS cs,unsigned char reg)
{
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	ad7793WriteByte(cs, reg);
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	ad7793Delay(10);
}

//read one byte of converted data
#ifdef RUNINRAM
section("ad_cal")
#endif
unsigned long ad7793ReadData(AD7793_CS cs)
{
	unsigned char byte;
	unsigned long value;
	
	value=0;
	SpiCS_Set(cs.Spix, cs.SpiCsx);		//select the chip first
	ad7793WrComReg(cs, CR_READ_DATA);		//write the communication register first to inform 7793 that the next operation is a read.
	SpiCS_Clear(cs.Spix, cs.SpiCsx);		//deselect the chip
	
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	byte=SPIByte_In(cs.Spix);
	value |=((unsigned long)byte)<<16;
	
	byte=SPIByte_In(cs.Spix);
	value |=((unsigned long)byte)<<8;
	
	byte=SPIByte_In(cs.Spix);
	value |=byte;
	
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	ad7793Delay(10);
	return value;
}

unsigned long ad7793Read24Bit(AD7793_CS cs)
{
	unsigned char byte;
	unsigned long value = 0;

	byte=SPIByte_In(cs.Spix);
	value |=((unsigned long)byte)<<16;

	byte=SPIByte_In(cs.Spix);
	value |=((unsigned long)byte)<<8;

	byte=SPIByte_In(cs.Spix);
	value |=byte;

	return value;
}

//read the status register
unsigned char ad7793ReadStaReg(AD7793_CS cs)
{
	unsigned char byte;
		
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	ad7793WrComReg(cs, CR_READ_STATUS);	//write the communication register first to inform 7793 that the next operation is a read.
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	
	ad7793Delay(20);
	
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	byte=SPIByte_In(cs.Spix);
	SpiCS_Clear(cs.Spix, cs.SpiCsx);

	ad7793Delay(10);
	return byte;
}

//read the mode register
//there are two bytes to be read
unsigned int ad7793ReadModeReg(AD7793_CS cs)
{
	unsigned int byte;
		
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	ad7793WrComReg(cs, CR_READ_MODE);	//write the communication register first to inform 7793 that the next operation is a read.
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	
	ad7793Delay(20);
	
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	byte=SPIByte_In(cs.Spix);
	byte<<=8;
	byte|=SPIByte_In(cs.Spix);
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	ad7793Delay(10);
	return byte;
}

unsigned int ad7793ReadConfigReg(AD7793_CS cs)
{
	unsigned int byte;
		
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	ad7793WrComReg(cs, CR_READ_CONFIG);	//write the communication register first to inform 7793 that the next operation is a read.
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	
	ad7793Delay(20);
	
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	byte=SPIByte_In(cs.Spix);
//	byte = 0x08;
	byte<<=8;
	byte|=SPIByte_In(cs.Spix);
//	byte |= 0x07;
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	ad7793Delay(10);
	return byte;
}

unsigned char ad7793ReadIDReg(AD7793_CS cs)
{
	unsigned char byte;
		
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	ad7793WrComReg(cs, CR_READ_ID);	//write the communication register first to inform 7793 that the next operation is a read.
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	
	ad7793Delay(20);
	
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	byte=SPIByte_In(cs.Spix);
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	ad7793Delay(10);
	return byte;
}

unsigned char ad7793ReadIOReg(AD7793_CS cs)
{
	unsigned char byte;
		
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	ad7793WrComReg(cs, CR_READ_IO);	//write the communication register first to inform 7793 that the next operation is a read.
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	
	ad7793Delay(20);
	
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	byte=SPIByte_In(cs.Spix);
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	ad7793Delay(10);
	
	return byte;
}

///////////////////////////////
//write subroutine
///////////////////////////////

//write the mode register
void ad7793WrModeReg(AD7793_CS cs,unsigned int mode)
{
	unsigned char byte;
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	ad7793WrComReg(cs, CR_WR_MODE);	//write the communication register first to inform 7793 that the next operation is a read.
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	
	ad7793Delay(20);
	
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	byte=(unsigned char)(mode>>8);
	ad7793WriteByte(cs, byte);
	byte=(unsigned char)mode;
	ad7793WriteByte(cs, byte);
	
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	ad7793Delay(10);
}

//write the config register
void ad7793WrConfigReg(AD7793_CS cs,unsigned int config)
{
	unsigned char byte;
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	ad7793WrComReg(cs, CR_WR_CONFIG);	//write the communication register first to inform 7793 that the next operation is a read.
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	
	ad7793Delay(20);
	
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	
	byte=(unsigned char)(config>>8);
	ad7793WriteByte(cs, byte);
	byte=(unsigned char)config;
	ad7793WriteByte(cs, byte);
	ad7793Delay(10);
	
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	ad7793Delay(10);
}

//write the io register
void ad7793WrIOReg(AD7793_CS cs,unsigned char io)
{
	
	SpiCS_Set(cs.Spix, cs.SpiCsx);
	ad7793WrComReg(cs, CR_WR_IO);	//write the communication register first to inform 7793 that the next operation is a read.
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	
	ad7793Delay(20);
	
	SpiCS_Set(cs.Spix, cs.SpiCsx);

	ad7793WriteByte(cs, io);
	
	SpiCS_Clear(cs.Spix, cs.SpiCsx);
	ad7793Delay(10);
}

//select the reference input
//ref=1,internal ref selected 
//ref=0,external ref selected
void ad7793ConfigSelRef(AD7793_CS cs,char ref)
{
	unsigned int configReg;
//	while(1)
//	{
//		configReg=ad7793ReadConfigReg(cs);
//	}
	configReg=ad7793ReadConfigReg(cs);
	
	if(ref)
		configReg|=CON_REFSEL;	//internal ref
	else
		configReg&=~CON_REFSEL;	//external ref
	
	ad7793WrConfigReg(cs,configReg);
}

//select the input buf
//buf=1,buffered
//buf=0,unbuffered
void ad7793ConfigSelBuf(AD7793_CS cs,char buf)
{
	unsigned int configReg;
	configReg=ad7793ReadConfigReg(cs);
	
	if(buf)
		configReg|=CON_BUF;	//buffered
	else
		configReg&=~CON_BUF;	//unbuffered
	
	ad7793WrConfigReg(cs,configReg);
}

//gain selection
void ad7793ConfigSelGain(AD7793_CS cs,unsigned short gain)
{
	unsigned int configReg;
	configReg=ad7793ReadConfigReg(cs);
	configReg &=~(CON_G0|CON_G1|CON_G2);
	configReg |=gain;
	ad7793WrConfigReg(cs,configReg);
	
}

void ad7793ConfigPolar(AD7793_CS cs, char en)
{
	unsigned int configReg;
	configReg=ad7793ReadConfigReg(cs);

	if(en)
		configReg|=CON_POLAR;	//单极性
	else
		configReg&=~CON_POLAR;	//双极性，默认

	ad7793WrConfigReg(cs,configReg);
}

//active channel selection
void ad7793ConfigSelChannel(AD7793_CS cs,char chanel)
{
	unsigned int configReg;
	configReg=ad7793ReadConfigReg(cs);
	configReg &=~(CON_CH0|CON_CH1|CON_CH2);
	
	configReg |=chanel;

	ad7793WrConfigReg(cs,configReg);
}


//set the sample rate
//rate=1;470hz
//rate=2;242hz
void ad7793ModeSelSampRate(AD7793_CS cs,unsigned int rate)
{
	unsigned int modeReg;
	modeReg=ad7793ReadModeReg(cs);

	modeReg &=~(MR_FS0|MR_FS1|MR_FS2|MR_FS3);
	modeReg |=rate;
	
	ad7793WrModeReg(cs,modeReg);
}

//set the working mode

void ad7793ModeSelMod(AD7793_CS cs,unsigned int mod)
{
	unsigned int modeReg;
	modeReg=ad7793ReadModeReg(cs);

	modeReg &=~(MR_MD0|MR_MD1|MR_MD2);
	modeReg |=mod;
	
	ad7793WrModeReg(cs,modeReg);
}

void ad7793ModeSelClk(AD7793_CS cs,unsigned int clk)
{
	unsigned int modeReg;
	modeReg=ad7793ReadModeReg(cs);

	modeReg &=~(MR_CLK0|MR_CLK1);
	modeReg |=clk;
	
	ad7793WrModeReg(cs,modeReg);
}


//initialize the ad7793
//cs is the chip selection
//we have four chip of ad7793
void ad7793Init(void)
{
	OS_ERR err;
	//if need reset the chip add at here	
	//ad7793Reset();

//	//if((ad7793ReadIDReg(CS_1,CS_1)&0x0B)!=0x0B)	//if not the ad7793 then return
//	//	return;
	AD7793_CS2.Spix = BF609_SPI1;
	AD7793_CS2.SpiCsx = BF609SPI_CS3;
	AD7793_CS2.SpiCsENx = BF609SPI_CSEN3;
	AD7793_CS3.Spix = BF609_SPI0;
	AD7793_CS3.SpiCsx = BF609SPI_CS1;
	AD7793_CS3.SpiCsENx = BF609SPI_CSEN1;
	SPICallBack_Init(AD7793_CS2.Spix, AD7793_CS2.SpiCsENx);	//初始化SPI接口
	SPICallBack_Init(AD7793_CS3.Spix, AD7793_CS3.SpiCsENx);
	OSTimeDly(300u,OS_OPT_TIME_DLY,&err);
	ad7793Reset(AD7793_CS2);
	ad7793Reset(AD7793_CS3);
	OSTimeDly(100u,OS_OPT_TIME_DLY,&err);

	ad7793ConfigSelRef(AD7793_CS2,CON_REF_EX);	//external ref
	ad7793ConfigSelRef(AD7793_CS3,CON_REF_EX);	//external ref
	ad7793ConfigSelBuf(AD7793_CS2, 0);		//not buffered
	ad7793ConfigSelBuf(AD7793_CS3, 0);		//not buffered
	ad7793ConfigSelGain(AD7793_CS2,CON_G_4);	//the gain is 1//此处无效
	ad7793ConfigSelGain(AD7793_CS3,CON_G_4);	//the gain is 1//此处无效
	ad7793ConfigSelChannel(AD7793_CS2,CON_CH_IN1);	//set the active input channel
	ad7793ConfigSelChannel(AD7793_CS3,CON_CH_IN1);	//set the active input channel
	ad7793ModeSelClk(AD7793_CS2,MR_CLK_IN64KOUT);
	ad7793ModeSelClk(AD7793_CS3,MR_CLK_IN64KOUT);
	ad7793ModeSelSampRate(AD7793_CS2, MR_F_62HZ);
	ad7793ModeSelSampRate(AD7793_CS3, MR_F_62HZ);
	ad7793ModeSelMod(AD7793_CS2, MR_M_INZSCAL);		//内部零位初始化
	ad7793ModeSelMod(AD7793_CS3, MR_M_INZSCAL);		//内部零位初始化
	OSTimeDly(300u,OS_OPT_TIME_DLY,&err);

	ad7793ModeSelMod(AD7793_CS2,MR_M_SCONVERT);	//单次转换	第一次采样的值无效
	ad7793ModeSelMod(AD7793_CS3,MR_M_SCONVERT);	//单次转换	第一次采样的值无效

	OSTimeDly(30u,OS_OPT_TIME_DLY,&err);	//等待内部zero-scale calibration to be completed.

}
//将片选取消SPI的复用，用于连续采样模式，同时拉低
void ad7793ContinueCSPin_Set(AD7793_CS Csx)
{

	if(Csx.Spix == BF609_SPI0)
	{
	    *pREG_PORTD_MUX &= ~SPI0_SEL1_PORTD_MUX;//Disable

	    /* PORTx_FER registers */
	    *pREG_PORTD_FER &= ~SPI0_SEL1_PORTD_FER;		//Disable

		/* set GPIO output */
		adi_gpio_SetDirection(
				ADI_GPIO_PORT_D,
				ADI_GPIO_PIN_11,
				ADI_GPIO_DIRECTION_OUTPUT);
		/* set it down */
		adi_gpio_Clear(ADI_GPIO_PORT_D,
				ADI_GPIO_PIN_11);

	}
	else if(Csx.Spix == BF609_SPI1)
	{
	    *pREG_PORTD_MUX &= ~SPI1_SEL3_PORTD_MUX;

	    /* PORTx_FER registers */
	    *pREG_PORTD_FER &= ~SPI1_SEL3_PORTD_FER;

		/* set GPIO output */
		adi_gpio_SetDirection(
				ADI_GPIO_PORT_D,
				ADI_GPIO_PIN_10,
				ADI_GPIO_DIRECTION_OUTPUT);
		/* set it down */
		adi_gpio_Clear(ADI_GPIO_PORT_D,
				ADI_GPIO_PIN_10);

	}

}

void ad7793ContinueInt_Init(AD7793_CS Csx)
{
	ADI_GPIO_RESULT result;
	static uint8_t gpioMemory[GPIO_MEMORY_SIZE];
	static uint32_t ad7793_gpioMaxCallbacks;
	static int i;

	if(i == 0)		//防止初始化多次
	{
		i++;
	    /* init the GPIO service */
//		result = adi_gpio_Init(
//				(void*)gpioMemory,
//				GPIO_MEMORY_SIZE,
//				&ad7793_gpioMaxCallbacks);

	}



	if(Csx.Spix == BF609_SPI0)
	{

		result = adi_gpio_PinInterruptAssignment(
				ADI_GPIO_PIN_INTERRUPT_2,
				ADI_GPIO_PIN_ASSIGN_BYTE_0,
				ADI_GPIO_PIN_ASSIGN_PDL_PINT2);


		/* set GPIO input */
		result = adi_gpio_SetDirection(
				ADI_GPIO_PORT_D,
				ADI_GPIO_PIN_2,
				ADI_GPIO_DIRECTION_INPUT);

		/* set edge sense mode (PORT G is connected to Pin Interrupt 5) */
		result = adi_gpio_SetPinIntEdgeSense(
				ADI_GPIO_PIN_INTERRUPT_2,
				ADI_GPIO_PIN_2,
				ADI_GPIO_SENSE_FALLING_EDGE);

		/* register pinint 0 callback */
		result = adi_gpio_RegisterCallback(
				ADI_GPIO_PIN_INTERRUPT_2,
				ADI_GPIO_PIN_2,
				pinDIntCallback,
				(void*)0);
//		adi_int_InstallHandler((uint32_t)INTR_PINT2_BLOCK, pinDIntCallback, NULL, true);
		/* set pin interrupt mask */
		result = adi_gpio_EnablePinInterruptMask(
				ADI_GPIO_PIN_INTERRUPT_2,
				ADI_GPIO_PIN_2,
				false);			//不开启

	}
	else if(Csx.Spix == BF609_SPI1)
	{



		/* set GPIO input */
		result = adi_gpio_SetDirection(
				ADI_GPIO_PORT_D,
				ADI_GPIO_PIN_14,
				ADI_GPIO_DIRECTION_INPUT);

		/* set edge sense mode (PORT D is connected to Pin Interrupt 2/3) */
		result = adi_gpio_SetPinIntEdgeSense(
				ADI_GPIO_PIN_INTERRUPT_2,
				ADI_GPIO_PIN_14,
				ADI_GPIO_SENSE_FALLING_EDGE);

		/* register pinint 0 callback */
		result = adi_gpio_RegisterCallback(
				ADI_GPIO_PIN_INTERRUPT_2,
				ADI_GPIO_PIN_14,
				pinDIntCallback,
				(void*)0);

		/* set pin interrupt mask */
		result = adi_gpio_EnablePinInterruptMask(
				ADI_GPIO_PIN_INTERRUPT_2,
				ADI_GPIO_PIN_14,
				true);
	}

}
//
//static void pinDIntCallback(unsigned int  iid,void* handlerArg)
// {
//	OS_ERR err;
//	//			/* toggle LED 1 */
//	adi_gpio_Toggle(ADI_GPIO_PORT_E, ADI_GPIO_PIN_14);
//	OSSemPost(&g_Sem_7793_Run, OS_OPT_POST_1, &err);
//
//}

/*
 * GPIO event Callback function
 */
static void pinDIntCallback(ADI_GPIO_PIN_INTERRUPT ePinInt, uint32_t PinIntData,  void *pCBParam)
{
	OS_ERR err;
	switch(ePinInt)
	{
	case ADI_GPIO_PIN_INTERRUPT_2:
		/* push button 1 */
		if ((PinIntData & ADI_GPIO_PIN_14)&&(PinIntData & ADI_GPIO_PIN_2))//两个中断都产生之后再进行信号量发送
		{
			adi_gpio_Toggle(ADI_GPIO_PORT_E, ADI_GPIO_PIN_14);
			OSSemPost(&g_Sem_7793_Run, OS_OPT_POST_1, &err);
//			/* toggle LED 1 */
//			adi_gpio_Toggle(ADI_GPIO_PORT_E, ADI_GPIO_PIN_14);
//			printf("pinIntG11");
		}
		if (PinIntData & ADI_GPIO_PIN_2)
		{
//			/* toggle LED 1 */
			adi_gpio_Toggle(ADI_GPIO_PORT_E, ADI_GPIO_PIN_14);
//			printf("pinIntG11");
			OSSemPost(&g_Sem_7793_Run, OS_OPT_POST_1, &err);
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

}
//此处将SPI的RX管脚的复用进行失能，同时输出低电平，用于连续模数时的TX上的下降沿中断
void ad7793ContinueRxInt_Enable(AD7793_CS Csx)
{
	if(Csx.Spix == BF609_SPI0)
	{
		/* PORTx_MUX registers */
		*pREG_PORTD_MUX &= ~SPI0_SEL1_PORTD_MUX;//Disable

		/* PORTx_FER registers */
		*pREG_PORTD_FER &= ~SPI0_MISO_PORTD_FER;		//Disable

	}
	else if(Csx.Spix == BF609_SPI1)
	{
		/* PORTx_MUX registers */
		 *pREG_PORTD_MUX &= ~SPI1_SEL3_PORTD_MUX;

	  /* PORTx_FER registers */
		 *pREG_PORTD_FER &= ~SPI1_MISO_PORTD_FER;

	}

}
//此处经SPI的RX管脚进行使能
void ad7793ContinueRxInt_Disable(AD7793_CS Csx)
{

	if(Csx.Spix == BF609_SPI0)
	{
		/* PORTx_MUX registers */
		*pREG_PORTD_MUX |= SPI0_MISO_PORTD_MUX;

		/* PORTx_FER registers */
		*pREG_PORTD_FER |= SPI0_MISO_PORTD_FER;

	}
	else if(Csx.Spix == BF609_SPI1)
	{
		/* PORTx_MUX registers */
		*pREG_PORTD_MUX |= SPI1_MISO_PORTD_MUX;

		/* PORTx_FER registers */
		*pREG_PORTD_FER |= SPI1_MISO_PORTD_FER;

	}

}
////以下给直流校准器
//void ad7793Continue_Init(void)
//{
//	OS_ERR err;
//
//	//if((ad7793ReadIDReg(CS_1,CS_1)&0x0B)!=0x0B)	//if not the ad7793 then return
//	//	return;
//
//	AD7793_CS2.Spix = BF609_SPI1;
//	AD7793_CS2.SpiCsx = BF609SPI_CS3;
//	AD7793_CS2.SpiCsENx = BF609SPI_CSEN3;
//	AD7793_CS3.Spix = BF609_SPI0;
//	AD7793_CS3.SpiCsx = BF609SPI_CS1;
//	AD7793_CS3.SpiCsENx = BF609SPI_CSEN1;
//	SPICallBack_Init(AD7793_CS2.Spix, AD7793_CS2.SpiCsENx);	//初始化SPI接口
//	SPICallBack_Init(AD7793_CS3.Spix, AD7793_CS3.SpiCsENx);
////	ad7793ContinueInt_Init(AD7793_CS2);								//初始化Rx的中断配置并开启
////	ad7793ContinueInt_Init(AD7793_CS3);								//初始化Rx的中断配置并开启
//	OSTimeDly(300u,OS_OPT_TIME_DLY,&err);
//	ad7793Reset(AD7793_CS2);
//	ad7793Reset(AD7793_CS3);
//	OSTimeDly(100u,OS_OPT_TIME_DLY,&err);
//
//	ad7793ConfigSelRef(AD7793_CS2,CON_REF_EX);	//external ref
//	ad7793ConfigSelRef(AD7793_CS3,CON_REF_EX);	//external ref
//	ad7793ConfigSelBuf(AD7793_CS2, 0);		//not buffered
//	ad7793ConfigSelBuf(AD7793_CS3, 0);		//not buffered
//	ad7793ConfigSelGain(AD7793_CS2,CON_G_128);	//the gain is 1
//	ad7793ConfigSelGain(AD7793_CS3,CON_G_128);	//the gain is 1
//	ad7793ConfigPolar(AD7793_CS2, 1);
//	ad7793ConfigPolar(AD7793_CS3, 1);
//	ad7793ConfigSelChannel(AD7793_CS2,CON_CH_IN1);	//set the active input channel
//	ad7793ConfigSelChannel(AD7793_CS3,CON_CH_IN2);	//set the active input channel
//	ad7793ModeSelClk(AD7793_CS2,MR_CLK_IN64KOUT);
//	ad7793ModeSelClk(AD7793_CS3,MR_CLK_IN64KOUT);
//	ad7793ModeSelSampRate(AD7793_CS2, MR_F_4D17HZ);
//	ad7793ModeSelSampRate(AD7793_CS3, MR_F_4D17HZ);
////	ad7793ModeSelMod(AD7793_CS2, MR_M_INZSCAL);		//内部零位初始化，该模式会造成连续采样无法启动
////	ad7793ModeSelMod(AD7793_CS3, MR_M_INZSCAL);		//内部零位初始化
//
////	OSTimeDly(300u,OS_OPT_TIME_DLY,&err);				//此处延时应该取消
//
//
//	ad7793ContinueCSPin_Set(AD7793_CS2);		//手动拉低CS,令其一直处于使能状态
//	ad7793ContinueCSPin_Set(AD7793_CS3);		//手动拉低CS,令其一直处于使能状态
//
////以下进行连续采样
//	ad7793WrComReg(AD7793_CS2, CR_READ_DATA);	//write the communication register first to inform 7793 that the next operation is a read.
//	ad7793WrComReg(AD7793_CS3, CR_READ_DATA);	//write the communication register first to inform 7793 that the next operation is a read.
//
////由于两块芯片采样率一致，此处只对后面一块芯片的中断进行响应，同时读取两块芯片的采样值
////	ad7793ContinueInt_Init(AD7793_CS2);				//初始化Rx的中断配置并开启
//	//此处将CS片选拉低放到ad7793ContinueCSPin_Set函数中，暂时没有调试（2015/9/23）
//
//	ad7793ContinueInt_Init(AD7793_CS3);				//初始化Rx的中断配置并开启
//
//
////	ad7793ContinueRxInt_Enable(AD7793_CS2);			//此处管脚被复用也会进入中断，BUG？
////	ad7793ContinueRxInt_Enable(AD7793_CS3);
//
////第一次必须读取一次，不然后面第一次读取会出现错误
//	ad7793WrComReg(AD7793_CS2, CR_READ_DATA);
//	ad7793Read24Bit(AD7793_CS2);
//
//	ad7793WrComReg(AD7793_CS3, CR_READ_DATA);
//	ad7793Read24Bit(AD7793_CS3);
//}

void ad7793Continue_Init(void)
{
	OS_ERR err;

	//if((ad7793ReadIDReg(CS_1,CS_1)&0x0B)!=0x0B)	//if not the ad7793 then return
	//	return;

	AD7793_CS2.Spix = BF609_SPI1;
	AD7793_CS2.SpiCsx = BF609SPI_CS3;
	AD7793_CS2.SpiCsENx = BF609SPI_CSEN3;
	AD7793_CS3.Spix = BF609_SPI0;
	AD7793_CS3.SpiCsx = BF609SPI_CS1;
	AD7793_CS3.SpiCsENx = BF609SPI_CSEN1;
	SPICallBack_Init(AD7793_CS2.Spix, AD7793_CS2.SpiCsENx);	//初始化SPI接口
	SPICallBack_Init(AD7793_CS3.Spix, AD7793_CS3.SpiCsENx);
//	ad7793ContinueInt_Init(AD7793_CS2);								//初始化Rx的中断配置并开启
//	ad7793ContinueInt_Init(AD7793_CS3);								//初始化Rx的中断配置并开启
	OSTimeDly(300u,OS_OPT_TIME_DLY,&err);
	ad7793Reset(AD7793_CS2);
	ad7793Reset(AD7793_CS3);
	OSTimeDly(100u,OS_OPT_TIME_DLY,&err);

	ad7793ConfigSelRef(AD7793_CS2,CON_REF_EX);	//external ref
	ad7793ConfigSelRef(AD7793_CS3,CON_REF_EX);	//external ref
	ad7793ConfigSelBuf(AD7793_CS2, 0);		//not buffered
	ad7793ConfigSelBuf(AD7793_CS3, 0);		//not buffered
	ad7793ConfigSelGain(AD7793_CS2,CON_G_1);	//the gain is 1
	ad7793ConfigSelGain(AD7793_CS3,CON_G_1);	//the gain is 1
	ad7793ConfigSelChannel(AD7793_CS2,CON_CH_IN1);	//set the active input channel
	ad7793ConfigSelChannel(AD7793_CS3,CON_CH_IN1);	//set the active input channel
	ad7793ModeSelClk(AD7793_CS2,MR_CLK_IN64KOUT);
	ad7793ModeSelClk(AD7793_CS3,MR_CLK_IN64KOUT);
	ad7793ModeSelSampRate(AD7793_CS2, MR_F_16D7HZ);
	ad7793ModeSelSampRate(AD7793_CS3, MR_F_16D7HZ);
//	ad7793ModeSelMod(AD7793_CS2, MR_M_INZSCAL);		//内部零位初始化，该模式会造成连续采样无法启动
//	ad7793ModeSelMod(AD7793_CS3, MR_M_INZSCAL);		//内部零位初始化

//	OSTimeDly(300u,OS_OPT_TIME_DLY,&err);				//此处延时应该取消


	ad7793ContinueCSPin_Set(AD7793_CS2);		//手动拉低CS,令其一直处于使能状态
	ad7793ContinueCSPin_Set(AD7793_CS3);		//手动拉低CS,令其一直处于使能状态

//以下进行连续采样
	ad7793WrComReg(AD7793_CS2, CR_READ_DATA);	//write the communication register first to inform 7793 that the next operation is a read.
	ad7793WrComReg(AD7793_CS3, CR_READ_DATA);	//write the communication register first to inform 7793 that the next operation is a read.

//由于两块芯片采样率一致，此处只对后面一块芯片的中断进行响应，同时读取两块芯片的采样值
//	ad7793ContinueInt_Init(AD7793_CS2);				//初始化Rx的中断配置并开启
	//此处将CS片选拉低放到ad7793ContinueCSPin_Set函数中，暂时没有调试（2015/9/23）

	ad7793ContinueInt_Init(AD7793_CS3);				//初始化Rx的中断配置并开启


//	ad7793ContinueRxInt_Enable(AD7793_CS2);			//此处管脚被复用也会进入中断，BUG？
//	ad7793ContinueRxInt_Enable(AD7793_CS3);

//第一次必须读取一次，不然后面第一次读取会出现错误
	ad7793WrComReg(AD7793_CS2, CR_READ_DATA);
	ad7793Read24Bit(AD7793_CS2);

	ad7793WrComReg(AD7793_CS3, CR_READ_DATA);
	ad7793Read24Bit(AD7793_CS3);
}

//AD7793的单次转换，返回的数据为24位的原始数据
unsigned long ad7793_Convert_Once(AD7793_CS Csx, unsigned int channal, unsigned int Gain)
{
	unsigned long AD7793_Data;
	unsigned char sta;
	OS_ERR err;

	ad7793ConfigSelChannel(Csx,channal);
	ad7793ConfigSelGain(Csx,Gain);
	ad7793ModeSelMod(Csx,MR_M_SCONVERT);	//单次转换
	do
	{
		OSTimeDly(1u,OS_OPT_TIME_DLY,&err);
		sta=ad7793ReadStaReg(Csx);
		sta&=SR_RDY;
	}while(sta != SR_RDY);
	AD7793_Data = ad7793ReadData(Csx);

	return AD7793_Data;

}


void ad7793_Convert_Continue(AD7793_CS Csx, unsigned long *AD7793_Data)
{

	/* clear pin interrupt mask */
	adi_gpio_EnablePinInterruptMask(
			ADI_GPIO_PIN_INTERRUPT_2,
			ADI_GPIO_PIN_2,
			false);

	ad7793WrComReg(Csx, CR_READ_DATA);
	*AD7793_Data = ad7793Read24Bit(Csx);

	/* set pin interrupt mask */
	adi_gpio_EnablePinInterruptMask(
			ADI_GPIO_PIN_INTERRUPT_2,
			ADI_GPIO_PIN_2,
			true);
}

void ad7793_Convert_Continue_Both(AD7793_CS Csx1, AD7793_CS Csx2,
		unsigned long *AD7793_Data1,unsigned long *AD7793_Data2)
{
#if OS_CRITICAL_METHOD == 3                                //Allocate storage for CPU status register
    OS_CPU_SR  cpu_sr;
#endif
	/* clear pin interrupt mask */
	adi_gpio_EnablePinInterruptMask(
			ADI_GPIO_PIN_INTERRUPT_2,
			ADI_GPIO_PIN_2,
			false);
////加入临界区
//	OS_ENTER_CRITICAL();

	ad7793WrComReg(Csx1, CR_READ_DATA);
	*AD7793_Data1 = ad7793Read24Bit(Csx1);

	ad7793WrComReg(Csx2, CR_READ_DATA);
	*AD7793_Data2 = ad7793Read24Bit(Csx2);

//	OS_EXIT_CRITICAL();
	/* set pin interrupt mask */
	adi_gpio_EnablePinInterruptMask(
			ADI_GPIO_PIN_INTERRUPT_2,
			ADI_GPIO_PIN_2,
			true);
}

void ad7793InterruptState_Set(ad7793Int_State state)
{
	if(state == ad7793Int_Enable)
	{
		/* set pin interrupt mask */
		adi_gpio_EnablePinInterruptMask(
				ADI_GPIO_PIN_INTERRUPT_2,
				ADI_GPIO_PIN_2,
				true);
	}else
	{
		/* clear pin interrupt mask */
		adi_gpio_EnablePinInterruptMask(
				ADI_GPIO_PIN_INTERRUPT_2,
				ADI_GPIO_PIN_2,
				false);
	}

}


///*
// * 函数:AD7793连续采样任务
// * 描述：通过接收信号量g_Sem_Uart_ad7793,启动ad7793
// * 接口参数：无
// * 返回：无
// * 备注：
// */
//extern ADI_UART_HANDLE ghUart;
//extern OS_SEM g_Sem_7793_Run;
//static unsigned char sta;
//void Sem_Control_ad7793(void* arg)
//{
////	ADI_GPIO_RESULT result;
////uint32_t GPIO_Data;
//	OS_ERR err;
//	ADI_GPIO_RESULT result;
//	uint32_t GPIO_Data;
//	unsigned short i = 0;
//	unsigned long ad7793_Data_Read = 0;
//	unsigned int ad7793_Data_Read1 = 0;
//	float ADC_Value = 0;
//	uint8_t Uart_Buff_Send1[3],text;
//	char Str_AD[100];
//	unsigned char Len_Str_AD = 0;
//	int j = 0;
//	ad7793Init();
////	ad7793WrComReg(AD7793_CS2, CR_READ_STATUS);
//	printf("ad7793 has be ready.\n");
//	OSTimeDly(20u,OS_OPT_TIME_DLY,&err);
//	while(1)
//	{
////		OSSemPend(&g_Sem_Uart_ad7793,0u, OS_OPT_PEND_BLOCKING, NULL, &err);
////		adi_gpio_Toggle(ADI_GPIO_PORT_G, ADI_GPIO_PIN_12);
////		result = adi_gpio_GetData(SPI0_DIN_PORT, &GPIO_Data);
////		i = GPIO_Data & SPI0_DIN_PIN;
////		if( i == 0 )
////		{
//////			ad7793WrComReg(AD7793_CS2, CR_READ_DATA);	//开启连续读取模数01011100
//////			printf("ad7793 come in 1.\n");
////			ad7793_Data_Read = ad7793ReadData(AD7793_CS2, CON_CH_IN1);
////			for(j = 0; j < 3; j++)
////			{
////				Uart_Buff_Send[j] = ad7793_Data_Read;
////				ad7793_Data_Read = ad7793_Data_Read >> 8;
////			}
////			adi_uart_Write(ghUart, "data", sizeof("data"));
////			for(j = 2; j >= 0; j--)
////			{
////				adi_uart_Write(ghUart, &Uart_Buff_Send[j], 1);
////			}
////			OSTimeDly(200u,OS_OPT_TIME_DLY,&err);
////		}
////		else
////		{
////			OSTimeDly(2000u,OS_OPT_TIME_DLY,&err);
////			ad7793_Data_Read1 = ad7793ReadConfigReg(AD7793_CS2, CR_READ_MODE);
////
////			for(j = 0; j < 2; j++)
////			{
////				Uart_Buff_Send[j] = ad7793_Data_Read1;
////				ad7793_Data_Read = ad7793_Data_Read >> 8;
////			}
////			adi_uart_Write(ghUart, "87", sizeof("87"));
////			for(j = 1; j >= 0; j--)
////			{
////				adi_uart_Write(ghUart, &Uart_Buff_Send[j], 1);
////			}
////			OSTimeDly(200u,OS_OPT_TIME_DLY,&err);
////		}
////		text = ad7793ReadStaReg(AD7793_CS2, CON_CH_IN1);
////		adi_uart_Write(ghUart, &text, 1);
////		OSTimeDly(1000u,OS_OPT_TIME_DLY,&err);
//
//		OSSemPend(&g_Sem_7793_Run,0u, OS_OPT_PEND_BLOCKING, NULL, &err);
//		ad7793ConfigSelChannel(AD7793_CS2,CON_CH_IN1);	//set the active input channel
//		ad7793ModeSelMod(AD7793_CS2,MR_M_SCONVERT);	//单次转换
//		do
//		{
//			OSTimeDly(1u,OS_OPT_TIME_DLY,&err);
//			sta=ad7793ReadStaReg(AD7793_CS2,AD7793_CS2);
//			sta&=SR_RDY;
//		}while(sta != SR_RDY);
//		ad7793_Data_Read = ad7793ReadData(AD7793_CS2,AD7793_CS2);
//		ADC_Value = (ad7793_Data_Read/pow(2 , 23)-1)*2480*6/4;//计算模拟值
//		Len_Str_AD = sprintf(Str_AD, "AD channal1:%.2fmv\n", ADC_Value);
//		adi_uart_Write(ghUart, Str_AD,  Len_Str_AD);
//
//
//		OSTimeDly(32u,OS_OPT_TIME_DLY,&err);		//此处需要依照采样率进行延时，延时市场为1ms+2采样周期
//		ad7793ConfigSelChannel(AD7793_CS2,CON_CH_IN2);	//set the active input channel
//		ad7793ModeSelMod(AD7793_CS2,MR_M_SCONVERT);	//单次转换
//		do
//		{
//			OSTimeDly(1u,OS_OPT_TIME_DLY,&err);
//			sta=ad7793ReadStaReg(AD7793_CS2,AD7793_CS2);
//			sta&=SR_RDY;
//		}while(sta != SR_RDY);
//		ad7793_Data_Read = ad7793ReadData(AD7793_CS2,AD7793_CS2);
//		ADC_Value = (ad7793_Data_Read/pow(2 , 23)-1)*2480*6/4;//计算模拟值
//		Len_Str_AD = sprintf(Str_AD, "AD channal2:%.2fmv\n", ADC_Value);
//		adi_uart_Write(ghUart, Str_AD,  Len_Str_AD);
//
//
////以下为采样数值的16进制串口发送
////		ad7793_Data_Read = ad7793ReadData(AD7793_CS2,AD7793_CS2);
////		for(j = 0; j < 3; j++)
////		{
////			Uart_Buff_Send1[j] = ad7793_Data_Read;
////			ad7793_Data_Read = ad7793_Data_Read >> 8;
////		}
//////		adi_uart_Write(ghUart, "data", sizeof("data"));
////		adi_uart_Write(ghUart, "1",  sizeof("1")-1);
////		for(j = 2; j >= 0; j--)
////		{
////			adi_uart_Write(ghUart, &Uart_Buff_Send1[j], 1);
////		}
//
////以下为模数和配置寄存器的读数据
//////		ad7793Init();
////
////
//////		OSTimeDly(300u,OS_OPT_TIME_DLY,&err);
////				ad7793_Data_Read1 = ad7793ReadConfigReg(AD7793_CS2, CR_READ_CONFIG);
////				ad7793_Data_Read = ad7793_Data_Read1;
////				ssync();
////				for(j = 0; j < 2; j++)
////				{
////					Uart_Buff_Send1[j] = (uint8_t)ad7793_Data_Read;
////					ad7793_Data_Read = ad7793_Data_Read1 >> 8;
////				}
////				adi_uart_Write(ghUart, "000", sizeof("000")-1);
//////				for(j = 1; j >= 0; j--)
//////				{
//////					adi_uart_Write(ghUart, &Uart_Buff_Send[j], 1);
//////				}
////				for(j = 1; j >= 0; j--)
////				{
////
////					adi_uart_Write(ghUart, &Uart_Buff_Send1[j], 1);
////				}
////
////
////				adi_uart_Write(ghUart, "8", sizeof("8")-1);
////
////				ad7793_Data_Read1 = ad7793ReadModeReg(AD7793_CS2, AD7793_CS2);
////				ad7793_Data_Read = ad7793_Data_Read1;
////				ssync();
////				for(j = 0; j < 2; j++)
////				{
////					Uart_Buff_Send1[j] = (uint8_t)ad7793_Data_Read;
////					ad7793_Data_Read = ad7793_Data_Read1 >> 8;
////				}
////				adi_uart_Write(ghUart, "000", sizeof("000")-1);
//////				for(j = 1; j >= 0; j--)
//////				{
//////					adi_uart_Write(ghUart, &Uart_Buff_Send[j], 1);
//////				}
////				for(j = 1; j >= 0; j--)
////				{
////
////					adi_uart_Write(ghUart, &Uart_Buff_Send1[j], 1);
////				}
////
////
////				adi_uart_Write(ghUart, "8", sizeof("8")-1);
//
//
////		ad7793Init();
//
//
//
//	}
//
//}











