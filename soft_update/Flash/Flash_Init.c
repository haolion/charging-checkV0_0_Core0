/*
 *  (C) Copyright 2012 - Analog Devices, Inc. All Rights Reserved.
 *
 * This software is proprietary and confidential.  By using this software
 * you agree to the terms of the associated Analog Devices License Agreement.
 *
 * Project Name:  	Power_On_Self_Test
 *
 * Hardware:		ADSP-BF609 EZ-Board
 *
 * Description:	This file tests the parallel flash on the EZ-Board.
 */

/*******************************************************************
*  include files
*******************************************************************/
#include <cdefBF609.h>
#include <ccblkfn.h>
#include <stdlib.h>
#include <math.h>
#include "common/dpia.h"
#include "common/flash_errors.h"
#include "pc28f128p33.h"

#include <services\pwr\adi_pwr.h>

#include "Flash_Init.h"

typedef enum
{
  ASYNC_SRAM,
  ASYNC_MODE,
  ASYNC_PAGE_MODE,
  SYNC_MODE,
} flash_mode_t;

#include <sys/platform.h>
#include <stdint.h>

#define SMC0_A03_PORTA_MUX  ((uint16_t) ((uint16_t) 0<<0))
#define SMC0_A04_PORTA_MUX  ((uint16_t) ((uint16_t) 0<<2))
#define SMC0_A05_PORTA_MUX  ((uint16_t) ((uint16_t) 0<<4))
#define SMC0_A06_PORTA_MUX  ((uint16_t) ((uint16_t) 0<<6))
#define SMC0_A07_PORTA_MUX  ((uint16_t) ((uint16_t) 0<<8))
#define SMC0_A08_PORTA_MUX  ((uint16_t) ((uint16_t) 0<<10))
#define SMC0_A09_PORTA_MUX  ((uint16_t) ((uint16_t) 0<<12))
#define SMC0_A10_PORTA_MUX  ((uint16_t) ((uint16_t) 0<<14))
#define SMC0_A11_PORTA_MUX  ((uint32_t) ((uint32_t) 0<<16))
#define SMC0_A12_PORTA_MUX  ((uint32_t) ((uint32_t) 0<<18))
#define SMC0_A13_PORTB_MUX  ((uint16_t) ((uint16_t) 0<<4))
#define SMC0_A14_PORTA_MUX  ((uint32_t) ((uint32_t) 0<<20))
#define SMC0_A15_PORTA_MUX  ((uint32_t) ((uint32_t) 0<<22))
#define SMC0_A16_PORTB_MUX  ((uint16_t) ((uint16_t) 0<<6))
#define SMC0_A17_PORTA_MUX  ((uint32_t) ((uint32_t) 0<<24))
#define SMC0_A18_PORTA_MUX  ((uint32_t) ((uint32_t) 0<<26))
#define SMC0_A19_PORTA_MUX  ((uint32_t) ((uint32_t) 0<<28))
#define SMC0_A20_PORTA_MUX  ((uint32_t) ((uint32_t) 0<<30))
#define SMC0_A21_PORTB_MUX  ((uint16_t) ((uint16_t) 0<<12))
#define SMC0_A22_PORTB_MUX  ((uint16_t) ((uint16_t) 0<<14))
#define SMC0_A23_PORTB_MUX  ((uint32_t) ((uint32_t) 0<<16))
#define SMC0_A24_PORTB_MUX  ((uint32_t) ((uint32_t) 0<<20))
#define SMC0_A25_PORTB_MUX  ((uint32_t) ((uint32_t) 0<<22))
#define SMC0_AMS1_PORTB_MUX  ((uint16_t) ((uint16_t) 0<<2))
#define SMC0_AMS2_PORTB_MUX  ((uint16_t) ((uint16_t) 0<<8))
#define SMC0_AMS3_PORTB_MUX  ((uint16_t) ((uint16_t) 0<<10))
#define SMC0_NORCLK_PORTB_MUX  ((uint16_t) ((uint16_t) 0<<0))
#define SMC0_BG_PORTB_MUX  ((uint32_t) ((uint32_t) 0<<24))
#define SMC0_BGH_PORTB_MUX  ((uint32_t) ((uint32_t) 0<<18))

