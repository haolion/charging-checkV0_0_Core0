/*
 * (C) Copyright 2012 - Analog Devices, Inc.  All rights reserved.
 * This software is proprietary and confidential.  By using this software
 * you agree to the terms of the associated Analog Devices License Agreement.
 *
 * Project Name:  	Power_On_Self_Test
 *
 * Hardware:		ADSP-BF609 EZ-Board
 *
 * Description:	This file initializes the CGU on the ADSP-BF609 EZ-Board.
 */
#include "cgu_init.h"
//#include "post_common.h"
//#include "post_debug.h"

#if defined(__DEBUG_FILE__)
#include <string.h>
extern FILE *pDebugFile;				/* debug file when directing output to a file */
#endif

/*
 * Function name:  CGU_Init
 *
 *	Description:	This function initializes the CGU with the multiplier, CCLK SEL, DDR2CLK 
 *					SEL values passed. Divide frequency, SCLK0 SEL, SCLK1 SEL, SYSCLK SEL, 
 *					Output clock and CLKOUT use default values from the header file.
 *	Arguments:
 *	Parameter	 		| Description		 					| Valid values      |
 *	iMultiplier	 		| Multiplier value						| 1 - 127           |
 *	iCCLKSel 			| CCLK SEL value						| 1 - 31			|
 *   iDDCLKSel			| DDR2CLK SEL value						| 1 - 31			|
 *   Return value:    None.
 *   Note: Multiplier value should be selected to make sure that PLLCLK does not exceed 1GHz.
 */
void CGU_Init(int iMultiplier, int iCCLKSel, int iDDCLKSel)
{
	STRUCT_ROM_SYSCTRL ConfigValues;

	/* CGU */
	ConfigValues.ulCGU_CTL = MSEL(iMultiplier)|DF(false);
	ConfigValues.ulCGU_DIV = (OSEL(OUTCLK_SEL)) | (DSEL(iDDCLKSel)) | (S1SEL(SCLK1_SEL)) | (S0SEL(SCLK0_SEL)) | (SYSSEL(SYSCLK_SEL)) | (CSEL(iCCLKSel));
	ConfigValues.ulCGU_CLKOUTSEL = CLKOUTSEL(CLKOUT_SEL);

	rom_SysControl((BITM_ROM_SYSCTRL_CGU_WRITE 		|
					BITM_ROM_SYSCTRL_CGU_CTL 		|
					BITM_ROM_SYSCTRL_CGU_DIV 		|
					BITM_ROM_SYSCTRL_CGU_CLKOUTSEL),
					&ConfigValues, NULL);

	*pREG_CGU0_CLKOUTSEL = CLKOUTSEL(CLKOUT_SEL);
	__builtin_ssync();

#if defined(__DEBUG_UART__)
	SetBaud(iMultiplier*25000000);
#endif

}

void GetSCLK0(float *freq, float *cycle)
{
	int msel, csel, syssel, s0sel, df;
	uint32_t cgu_ctl, cgu_div;

	cgu_ctl = *pREG_CGU0_CTL;
	cgu_div = *pREG_CGU0_DIV;

	msel = (cgu_ctl & BITM_CGU_CTL_MSEL) >> BITP_CGU_CTL_MSEL;
	df = (cgu_ctl & BITM_CGU_CTL_DF) >> BITP_CGU_CTL_DF;
	csel = (cgu_div & BITM_CGU_DIV_CSEL) >> BITP_CGU_DIV_CSEL;
	syssel = (cgu_div & BITM_CGU_DIV_SYSSEL) >> BITP_CGU_DIV_SYSSEL;
	s0sel = (cgu_div & BITM_CGU_DIV_S0SEL) >> BITP_CGU_DIV_S0SEL;

	*freq = CLKIN * msel / (1 + df) / syssel / s0sel;
	*cycle = 1000 / *freq;
}

