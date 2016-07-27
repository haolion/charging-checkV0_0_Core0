/*
**
** cpu_cfg.h header file generated on ÆßÔÂ 3, 2015 at 14:45:58.
**
** Copyright (C) 2000-2015 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected in 
** the uC/CPU configuration editor. Changes to the uC/CPU configuration should
** be made by changing the appropriate options rather than editing this file.
**
** For descriptions of each parameter, please refer to the uC/CPU product Help.
**
*/

#ifndef _CPU_CFG_H_
#define _CPU_CFG_H_

/*
*******************************************************************************
*                  CPU NAME CONFIGURATION
*
* Note(s) : (1) Configure CPU_CFG_NAME_EN to enable/disable CPU host name 
*               feature :
*
*               (a) CPU host name storage
*               (b) CPU host name API functions
*
*           (2) Configure CPU_CFG_NAME_SIZE with the desired ASCII string 
*               size of the CPU host name, including the terminating NULL 
*               character.
*
*               See also 'cpu_core.h  GLOBAL VARIABLES  Note #1'.
*******************************************************************************
*/

         /* Configure CPU host name feature (see Note #1) :                  */
#define CPU_CFG_NAME_EN   (DEF_DISABLED)
             /*   DEF_DISABLED  CPU host name DISABLED                       */
             /*   DEF_ENABLED   CPU host name ENABLED                        */

         /* Configure CPU host name ASCII string size (see Note #2).         */
#define CPU_CFG_NAME_SIZE   16u

/*
*******************************************************************************
*                  CPU TIMESTAMP CONFIGURATION
*
* Note(s) : (1) Configure CPU_CFG_TS_xx_EN to enable/disable CPU timestamp 
*               features :
*
*             (a) CPU_CFG_TS_32_EN enable/disable 32-bit CPU timestamp feature
*             (b) CPU_CFG_TS_64_EN enable/disable 64-bit CPU timestamp feature
*
*           (2) (a) Configure CPU_CFG_TS_TMR_SIZE with the CPU timestamp 
*               timer's word size :
*
*                       CPU_WORD_SIZE_08         8-bit word size
*                       CPU_WORD_SIZE_16        16-bit word size
*                       CPU_WORD_SIZE_32        32-bit word size
*                       CPU_WORD_SIZE_64        64-bit word size
*
*               (b) If the size of the CPU timestamp timer is not a binary 
*                   multiple of 8-bit octets (e.g. 20-bits or even 24-bits), 
*                   then the next lower, binary-multiple octet word size 
*                   SHOULD be configured (e.g. to 16-bits). However, the 
*                   minimum supported word size for CPU timestamp timers 
*                   is 8-bits.
*
*                   See also 'cpu_core.h  FUNCTION PROTOTYPES  CPU_TS_TmrRd() 
*                   Note #2a'.
*******************************************************************************
*/

         /* Configure CPU timestamp features (see Note #1) :                 */
#define CPU_CFG_TS_32_EN   (DEF_DISABLED)

#define CPU_CFG_TS_64_EN   (DEF_DISABLED)

#if defined (__ADSPBLACKFIN__) || defined (__ADSP21000__)
#if defined (DEF_ENABLED) && defined (CPU_CFG_TS_64_EN) && (CPU_CFG_TS_64_EN == DEF_ENABLED)
#error "The Blackfin and SHARC processors supports 32 bit timestamp only"
#endif
#endif
             /*   DEF_DISABLED  CPU timestamps DISABLED                      */
             /*   DEF_ENABLED   CPU timestamps ENABLED                       */

             /* Configure CPU timestamp timer word size ...                  */
             /* ... (see Note #2) :                                          */
#define CPU_CFG_TS_TMR_SIZE   (CPU_WORD_SIZE_32)

/*
*******************************************************************************
*                  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION
*
* Note(s) : (1) (a) Configure CPU_CFG_INT_DIS_MEAS_EN to enable/disable 
*                   measuring CPU's interrupts disabled time :
*
*              (a) Enabled, if CPU_CFG_INT_DIS_MEAS_EN #define'd in 'cpu_cfg.h'
*
*              (b) Disabled, if CPU_CFG_INT_DIS_MEAS_EN NOT #define'd 
*                  in 'cpu_cfg.h'
*
*                  See also 'cpu_core.h  FUNCTION PROTOTYPES  Note #1'.
*
*               (b) Configure CPU_CFG_INT_DIS_MEAS_OVRHD_NBR with the number 
*                   of times to measure & average the interrupts disabled 
*                   time measurements overhead.
*
*                   Recommend a single (1) overhead time measurement, even 
*                   for instruction-cache-enabled CPUs, since critical 
*                   sections are NOT typically called within instruction-
*                   cached loops. Thus, a single non-cached/non-averaged time 
*                   measurement is a more realistic overhead for the majority 
*                   of non-cached interrupts disabled time measurements.
*
*                   See also 'cpu_core.c  CPU_IntDisMeasInit()  Note #3a'.
*******************************************************************************
*/


/*
*******************************************************************************
*                  CPU COUNT ZEROS CONFIGURATION
*
* Note(s) : (1) (a) Configure CPU_CFG_LEAD_ZEROS_ASM_PRESENT to define count 
*                   leading  zeros bits function(s) in :
*
*                   (1) 'cpu_a.asm', if CPU_CFG_LEAD_ZEROS_ASM_PRESENT 
*                        #define'd in 'cpu.h' 'cpu_cfg.h' to enable 
*                        assembly-optimized function(s)
*
*                   (2) 'cpu_core.c', if CPU_CFG_LEAD_ZEROS_ASM_PRESENT NOT 
*                        #define'd in 'cpu.h' 'cpu_cfg.h' to enable C-source
*                        -optimized function(s) otherwise
*
*               (b) Configure CPU_CFG_TRAIL_ZEROS_ASM_PRESENT to define count 
*                   trailing zeros bits function(s) in :
*
*                   (1) 'cpu_a.asm', if CPU_CFG_TRAIL_ZEROS_ASM_PRESENT 
*                   #define'd in 'cpu.h' 'cpu_cfg.h' to enable assembly
*                   -optimized function(s)
*
*                   (2) 'cpu_core.c', if CPU_CFG_TRAIL_ZEROS_ASM_PRESENT NOT 
*                   #define'd in 'cpu.h' 'cpu_cfg.h' to enable C-source
*                   -optimized function(s) otherwise
*******************************************************************************
*/



/*
*********************************************************************************************************
*                                          CACHE MANAGEMENT
*
* Note(s) : (1) Configure CPU_CFG_CACHE_MGMT_EN to enable the cache managment API.

*
*           (2) Defining CPU_CFG_CACHE_MGMT_EN to DEF_ENABLED only enable the cache management function.
*               Cache are assumed to be configured and enabled by the time CPU_init() is called.
*********************************************************************************************************
*/

#define  CPU_CFG_CACHE_MGMT_EN            DEF_DISABLED          /* Defines CPU data    word-memory order (see Note #1). */



#if defined (CPU_CFG_TRAIL_ZEROS_ASM_PRESENT) && (defined (__ADSPBLACKFIN__) || defined (__ADSP21000__) )
#error "The Blackfin and SHARC port does not contain an assembly version of count trailing zeros bits"
#endif

#endif /* _CPU_CFG_H_ */

