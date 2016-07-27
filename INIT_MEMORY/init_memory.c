/*
** init_memory.c
**
** Initialize memory programmatically 
** DMC is the Dynamic Memory Controller, the initialization is for DDR2 SDRAM
**
** Copyright (c) 2011-2012 Analog Devices, Inc. All Rights Reserved.
**/

#include "init_memory.h"
#include "cdefBF609.h"

void init_memory(void)
{
   unsigned int i, Current_CSEL_Val, Current_DSEL_Val, DLL_Lock_Cycles;

   /* Read the current CSEL and DSEL values to calculate the DLL lock cycles */

   Current_CSEL_Val = (*pREG_CGU0_DIV & BITM_CGU_DIV_CSEL) >> BITP_CGU_DIV_CSEL ;
   Current_DSEL_Val = (*pREG_CGU0_DIV & BITM_CGU_DIV_DSEL) >> BITP_CGU_DIV_DSEL ;

   DLL_Lock_Cycles = DLL_LOCK_PERIOD * (Current_DSEL_Val/Current_CSEL_Val);

   /* Wait for DLL to lock */
   for (i=0; i<DLL_Lock_Cycles; i++)
   {
      asm("nop;");
   }

   /* Keep the DLLCALRDCNT reset value and only modify DATACYC */
   *pREG_DMC0_DLLCTL= 0x54B;

   *pREG_DMC0_CFG  = (ADI_DMC_CFG_IFWID16) | (ADI_DMC_CFG_SDRWID16) | (ADI_DMC_CFG_SDRSIZE1G) | (ADI_DMC_CFG_EXTBANK1);
   *pREG_DMC0_TR0  = (ADI_DMC_TR0_TRCD4)   | (ADI_DMC_TR0_TWTR2)    | (ADI_DMC_TR0_TRP4)      | (ADI_DMC_TR0_TRAS10) | (ADI_DMC_TR0_TRC14) | (ADI_DMC_TR0_TMRD2);
   *pREG_DMC0_TR1  = (ADI_DMC_TR1_TRRD3)   | (ADI_DMC_TR1_TRFC32)   | (ADI_DMC_TR1_TREFVAL);
   *pREG_DMC0_TR2  = (ADI_DMC_TR2_TCKE3)   | (ADI_DMC_TR2_TXP2)     | (ADI_DMC_TR2_TRTP5)     | (ADI_DMC_TR2_TFAW13);
   *pREG_DMC0_MR   = (ADI_DMC_MR_WRRECOV4) | (ADI_DMC_MR_CL4)       | (ADI_DMC_MR_BLEN4);
   *pREG_DMC0_PHY_CTL3 = BITM_DMC_PHY_CTL3_ENODTDQ|BITM_DMC_PHY_CTL3_TMG0|BITM_DMC_PHY_CTL3_TMG1|BITM_DMC_PHY_CTL3_OFST0|BITM_DMC_PHY_CTL3_OFST1;
   *pREG_DMC0_PHY_CTL1 = ENUM_DMC_PHY_CTL1_ODT_75;
   *pREG_DMC0_CTL  = (BITM_DMC_CTL_INIT)   | (ADI_DMC_CTL_RDTOWR4);

   /* Wait for DDR2 initialization to complete */
   while(!(*pREG_DMC0_STAT & BITM_DMC_STAT_MEMINITDONE));

   /* Wait for Calibration to complete */
   while(!(*pREG_DMC0_STAT & BITM_DMC_STAT_DLLCALDONE));

}

