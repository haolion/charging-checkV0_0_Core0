#ifndef AD7793_SPI_H
#define AD7793_SPI_H
#include <drivers/spi/adi_spi.h>
#include "../AD7793_SPI/defines.h"
#include "../AD7793_SPI/spi.h"
#include "os.h"


//communication register's function bit define
#define CR0	BIT0
#define CR1	BIT1
#define CR2	BIT2
#define CR3	BIT3
#define CR4	BIT4
#define CR5	BIT5
#define CR6	BIT6
#define CR7	BIT7

#define CR_CREAD 	CR2
#define CR_RS0		CR3
#define CR_RS1		CR4
#define CR_RS2		CR5
#define CR_R_W		CR6
#define CR_WEN		CR7

#define CR_READ_STATUS		(0x00|CR_R_W)
#define CR_READ_MODE		(0x00|CR_R_W|CR_RS0)
#define CR_READ_CONFIG		(0x00|CR_R_W|CR_RS1)
#define CR_READ_DATA		(0x00|CR_R_W|CR_RS1|CR_RS0)	//read the data register
#define CR_READ_ID			(0x00|CR_R_W|CR_RS2)
#define CR_READ_IO			(0x00|CR_R_W|CR_RS2|CR_RS0)
#define CR_READ_OFFSET		(0x00|CR_R_W|CR_RS2|CR_RS1)
#define CR_READ_FULLSCALE	(0x00|CR_R_W|CR_RS2|CR_RS1|CR_RS0)
#define CR_READ_DATA_C		(0x00&CR_WEN|CR_R_W|CR_RS1|CR_RS0|CR_CREAD)
#define CR_READ_CONTINUE	(0x00|CR_R_W|CR_RS1|CR_RS0|CR_CREAD)    //continue read the data register

#define CR_WR_MODE			(0x00|CR_RS0)
#define CR_WR_CONFIG		(0x00|CR_RS1)
#define CR_WR_IO			(0x00|CR_RS2|CR_RS0)
#define CR_WR_OFFSET		(0x00|CR_RS2|CR_RS1)
#define CR_WR_FULLSCALE	(0x00|CR_RS2|CR_RS1|CR_RS0)

//status register
#define SR0	BIT0
#define SR1	BIT1
#define SR2	BIT2
#define SR3	BIT3
#define SR4	BIT4
#define SR5	BIT5
#define SR6	BIT6
#define SR7	BIT7

#define SR_CH0	SR0
#define SR_CH1	SR1
#define SR_CH2	SR2
#define SR_ERR	SR6
#define SR_RDY	SR7

//mode register
#define MR_FS0		BIT0
#define MR_FS1		BIT1
#define MR_FS2		BIT2
#define MR_FS3		BIT3
#define MR_CLK0		BIT6
#define MR_CLK1		BIT7
#define MR_MD0		BIT13
#define MR_MD1		BIT14
#define MR_MD2		BIT15

#define MR_F_470HZ	1
#define MR_F_242HZ	2
#define MR_F_123HZ	3
#define MR_F_62HZ	4
#define MR_F_50HZ	5
#define MR_F_39HZ	6
#define MR_F_33D2HZ	7
#define MR_F_19D6HZ	8
#define MR_F_16D7HZ	9
#define MR_F_16D7HZ_1	10
#define MR_F_12D5HZ		11
#define MR_F_10HZ		12
#define MR_F_8D33HZ		13
#define MR_F_6D25HZ		14
#define MR_F_4D17HZ		15

#define MR_M_CCONVERT	0							//连续转换
#define MR_M_SCONVERT	(0x0000|MR_MD0)				//单次转换
#define MR_M_IDLE		(0x0000|MR_MD1)				//
#define MR_M_PDOWN		(0x0000|MR_MD1|MR_MD0)
#define MR_M_INZSCAL	(0x0000|MR_MD2)				//内部零位校准
#define MR_M_INFSCAL	(0x0000|MR_MD2|MR_MD0)		//内部满量程校准
#define MR_M_SYSZSCAL	(0x0000|MR_MD2|MR_MD1)		//系统零位校准
#define MR_M_SYSFSCAL	(0x0000|MR_MD2|MR_MD1|MR_MD0)

#define MR_CLK_IN64K		(0x0000)
#define MR_CLK_IN64KOUT		(0x0000|MR_CLK0)
#define MR_CLK_EX64K		(0x0000|MR_CLK1)
#define MR_CLK_EX128K		(0x0000|MR_CLK1|MR_CLK0)

