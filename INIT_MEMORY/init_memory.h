/*
** init_memory.h
**
** Initialize DDR2 SDRAM memory programmatically
**
** Note that DMC is the aconym for the Dynamic Memory Controller
**
** Copyright (c) 2011-2012 Analog Devices, Inc. All Rights Reserved.
**/

#ifndef __INIT_MEMORY_H__
#define __INIT_MEMORY_H__

//#include "init_platform.h"
#define __CONFIG_MEMORY__
#if defined(__CONFIG_MEMORY__)

/*

 For the case,  tDD2CLK = 1/250 MHz = 4ns
	CAS Latency = 4
	tRCD = 15/4=  4 (3.75)
	tWTR = 7.5/4 = 2 (1.85)
	tRP = 15/4 = 4 (3.75)
 	tRAS = 40/4 = 10
 	tRC = 55/4 = 14
	tMRD = 2
	tRRD = 10/4 = 3 (2.5)
	tRFC = 127.5/4 = 32(31.875) = 0x20
 	tCKE = 3
 	tXP = 2
 	tRTP = 7.5/4 = 2(1.85) // Keep it 5 ONLY
 	tFAW = 50/4 =  13(12.5)
 	tWR = 15/4 = 4 (3.75)
 	RDIV = ((f DDCLK X t REFI))
	RDIV = (250 X 10^6 x 7.8 x 10^-6)
	RDIV = 1950

*/


#define DLL_LOCK_PERIOD 4500

/* CFG bits */
#define ADI_DMC_CFG_IFWID16           (BITM_DMC_CFG_IFWID   & (ENUM_DMC_CFG_IFWID16))      /* Interface Width is 16-bits */
#define ADI_DMC_CFG_SDRSIZE1G         (BITM_DMC_CFG_SDRSIZE & (ENUM_DMC_CFG_SDRSIZE1G))    /* Individual SDRAM is 1Gbit */
#define ADI_DMC_CFG_SDRWID16          (BITM_DMC_CFG_SDRWID  & (ENUM_DMC_CFG_SDRWID16))     /* Individual SDRAM is 16-bits wide */
#define ADI_DMC_CFG_EXTBANK1          (BITM_DMC_CFG_EXTBANK & (ENUM_DMC_CFG_EXTBANK1))     /* 1 external bank */

/* TR0 bits */
#define ADI_DMC_TR0_TRCD4             (BITM_DMC_TR0_TRCD & (4<< BITP_DMC_TR0_TRCD))
#define ADI_DMC_TR0_TWTR2             (BITM_DMC_TR0_TWTR & (2 << BITP_DMC_TR0_TWTR))
#define ADI_DMC_TR0_TRP4              (BITM_DMC_TR0_TRP & (4 << BITP_DMC_TR0_TRP))
#define ADI_DMC_TR0_TRAS10            (BITM_DMC_TR0_TRAS & (10 << BITP_DMC_TR0_TRAS))
#define ADI_DMC_TR0_TRC14             (BITM_DMC_TR0_TRC & (14 << BITP_DMC_TR0_TRC))
#define ADI_DMC_TR0_TMRD2             (BITM_DMC_TR0_TMRD & (2 << BITP_DMC_TR0_TMRD))

/* TR1 bits */
#define ADI_DMC_TR1_TRRD3             (BITM_DMC_TR1_TRRD & (3 << BITP_DMC_TR1_TRRD))
#define ADI_DMC_TR1_TRFC32            (BITM_DMC_TR1_TRFC & (32 << BITP_DMC_TR1_TRFC))
#define ADI_DMC_TR1_TREFVAL           (BITM_DMC_TR1_TREF & (1950 << BITP_DMC_TR1_TREF))

/* TR2 bits */
#define ADI_DMC_TR2_TCKE3             (BITM_DMC_TR2_TCKE & (3 << BITP_DMC_TR2_TCKE))
#define ADI_DMC_TR2_TXP2              (BITM_DMC_TR2_TXP & (2 << BITP_DMC_TR2_TXP))
#define ADI_DMC_TR2_TRTP5             (BITM_DMC_TR2_TRTP & (5 << BITP_DMC_TR2_TRTP))
#define ADI_DMC_TR2_TFAW13            (BITM_DMC_TR2_TFAW & (13 << BITP_DMC_TR2_TFAW))

/* MR bits */
#define ADI_DMC_MR_WRRECOV4           (BITM_DMC_MR_WRRECOV & (4 << BITP_DMC_MR_WRRECOV))
#define ADI_DMC_MR_CL4                (BITM_DMC_MR_CL      & (ENUM_DMC_MR_CL4))            /* 4 clock cycle latency (DMC2) */
#define ADI_DMC_MR_BLEN4              (BITM_DMC_MR_BLEN    & (ENUM_DMC_MR_BLEN4))          /* Burst Length 4 */

/* EMR1 bits */
#define ADI_DMC_EMR1_AL0              (BITM_DMC_EMR1_AL & (0 << BITP_DMC_EMR1_AL))

/* CTL bits */
#define ADI_DMC_CTL_RDTOWR4           (BITM_DMC_CTL_RDTOWR & (ENUM_DMC_CTL_RDTOWR4))       /* Four extra cycles added */

#endif /* __CONFIG_MEMORY__ */

void init_memory(void);

#endif /* __INIT_MEMORY_H__ */