#define SMC0_A03_PORTA_FER  ((uint16_t) ((uint16_t) 1<<0))
#define SMC0_A04_PORTA_FER  ((uint16_t) ((uint16_t) 1<<1))
#define SMC0_A05_PORTA_FER  ((uint16_t) ((uint16_t) 1<<2))
#define SMC0_A06_PORTA_FER  ((uint16_t) ((uint16_t) 1<<3))
#define SMC0_A07_PORTA_FER  ((uint16_t) ((uint16_t) 1<<4))
#define SMC0_A08_PORTA_FER  ((uint16_t) ((uint16_t) 1<<5))
#define SMC0_A09_PORTA_FER  ((uint16_t) ((uint16_t) 1<<6))
#define SMC0_A10_PORTA_FER  ((uint16_t) ((uint16_t) 1<<7))
#define SMC0_A11_PORTA_FER  ((uint32_t) ((uint32_t) 1<<8))
#define SMC0_A12_PORTA_FER  ((uint32_t) ((uint32_t) 1<<9))
#define SMC0_A13_PORTB_FER  ((uint16_t) ((uint16_t) 1<<2))
#define SMC0_A14_PORTA_FER  ((uint32_t) ((uint32_t) 1<<10))
#define SMC0_A15_PORTA_FER  ((uint32_t) ((uint32_t) 1<<11))
#define SMC0_A16_PORTB_FER  ((uint16_t) ((uint16_t) 1<<3))
#define SMC0_A17_PORTA_FER  ((uint32_t) ((uint32_t) 1<<12))
#define SMC0_A18_PORTA_FER  ((uint32_t) ((uint32_t) 1<<13))
#define SMC0_A19_PORTA_FER  ((uint32_t) ((uint32_t) 1<<14))
#define SMC0_A20_PORTA_FER  ((uint32_t) ((uint32_t) 1<<15))
#define SMC0_A21_PORTB_FER  ((uint16_t) ((uint16_t) 1<<6))
#define SMC0_A22_PORTB_FER  ((uint16_t) ((uint16_t) 1<<7))
#define SMC0_A23_PORTB_FER  ((uint32_t) ((uint32_t) 1<<8))
#define SMC0_A24_PORTB_FER  ((uint32_t) ((uint32_t) 1<<10))
#define SMC0_A25_PORTB_FER  ((uint32_t) ((uint32_t) 1<<11))
#define SMC0_AMS1_PORTB_FER  ((uint16_t) ((uint16_t) 1<<1))
#define SMC0_AMS2_PORTB_FER  ((uint16_t) ((uint16_t) 1<<4))
#define SMC0_AMS3_PORTB_FER  ((uint16_t) ((uint16_t) 1<<5))
#define SMC0_NORCLK_PORTB_FER  ((uint16_t) ((uint16_t) 1<<0))
#define SMC0_BG_PORTB_FER  ((uint32_t) ((uint32_t) 1<<12))
#define SMC0_BGH_PORTB_FER  ((uint32_t) ((uint32_t) 1<<9))


/*
 * Initialize the Port Control MUX and FER Registers
 */
