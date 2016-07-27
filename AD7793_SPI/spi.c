
/*
 * 功能：IO模拟SPI接口
 * 版本：V1.0
 * 描述：此处用于XL803M-609信号板，用于控制AD7793
 * 公司：深圳市星龙科技股份有限公司
 * 作者：张颖豪
 * 日期：2015/5/27
 * 备注：修改接口时请直接在头文件中的接口宏定义进行修改。
 */

#include "spi.h"
#include <services/gpio/adi_gpio.h>



static unsigned char BUFF_SPI0TX[NUM_SPITXBUFF];
static unsigned char BUFF_SPI0RX[NUM_SPIRXBUFF];
static unsigned char BUFF_SPI1TX[NUM_SPITXBUFF];
static unsigned char BUFF_SPI1RX[NUM_SPIRXBUFF];
static char Flag_Tx0, Flag_Tx1;
static ADI_SPI_HANDLE phSPI1;
static ADI_SPI_HANDLE phSPI0;

static ADI_SPI_TRANSCEIVER SPI0Struct_Tx = {NULL,
									0,
									&BUFF_SPI0TX[0],
									NUM_SPITXBUFF,
									NULL,
									0
									};

static ADI_SPI_TRANSCEIVER SPI0Struct_Rx = {NULL,
									0,
									NULL,
									0,
									&BUFF_SPI0RX[0],
									NUM_SPIRXBUFF
									};
static ADI_SPI_TRANSCEIVER SPI1Struct_Tx = {NULL,
									0,
									&BUFF_SPI1TX[0],
									NUM_SPITXBUFF,
									NULL,
									0
									};

static ADI_SPI_TRANSCEIVER SPI1Struct_Rx = {NULL,
									0,
									NULL,
									0,
									&BUFF_SPI1RX[0],
									NUM_SPIRXBUFF
									};

uint8_t Spi0Memory[ADI_SPI_INT_MEMORY_SIZE];
uint8_t Spi1Memory[ADI_SPI_INT_MEMORY_SIZE];


void spi_delay_n10ns(unsigned int n_10ns)
{
	unsigned int i;
	for(i = 0;i < n_10ns; i++)
	{
		asm("nop;");
		asm("nop;");
		asm("nop;");
		asm("nop;");
		asm("nop;");
	}
}


#ifdef RUNINRAM
section("ad_cal")
#endif
//根据用户需要的资源进行复用，此处用户需要进行修改，暂时不具备选用功能
void SPIPIN_Init(void)
{

/* PORTx_MUX registers */
    *pREG_PORTD_MUX |= SPI0_CLK_PORTD_MUX | SPI0_MISO_PORTD_MUX
     | SPI0_MOSI_PORTD_MUX | SPI0_SEL1_PORTD_MUX | SPI1_CLK_PORTD_MUX
     | SPI1_MISO_PORTD_MUX | SPI1_MOSI_PORTD_MUX | SPI1_SEL3_PORTD_MUX;

    /* PORTx_FER registers */
    *pREG_PORTD_FER |= SPI0_CLK_PORTD_FER | SPI0_MISO_PORTD_FER
     | SPI0_MOSI_PORTD_FER | SPI0_SEL1_PORTD_FER | SPI1_CLK_PORTD_FER
     | SPI1_MISO_PORTD_FER | SPI1_MOSI_PORTD_FER | SPI1_SEL3_PORTD_FER;
}


