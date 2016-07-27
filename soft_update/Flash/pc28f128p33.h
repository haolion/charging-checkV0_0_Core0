/*******************************************************************************/
/*                                                                             */
/*   (C) Copyright 2008 - Analog Devices, Inc.  All rights reserved.           */
/*                                                                             */
/*    FILE:     pc28f128P33.h                                                       */
/*																			   */
/*    PURPOSE:  This header file defines items specific to the pc28f128P33 flash.   */
/*                                                                             */
/*******************************************************************************/

#ifndef __PC28F128P33_H__
#define __PC28F128P33_H__
//#define FLASH_START_ADDR		0xB0000000
#include "./common/flash_errors.h"
#include "./common/dpia.h"
#include "FlashAddress_map.h"
/* interface function prototypes */
ERROR_CODE pc28f128P33_Open(void);						/* open the flash device */
ERROR_CODE pc28f128P33_Close(void);						/* close the flas device */
ERROR_CODE pc28f128P33_Read(unsigned short *pusData,		/* read some bytes from flash */
					   unsigned long ulStartAddress,
					   unsigned int uiCount );
ERROR_CODE pc28f128P33_Write(unsigned short *pusData,	/* write some bytes to flash */
					    unsigned long ulStartAddress,
					    unsigned int uiCount );
ERROR_CODE pc28f128P33_Control(unsigned int uiCmd,		/* send a command to device */
						  COMMAND_STRUCT *pCmdStruct);
ERROR_CODE pc28f128P33_ConfigSyncModeRCR (float nor_clk, int *latency_count);

#endif	/* __PC28F128P33_H__ */