int32_t SMC_PinInit(void)
{
    /* PORTx_MUX registers */
    *pREG_PORTA_MUX |= SMC0_A03_PORTA_MUX | SMC0_A04_PORTA_MUX
     | SMC0_A05_PORTA_MUX | SMC0_A06_PORTA_MUX | SMC0_A07_PORTA_MUX
     | SMC0_A08_PORTA_MUX | SMC0_A09_PORTA_MUX | SMC0_A10_PORTA_MUX
     | SMC0_A11_PORTA_MUX | SMC0_A12_PORTA_MUX | SMC0_A14_PORTA_MUX
     | SMC0_A15_PORTA_MUX | SMC0_A17_PORTA_MUX | SMC0_A18_PORTA_MUX
     | SMC0_A19_PORTA_MUX | SMC0_A20_PORTA_MUX;
    *pREG_PORTB_MUX |= SMC0_A13_PORTB_MUX | SMC0_A16_PORTB_MUX
     | SMC0_A21_PORTB_MUX | SMC0_A22_PORTB_MUX | SMC0_A23_PORTB_MUX
     | SMC0_A24_PORTB_MUX | SMC0_A25_PORTB_MUX | SMC0_AMS1_PORTB_MUX
     | SMC0_AMS2_PORTB_MUX | SMC0_AMS3_PORTB_MUX | SMC0_NORCLK_PORTB_MUX
     | SMC0_BG_PORTB_MUX | SMC0_BGH_PORTB_MUX;

    /* PORTx_FER registers */
    *pREG_PORTA_FER |= SMC0_A03_PORTA_FER | SMC0_A04_PORTA_FER
     | SMC0_A05_PORTA_FER | SMC0_A06_PORTA_FER | SMC0_A07_PORTA_FER
     | SMC0_A08_PORTA_FER | SMC0_A09_PORTA_FER | SMC0_A10_PORTA_FER
     | SMC0_A11_PORTA_FER | SMC0_A12_PORTA_FER | SMC0_A14_PORTA_FER
     | SMC0_A15_PORTA_FER | SMC0_A17_PORTA_FER | SMC0_A18_PORTA_FER
     | SMC0_A19_PORTA_FER | SMC0_A20_PORTA_FER;
    *pREG_PORTB_FER |= SMC0_A13_PORTB_FER | SMC0_A16_PORTB_FER
     | SMC0_A21_PORTB_FER | SMC0_A22_PORTB_FER | SMC0_A23_PORTB_FER
     | SMC0_A24_PORTB_FER | SMC0_A25_PORTB_FER | SMC0_AMS1_PORTB_FER
     | SMC0_AMS2_PORTB_FER | SMC0_AMS3_PORTB_FER | SMC0_NORCLK_PORTB_FER
     | SMC0_BG_PORTB_FER | SMC0_BGH_PORTB_FER;
    return 0;
}
/*******************************************************************
*  function prototypes
*******************************************************************/
ERROR_CODE SetupForFlash(flash_mode_t mode);

/*
 *   Function:    Init_ParallelFlash
 *   Description: This function initializes parallel flash.
 */
void Flash_Init(void)
{
	/*
	 * We need first configure it as asynchronous mode
	 * before configure it as synchronous mode
	*/
	SMC_PinInit();
	SetupForFlash(ASYNC_MODE);

}
void Flash_Reset(void)
{
	COMMAND_STRUCT pCmdBuffer;
	pCmdBuffer.SGetCodes.ulFlashStartAddr = 0xB0000000;
	pc28f128P33_Control( CNTRL_RESET, &pCmdBuffer );
}



/*
 *----------- S e t u p F o r F l a s h ( ) ----------
 *
 *  PURPOSE
 *   		Perform necessary setup for the processor to talk to the
 *   		flash such as external memory interface registers, etc.
 *
 *   RETURN VALUE
 *   	ERROR_CODE - value if any error occurs during Opcode scan
 *   	NO_ERR     - otherwise
 */
