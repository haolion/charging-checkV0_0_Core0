#ifndef SPI_H
#define SPI_H
#include "../AD7793_SPI/defines.h"
#include "cdefBF609.h"
#include <drivers/spi/adi_spi.h>

#define SPI0_CLK_PORTD_MUX  ((uint16_t) ((uint16_t) 0<<8))
#define SPI0_MISO_PORTD_MUX  ((uint16_t) ((uint16_t) 0<<4))
#define SPI0_MOSI_PORTD_MUX  ((uint16_t) ((uint16_t) 0<<6))
#define SPI0_SEL1_PORTD_MUX  ((uint32_t) ((uint32_t) 0<<22))
#define SPI1_CLK_PORTD_MUX  ((uint16_t) ((uint16_t) 0<<10))
#define SPI1_MISO_PORTD_MUX  ((uint32_t) ((uint32_t) 0<<28))
#define SPI1_MOSI_PORTD_MUX  ((uint32_t) ((uint32_t) 0<<26))
#define SPI1_SEL3_PORTD_MUX  ((uint32_t) ((uint32_t) 2<<20))

#define SPI0_CLK_PORTD_FER  ((uint16_t) ((uint16_t) 1<<4))
#define SPI0_MISO_PORTD_FER  ((uint16_t) ((uint16_t) 1<<2))
#define SPI0_MOSI_PORTD_FER  ((uint16_t) ((uint16_t) 1<<3))
#define SPI0_SEL1_PORTD_FER  ((uint32_t) ((uint32_t) 1<<11))
#define SPI1_CLK_PORTD_FER  ((uint16_t) ((uint16_t) 1<<5))
#define SPI1_MISO_PORTD_FER  ((uint32_t) ((uint32_t) 1<<14))
#define SPI1_MOSI_PORTD_FER  ((uint32_t) ((uint32_t) 1<<13))
#define SPI1_SEL3_PORTD_FER  ((uint32_t) ((uint32_t) 1<<10))


#define BF609SPI_CS1	BITM_SPI_SLVSEL_SSEL1
#define BF609SPI_CS2	BITM_SPI_SLVSEL_SSEL2
#define BF609SPI_CS3	BITM_SPI_SLVSEL_SSEL3
#define BF609SPI_CS4	BITM_SPI_SLVSEL_SSEL4
#define BF609SPI_CS5	BITM_SPI_SLVSEL_SSEL5
#define BF609SPI_CS6	BITM_SPI_SLVSEL_SSEL6
#define BF609SPI_CS7	BITM_SPI_SLVSEL_SSEL7

#define BF609SPI_CSEN1	BITM_SPI_SLVSEL_SSE1
#define BF609SPI_CSEN2	BITM_SPI_SLVSEL_SSE2
#define BF609SPI_CSEN3	BITM_SPI_SLVSEL_SSE3
#define BF609SPI_CSEN4	BITM_SPI_SLVSEL_SSE4
#define BF609SPI_CSEN5	BITM_SPI_SLVSEL_SSE5
#define BF609SPI_CSEN6	BITM_SPI_SLVSEL_SSE6
#define BF609SPI_CSEN7	BITM_SPI_SLVSEL_SSE7

#define NUM_SPITXBUFF	1
#define NUM_SPIRXBUFF	1


typedef enum BF609_SPIx
{
	BF609_SPI0 = 0u,
	BF609_SPI1 = 1u
}BF609_SPIx;


void spi_delay_n10ns(unsigned int n_10ns);

void SPIPIN_Init(void);//SPI管脚的复用使能，使用前必须根据使用情况进入修改，或者直接利用系统进行复用

void SPICallBack_Init(BF609_SPIx SPIx, unsigned int CSENx);

void SPI0_Callback(void *pCBParam, uint32_t nEvent, void *pArg);

void SPI1_Callback(void *pCBParam, uint32_t nEvent, void *pArg);

void SpiCS_Set(BF609_SPIx SPIx, unsigned int CSx);

void SpiCS_Clear(BF609_SPIx SPIx, unsigned int CSx);

//void spi_delay_n10ns(unsigned int n_10ns);

void SPIByte_Out(BF609_SPIx SPIx, unsigned char byte);

unsigned char SPIByte_In(BF609_SPIx SPIx);


#endif