//对SPI0和SPI1进行复用
void SPICallBack_Init(BF609_SPIx SPIx, unsigned int CSENx)
{
	unsigned int Result = 0u;
	unsigned int Channal = 0u;
	uint8_t *p_SpiMemory;
	ADI_SPI_HANDLE *phSPI;
	SPIPIN_Init();
	Flag_Tx0 = 0;
	Flag_Tx1 = 0;
	switch(SPIx)
	{
	case	BF609_SPI0:
		Channal = 0u;
		p_SpiMemory = Spi0Memory;
		phSPI = &phSPI0;
		break;
	case	BF609_SPI1:
		Channal = 1u;
		p_SpiMemory = Spi1Memory;
		phSPI = &phSPI1;
		break;
	default:
		//可加入异常处理代码
		break;
	}
    if(Result == 0u)
    {
    	Result = adi_spi_Open(SPIx, p_SpiMemory, (uint32_t)ADI_SPI_INT_MEMORY_SIZE, phSPI);
    }
    if (Result == 0u)
	{
       /* device in master of the SPI interface */
	   Result = (uint32_t)adi_spi_SetMaster(*phSPI, true);
	}
    if (Result == 0u)
	{
       /* SPI slave select in controlled by software not hardware */
	   Result = (uint32_t)adi_spi_SetHwSlaveSelect(*phSPI, false);
	}
    if (Result == 0u)
	{
    	/* send zeros if tx SPI underflows*/
		Result = (uint32_t)adi_spi_SetTransmitUnderflow(*phSPI, true);
	}
    if (Result == 0u)
	{
    	/* data transitions on falling edge of clock */
    	Result = (uint32_t)adi_spi_SetClockPhase(*phSPI, true);
	}
    if (Result == 0u)
	{
    	/* SPI clock Polarity */
    	//Result = (uint32_t)adi_spi_SetClockPolarity(phSPI1, false);
    	if(SPIx == BF609_SPI0)
    	{
    	   	*pREG_SPI0_CTL |= BITM_SPI_CTL_CPOL;

    	}else
    	{
    		*pREG_SPI1_CTL |= BITM_SPI_CTL_CPOL;

    	}
	}
    if (Result == 0u)
	{
    	/* SPI clock is SCLK divided by 25 + 1 */
    	Result = (uint32_t)adi_spi_SetClock(*phSPI, 25u);
	}
    if (Result == 0u)
	{
    	/* SPI slave select is on SPI slave select 1 pin */
    	Result = (uint32_t)adi_spi_SetSlaveSelect(*phSPI, CSENx);
	}
    if (Result == 0u)
	{
    	/* SPI data transfers are 8 bit */
    	Result = (uint32_t)adi_spi_SetWordSize(*phSPI, ADI_SPI_TRANSFER_8BIT);
	}
    if (Result == 0u)
	{
    	/* generate tx data interrupt when watermark level breaches 50% level */
    	/* DMA watermark levels are disabled because SPI is in interrupt mode */
    	Result = (uint32_t)adi_spi_SetTxWatermark(*phSPI,
                                                  ADI_SPI_WATERMARK_50,
                                                  ADI_SPI_WATERMARK_DISABLE,
                                                  ADI_SPI_WATERMARK_DISABLE);
	}
    if (Result == 0u)
	{
    	/* generate rx data interrupt when watermark level breaches 50% level */
    	/* DMA watermark levels are disabled because SPI is in interrupt mode */
    	Result = (uint32_t)adi_spi_SetRxWatermark(*phSPI,
                                                  ADI_SPI_WATERMARK_50,
                                                  ADI_SPI_WATERMARK_DISABLE,
                                                  ADI_SPI_WATERMARK_DISABLE);
	}
    //callback mode
    if (Result == 0u)
    {
       	/* enable callbacks */
    	if(SPIx == BF609_SPI0)
    	{
    		Result = (uint32_t)adi_spi_RegisterCallback(*phSPI, SPI0_Callback, NULL);

    	}else
    	{
    		Result = (uint32_t)adi_spi_RegisterCallback(*phSPI, SPI1_Callback, NULL);

    	}
   	}
    if (Result == 0u)
   	{
       	/* interrupt mode, i.e no dma */
   	    Result = (uint32_t)adi_spi_EnableDmaMode(*phSPI, false);
   	}

}

void SPI0_Callback(void *pCBParam, uint32_t nEvent, void *pArg)
{
    switch(nEvent)
    {
        case (uint32_t)ADI_SPI_TRANSCEIVER_PROCESSED:
			/* set test end callback flag when last transceiver in test array is processed */
        		Flag_Tx0 = 0;
            break;
        default:
            break;
    }
}

void SPI1_Callback(void *pCBParam, uint32_t nEvent, void *pArg)
{
    switch(nEvent)
    {
        case (uint32_t)ADI_SPI_TRANSCEIVER_PROCESSED:
			/* set test end callback flag when last transceiver in test array is processed */
        		Flag_Tx1 = 0;
            break;
        default:
            break;
    }
}


void SpiCS_Set(BF609_SPIx SPIx, unsigned int CSx)		//低电平有效
{
	unsigned int i;
	i = CSx;
	i = ~i;
	switch(SPIx)
	{
	case	BF609_SPI0:
		*pREG_SPI0_SLVSEL &= i;

		break;
	case	BF609_SPI1:
		*pREG_SPI1_SLVSEL &= i;

		break;
	default:
			//可加入异常处理代码
		break;
	}
	spi_delay_n10ns( 8 );

}
void SpiCS_Clear(BF609_SPIx SPIx, unsigned int CSx)		//高电平有效
{
	switch(SPIx)
	{
	case	BF609_SPI0:
		*pREG_SPI0_SLVSEL |= CSx;

		break;
	case	BF609_SPI1:
		*pREG_SPI1_SLVSEL |= CSx;

		break;
	default:
				//可加入异常处理代码
		break;
		}
}

void SPIByte_Out(BF609_SPIx SPIx, unsigned char byte)
{

	if(SPIx == BF609_SPI0)
	{
		Flag_Tx0 = 1;
		*(SPI0Struct_Tx.pTransmitter) = byte;
		adi_spi_SubmitBuffer(phSPI0, &SPI0Struct_Tx);

		while(Flag_Tx0 != 0)
		{
			asm("nop;");
		}

	}else
	{
		Flag_Tx1 = 1;
		*(SPI1Struct_Tx.pTransmitter) = byte;
		adi_spi_SubmitBuffer(phSPI1, &SPI1Struct_Tx);
		while(Flag_Tx1 != 0)
		{
			asm("nop;");
		}
	}


}

unsigned char SPIByte_In(BF609_SPIx SPIx)
{
	unsigned char SPI_data;
	if(SPIx == BF609_SPI0)
	{
		Flag_Tx0 = 1;
		adi_spi_SubmitBuffer(phSPI0, &SPI0Struct_Rx);
		while(Flag_Tx0 != 0)
		{
			asm("nop;");
		}
		SPI_data = *(SPI0Struct_Rx.pReceiver);			//此处数据读取切记需要在等待装换后再进行阅读
	}else
	{
		Flag_Tx1 = 1;
		adi_spi_SubmitBuffer(phSPI1, &SPI1Struct_Rx);
		while(Flag_Tx1 != 0)
		{
			asm("nop;");
		}
		SPI_data = *(SPI1Struct_Rx.pReceiver);

	}

	return SPI_data;

}