ERROR_CODE SetupForFlash(flash_mode_t mode)
{

	ERROR_CODE ErrorCode = NO_ERR;			/* return error code */
	float freq, cycle;
	int bclk;
	int wst, wht, wat, rst, rht, rat;
	int prest, preat, tt, it, pgws;
	uint32_t smc_b0ctl, smc_b0tim, smc_b0etim;
	/* For Read Configuration Register */
	int latency_count;
	float nor_clk;



	GetSCLK0(&freq, &cycle);


	/* calculate for PC28F128P33BF60 */

	if (mode == SYNC_MODE)
	  {
	    /* BCLK for synchronous burst read */
	    for (bclk = 0; bclk <= 3; bclk++)
	      if (freq / (bclk + 1) <= 52)
		break;

	    /* SCLK0 is too high, we can't get a valid BCLK by dividing it */
	    if (bclk == 4)
	      return SETUP_ERROR;

	    nor_clk = freq / (bclk + 1);
	    pc28f128P33_ConfigSyncModeRCR (nor_clk, &latency_count);
	  }

	/* Write setup time 2*/
	wst = 2 + 2;

	/* Write hold time 0*/
	wht = 0 + 1;

	/* Write access time >= 50ns */
	wat = ceilf (100 / cycle);

	if (mode == SYNC_MODE)
	{
		int j;

		/* See SMC section of the HW Reference Manual */
		j = 1;
		while (j * (bclk + 1) * cycle < 60 - 25)
			j++;
		rst = j;

		rat = (bclk + 1) * latency_count + 1 - rst;

		rht = bclk;
	}
	else
	{
		/* Read access time >= 25ns */
		rat = ceilf (35 / cycle);

		/* Read setup time 60*/
		rst = ceilf (75 / cycle) - rat;
		if (rst < 1)
			rst = 1;

		/* Read hold time >= 20ns */
		rht = ceilf (30 / cycle);
	}

	/* Pre setup time 0*/
	prest = 0 + 1;

	/* Pre access time 0*/
	preat = 0 + 1;

	/* Memory transition time */
	tt = 1;

	/* Memory idle time */
	it = 1;

	/* Page mode wait states */
	/* ceilf ((tCO + tAPA + tDS) / cycle) */
	/* tAPA is 25ns */
	pgws = ceilf (25 / cycle);
	if (pgws < 2)
	  pgws = 2;

	smc_b0tim = (((wst << BITP_SMC_B0TIM_WST) & BITM_SMC_B0TIM_WST)
		     | ((wht << BITP_SMC_B0TIM_WHT) & BITM_SMC_B0TIM_WHT)
		     | ((wat << BITP_SMC_B0TIM_WAT) & BITM_SMC_B0TIM_WAT)
		     | ((rst << BITP_SMC_B0TIM_RST) & BITM_SMC_B0TIM_RST)
		     | ((rht << BITP_SMC_B0TIM_RHT) & BITM_SMC_B0TIM_RHT)
		     | ((rat << BITP_SMC_B0TIM_RAT) & BITM_SMC_B0TIM_RAT));

	smc_b0etim = (((prest << BITP_SMC_B0ETIM_PREST) & BITM_SMC_B0ETIM_PREST)
		      | ((preat << BITP_SMC_B0ETIM_PREAT) & BITM_SMC_B0ETIM_PREAT)
		      | ((tt << BITP_SMC_B0ETIM_TT) & BITM_SMC_B0ETIM_TT)
		      | ((it << BITP_SMC_B0ETIM_IT) & BITM_SMC_B0ETIM_IT)
		      | ((pgws << BITP_SMC_B0ETIM_PGWS) & BITM_SMC_B0ETIM_PGWS));

	smc_b0ctl = ((1 << BITP_SMC_B0CTL_EN)
		     | (mode << BITP_SMC_B0CTL_MODE));
	switch (mode)
	  {
	  case ASYNC_MODE:
	    break;

	  case ASYNC_PAGE_MODE:
	    /* Set page size to 8 bytes according to the flash datasheet */
	    smc_b0ctl |= (1 << BITP_SMC_B0CTL_PGSZ);
	    break;

	  case SYNC_MODE:
	    smc_b0ctl |= (
			  /* Enable READY */
			  (1 << BITP_SMC_B0CTL_RDYEN)
			  | (0 << BITP_SMC_B0CTL_RDYPOL)
			  /* RDYABTEN can be used for debugging purpose */
			  /* | (1 << BITP_SMC_B0CTL_RDYABTEN) */
			  /* Set page size to 16 bytes */
			  | (2 << BITP_SMC_B0CTL_PGSZ)
			  | (bclk << BITP_SMC_B0CTL_BCLK)
			  /* BTYPE should be same as BW in flash RCR */
			  | (1 << BITP_SMC_B0CTL_BTYPE)
			  );
	    break;
	  default:
	    return SETUP_ERROR;
	  }

	*pREG_SMC0_B0TIM = smc_b0tim;
	asm(" ssync ; ");
	*pREG_SMC0_B0ETIM = smc_b0etim;
	asm(" ssync ; ");
	*pREG_SMC0_B0CTL = smc_b0ctl;
	asm(" ssync ; ");

	return NO_ERR;
}