//configuration register
#define CON_CH0		BIT0
#define CON_CH1		BIT1
#define CON_CH2		BIT2
#define CON_BUF		BIT4
#define CON_REFSEL	BIT7
#define CON_G0		BIT8
#define CON_G1		BIT9
#define CON_G2		BIT10
#define CON_BOOST	BIT11
#define CON_POLAR	BIT12
#define CON_BURNOUT	BIT13
#define CON_VBIAS0	BIT14
#define CON_VBIAS1	BIT15

#define CON_REF_IN 	CON_REFSEL
#define CON_REF_EX 	0x0000

#define CON_CH_IN1	(0x0000)
#define CON_CH_IN2	(0x0000|CON_CH0)
#define CON_CH_IN3	(0x0000|CON_CH1)
#define CON_CH_TMP	(0x0000|CON_CH2|CON_CH1)

#define CON_G_1		(0x0000)
#define CON_G_2		(0x0000|CON_G0)
#define CON_G_4		(0x0000|CON_G1)
#define CON_G_8		(0x0000|CON_G1|CON_G0)
#define CON_G_16	(0x0000|CON_G2)
#define CON_G_32	(0x0000|CON_G2|CON_G0)
#define CON_G_64	(0x0000|CON_G2|CON_G1)
#define CON_G_128	(0x0000|CON_G2|CON_G1|CON_G0)


 typedef struct AD7793_CS		//	以下未标明TX的皆为RX数据
 {
	BF609_SPIx Spix;
	unsigned int SpiCsx;		//SPI的片选脚x
	unsigned int SpiCsENx;		//SPI的片选脚x使能位
 }AD7793_CS;
extern AD7793_CS AD7793_CS2,AD7793_CS3;

extern OS_SEM g_Sem_7793_Run;

typedef enum ad7793Int_State
{
	ad7793Int_Enable,
	ad7793Int_Disable
}ad7793Int_State;

void ad7793Reset(AD7793_CS cs);
void ad7793Delay(char sclk);
void ad7793WriteByte(AD7793_CS cs, unsigned char byte);
void ad7793WrComReg(AD7793_CS cs,unsigned char reg);
unsigned long ad7793ReadData(AD7793_CS cs);
unsigned long ad7793Read24Bit(AD7793_CS cs);
unsigned char ad7793ReadStaReg(AD7793_CS cs);
unsigned int ad7793ReadModeReg(AD7793_CS cs);
unsigned int ad7793ReadConfigReg(AD7793_CS cs);
unsigned char ad7793ReadIDReg(AD7793_CS cs);
unsigned char ad7793ReadIOReg(AD7793_CS cs);
void ad7793WrModeReg(AD7793_CS cs,unsigned int mode);
void ad7793WrConfigReg(AD7793_CS cs,unsigned int config);
void ad7793WrIOReg(AD7793_CS cs,unsigned char io);
void ad7793ConfigSelRef(AD7793_CS cs,char ref);
void ad7793ConfigSelBuf(AD7793_CS cs,char buf);
//void ad7793ConfigSelGain(char cs,char gain);
void ad7793ConfigSelGain(AD7793_CS cs,unsigned short gain);
void ad7793ConfigPolar(AD7793_CS cs, char en);
void ad7793ConfigSelChannel(AD7793_CS cs,char chanel);
void ad7793ModeSelSampRate(AD7793_CS cs,unsigned int rate);
void ad7793ModeSelMod(AD7793_CS cs,unsigned int mod);
void ad7793ModeSelClk(AD7793_CS cs,unsigned int clk);
void ad7793Init(void);
void ad7793ContinueCSPin_Set(AD7793_CS Csx);
void ad7793ContinueInt_Init(AD7793_CS Csx);
void ad7793ContinueRxInt_Enable(AD7793_CS Csx);
void ad7793ContinueRxInt_Disable(AD7793_CS Csx);
void ad7793Continue_Init(void);//此处为了减小两路的同步误差，对两路都进行设置，实际中只需要用到一路时需要进入修改
unsigned long ad7793_Convert_Once(AD7793_CS Csx, unsigned int channal, unsigned int Gain);
void ad7793_Convert_Continue(AD7793_CS Csx, unsigned long *AD7793_Data);
void ad7793_Convert_Continue_Both(AD7793_CS Csx1, AD7793_CS Csx2,
		unsigned long *AD7793_Data1,unsigned long *AD7793_Data2);
void ad7793InterruptState_Set(ad7793Int_State state);
//void Sem_Control_ad7793(void* arg);
//void pinIntCallback(ADI_GPIO_PIN_INTERRUPT ePinInt, uint32_t PinIntData,  void *pCBParam);
//void ad7793_RDY_Int_Init(void);

#endif
