/*
 *
 *   (C) Copyright 2012 - Analog Devices, Inc.  All rights reserved.
 *
 *    FILE:     CGUInit.h ( )
 *
 *    PURPOSE:  Includes the macros, variables and function declarations for CGUInit.C
 *
 */

#ifndef CGUINIT_H_
#define CGUINIT_H_

#include <sysreg.h>
#include <ccblkfn.h>
#include <cdefBF609_rom.h>
#include <defBF609_rom.h>

/* macros for CLKOUT options */

#define	CLKIN 20//µµÎ»MHz
#define	CCLK_BY_4  1
#define	SYSCLK_BY_2 2
#define	SCLK0 3
#define	SCLK1 4
#define	DCLK_BY_2 5
#define	USB_PLL_CLK 6
#define	OUT_CLK 7
#define	USB_CLKIN 8

/* Default values for the parameters */
#define  MULTIPLIER_SEL		20
#define	 DF_SEL				false
#define  CCLK_SEL			1
#define  DDRCLK_SEL			2
#define	 SCLK0_SEL			2
#define	 SCLK1_SEL			2
#define	 SYSCLK_SEL			2
#define	 OUTCLK_SEL			4
#define  CLKOUT_SEL			CCLK_BY_4


/* PLL Multiplier and Divisor Selections (Required Value, Bit Position) */
#define MSEL(X)   ((X << BITP_CGU_CTL_MSEL) 	& BITM_CGU_CTL_MSEL) 	/* PLL Multiplier Select [1-127]: PLLCLK = ((CLKIN x MSEL/DF+1)) = 1000MHz(max) */
#define DF(X)	((X << BITP_CGU_CTL_DF) & BITM_CGU_CTL_DF )				/* Divide frequency[true or false] */
#define CSEL(X)   ((X  << BITP_CGU_DIV_CSEL)	& BITM_CGU_DIV_CSEL)	/* Core Clock Divisor Select [1-31]: (CLKIN x MSEL/DF+1)/CSEL = 500MHz(max) */
#define SYSSEL(X) ((X  << BITP_CGU_DIV_SYSSEL) & BITM_CGU_DIV_SYSSEL)	/* System Clock Divisor Select [1-31]: (CLKIN x MSEL/DF+1)/SYSSEL = 250MHz(max) */
#define S0SEL(X)  ((X  << BITP_CGU_DIV_S0SEL)	& BITM_CGU_DIV_S0SEL)	/* SCLK0 Divisor Select [1-7]: SYSCLK/S0SEL = 125MHz(max) */
#define S1SEL(X)  ((X  << BITP_CGU_DIV_S1SEL)	& BITM_CGU_DIV_S1SEL)	/* SCLK1 Divisor Select [1-7]: SYSLCK/S1SEL = 125MHz(max) */
#define DSEL(X)   ((X  << BITP_CGU_DIV_DSEL)	& BITM_CGU_DIV_DSEL) 	/* DDR Clock Divisor Select [1-31]: (CLKIN x MSEL/DF+1)/DSEL = 250MHz(max) */
#define OSEL(X)   ((X  << BITP_CGU_DIV_OSEL)	& BITM_CGU_DIV_OSEL) 	/* OUTCLK Divisor Select [1-127]: (CLKIN x MSEL/DF+1)/OSEL = 125MHz(max). */

#define CLKOUTSEL(X)   ((X  << BITP_CGU_CLKOUTSEL_CLKOUTSEL)	& BITM_CGU_CLKOUTSEL_CLKOUTSEL) 	/* CLKOUT Select [0-8] */


/*
 *	Function name:  CGU_Init
 *	Description:	This macro initializes the CGU
 *	Arguments:
 *	Parameter	 		|	Description		 					| Valid values      |
 *	iMultiplier	 		|Multiplier value						| 1 - 127           |
 *	iCCLKSel 			| CCLK SEL value						| 1 - 31			|
 *   iDDCLKSel			| DDR2CLK SEL value						| 1 - 31			|
 *   Return value:    None.
 *   Note: Multiplier value should be selected to make sure that PLLCLK does not exceed 1GHz.
 *         The parameters can be variable. This can accept a maximum of 9 parameters and a minimum
 *         one parameter. The other parameters will be initialized to the default values.
 *         iCCLKSel - 5
 *         iDDCLKSel - 2
 */
void CGU_Init(int iMultiplier, int iCCLKSel, int iDDCLKSel);
void GetSCLK0(float *freq, float *cycle);

#endif /* CGUINIT_H_ */
