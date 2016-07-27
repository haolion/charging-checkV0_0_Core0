
/*
**
** app_cfg.h source file generated on ÆßÔÂ 3, 2015 at 14:45:59.
**
** Copyright (C) 2000-2015 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected in the
** uC/APP configuration editor.
** Changes to the uC/APP configuration should be made by changing the
** appropriate options rather than editing this file.
**	
** For descriptions of each parameter, please refer to the uC/APP product Help.
**
*/

#ifndef APP_CFG_H
#define APP_CFG_H

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rules_all)
#endif /* _MISRA_RULES */

#ifndef TRACE_LEVEL_OFF
#define TRACE_LEVEL_OFF                          0
#endif
#ifndef TRACE_LEVEL_INFO
#define TRACE_LEVEL_INFO                         1
#endif
#ifndef TRACE_LEVEL_DBG
#define TRACE_LEVEL_DBG                          2
#endif

#if defined (ADI_UCOS3_PRODUCT)
#include "app_ucos3_cfg.h"
#endif
#if defined (ADI_UCOS2_PRODUCT)
#include "app_ucos2_cfg.h"
#endif
#if defined (ADI_UCFS_PRODUCT)
#include "app_ucfs_cfg.h"
#endif
#if defined (ADI_UCUSBD_PRODUCT)
#include "app_ucusbd_cfg.h"
#endif
#if defined (ADI_UCUSBH_PRODUCT)
#include "app_ucusbh_cfg.h"
#endif
#if defined (ADI_UCTCP_PRODUCT)
#include "app_uctcp_cfg.h"
#endif

/* Several of the Micrium products require uC-LIB settings */
#include <lib_cfg.h>

/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

extern int adi_app_printf(const char *txt, ...);

#ifndef APP_CFG_TRACE_LEVEL
#define APP_CFG_TRACE_LEVEL                      (TRACE_LEVEL_OFF)
#endif
#ifndef APP_CFG_TRACE
#define APP_CFG_TRACE                            adi_app_printf
#endif

#ifndef APP_TRACE_INFO
#define APP_TRACE_INFO(x)                        ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_CFG_TRACE x) : (void)(0))
#endif
#ifndef APP_TRACE_DBG
#define APP_TRACE_DBG(x)                         ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_CFG_TRACE x) : (void)(0))
#endif

#ifdef _MISRA_RULES
#pragma diag(pop)
#endif /* _MISRA_RULES */

#endif /* APP_CFG_H */

