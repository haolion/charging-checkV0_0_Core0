 
/*
**
** lib_cfg.h header file generated on ÆßÔÂ 3, 2015 at 14:45:59.
**
** Copyright (C) 2000-2015 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected in 
** the uC/LIB configuration editor. Changes to the uC/LIB configuration 
** should be made by changing the appropriate options rather than editing 
** this file.
**
** For descriptions of each parameter, please refer to the uC/LIB product Help.
**
*/

#ifndef _LIB_CFG_H_
#define _LIB_CFG_H_

#include <lib_def.h>

/*
*******************************************************************************
*                  MEMORY LIBRARY ARGUMENT CHECK CONFIGURATION
*
* Note(s) : (1) Configure LIB_MEM_CFG_ARG_CHK_EXT_EN to enable/disable the 
*               memory library suite external argument check feature :
*
*               (a) When ENABLED, arguments received from any port interface 
*                   provided by the developer or application are 
*                   checked/validated.
*
*               (b) When DISABLED, NO arguments received from any port 
*                   interface provided by the developer or application 
*                   are checked/validated.
*******************************************************************************
*/

         /* Configure external argument check feature (see Note #1) :        */
#define LIB_MEM_CFG_ARG_CHK_EXT_EN   (DEF_DISABLED)
            /*   DEF_DISABLED     Argument check DISABLED                    */
            /*   DEF_ENABLED      Argument check ENABLED                     */


/*
*******************************************************************************
*                  MEMORY LIBRARY ASSEMBLY OPTIMIZATION CONFIGURATION
*
* Note(s) : (1) Configure LIB_MEM_CFG_OPTIMIZE_ASM_EN to enable/disable 
* assembly-optimized memory function(s).
*******************************************************************************
*/

         /* Configure assembly-optimized function(s) [see Note #1] :         */
#define LIB_MEM_CFG_OPTIMIZE_ASM_EN (DEF_DISABLED) 
             /*   DEF_DISABLED     Assembly-optimized function(s) DISABLED   */
             /*   DEF_ENABLED      Assembly-optimized function(s) ENABLED    */

#if (LIB_MEM_CFG_OPTIMIZE_ASM_EN == DEF_ENABLED )
#if defined (__ADSPBLACKFIN__) || defined (__ADSP21000__)
#error "The Blackfin and SHARC processors do not implement assembly-optimized memory function(s)"
#endif
#endif

/*
*******************************************************************************
*                  MEMORY ALLOCATION CONFIGURATION
*
* Note(s) : (1) Configure LIB_MEM_CFG_ALLOC_EN to enable/disable memory 
*               allocation functions.
*
*           (2) (a) Configure LIB_MEM_CFG_HEAP_SIZE with the desired size of 
*               heap memory (in octets).
*
*               (b) Configure LIB_MEM_CFG_HEAP_BASE_ADDR to specify a base 
*               address for heap memory :
*
*                   (1) Heap initialized to specified application memory,  
*                   if LIB_MEM_CFG_HEAP_BASE_ADDR #define'd in 'app_cfg.h' 
*                       CANNOT #define to address 0x0
*
*                   (2) Heap declared to Mem_Heap[] in 'lib_mem.c',
*                   if LIB_MEM_CFG_HEAP_BASE_ADDR NOT #define'd in 'app_cfg.h'
*******************************************************************************
*/

/* Configure memory allocation feature (see Note #1) :              */
#define LIB_MEM_CFG_ALLOC_EN   (DEF_ENABLED)
             /*   DEF_DISABLED     Memory allocation DISABLED                */
             /*   DEF_ENABLED      Memory allocation ENABLED                 */

/* Allocation debugging information.                    */
/* Enable/disable allocation of debug information associated to each memory allocation.            */
#define LIB_MEM_CFG_DBG_INFO_EN   (DEF_DISABLED)


/* Configure heap memory size         [see Note #2a].                        */
#define LIB_MEM_CFG_HEAP_SIZE   8192u


/* Heap memory padding alignment (in bytes).            */
/* Configure the desired size of padding alignment of each buffer allocated from the heap. */
#define LIB_MEM_CFG_HEAP_PADDING_ALIGN   1u

/*
*******************************************************************************
*******************************************************************************
*                  STRING LIBRARY CONFIGURATION
*******************************************************************************
*******************************************************************************
*/

/*
*******************************************************************************
*                  STRING FLOATING POINT CONFIGURATION
*
* Note(s) : (1) Configure LIB_STR_CFG_FP_EN to enable/disable floating point 
*               string function(s).
*
*           (2) Configure LIB_STR_CFG_FP_MAX_NBR_DIG_SIG to configure the 
*               maximum number of significant digits to calculate &/or 
*               display for floating point string function(s).
*
*               See also 'lib_str.h  STRING FLOATING POINT DEFINES  Note #1'.
*******************************************************************************
*/

         /* Configure floating point feature(s) [see Note #1] :              */
#define LIB_STR_CFG_FP_EN   (DEF_DISABLED)
             /*   DEF_DISABLED     Floating point functions DISABLED         */
             /*   DEF_ENABLED      Floating point functions ENABLED          */

         /* Configure floating point feature(s)' number of ...               */
         /* ... significant digits (see Note #2).                            */
#define LIB_STR_CFG_FP_MAX_NBR_DIG_SIG   7u

#endif /* _LIB_CFG_H_ */

