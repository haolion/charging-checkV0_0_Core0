
/*
**
** os_cfg.h source file generated on ÆßÔÂ 3, 2015 at 14:46:00.
**
** Copyright (C) 2000-2015 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected in
** the uC/OS-III configuration editor. Changes to the uC/OS-III configuration
** should be made by changing the appropriate options rather than editing 
** this file.
**
** For descriptions of each parameter, please refer to the uC/OS-III 
** product Help.
**
*/

#ifndef OS_CFG_H_
#define OS_CFG_H_

/* ---------------------------- MISCELLANEOUS -------------------------- */
#define OS_CFG_APP_HOOKS_EN                        0u 

#define OS_CFG_ARG_CHK_EN                          1u 
#define OS_CFG_CALLED_FROM_ISR_CHK_EN              1u 
#define OS_CFG_DBG_EN                              1u 
#define OS_CFG_ISR_POST_DEFERRED_EN                0u 
#define OS_CFG_OBJ_TYPE_CHK_EN                     1u 
#define OS_CFG_TS_EN                               0u 

#define OS_CFG_PEND_MULTI_EN                       1u 

#define OS_CFG_PRIO_MAX                            32u 

#define OS_CFG_SCHED_LOCK_TIME_MEAS_EN             0u 
#define OS_CFG_SCHED_ROUND_ROBIN_EN                0u 
#define OS_CFG_STK_SIZE_MIN                        32u 

/* ----------------------------- EVENT FLAGS --------------------------- */
#define OS_CFG_FLAG_EN                             1u 
#define OS_CFG_FLAG_DEL_EN                         1u 
#define OS_CFG_FLAG_MODE_CLR_EN                    1u 
#define OS_CFG_FLAG_PEND_ABORT_EN                  1u 

/* -------------------------- MEMORY MANAGEMENT ------------------------ */
#define OS_CFG_MEM_EN                              1u 

/* --------------------- MUTUAL EXCLUSION SEMAPHORES ------------------- */
#define OS_CFG_MUTEX_EN                            1u 
#define OS_CFG_MUTEX_DEL_EN                        1u 
#define OS_CFG_MUTEX_PEND_ABORT_EN                 1u 

/* --------------------------- MESSAGE QUEUES -------------------------- */
#define OS_CFG_Q_EN                                1u 
#define OS_CFG_Q_DEL_EN                            1u 
#define OS_CFG_Q_FLUSH_EN                          1u 
#define OS_CFG_Q_PEND_ABORT_EN                     1u 

/* ----------------------------- SEMAPHORES ---------------------------- */
#define OS_CFG_SEM_EN                              1u 
#define OS_CFG_SEM_DEL_EN                          1u 
#define OS_CFG_SEM_PEND_ABORT_EN                   1u 
#define OS_CFG_SEM_SET_EN                          1u 

/* -------------------------- TASK MANAGEMENT -------------------------- */
#define OS_CFG_STAT_TASK_EN                        0u 
#define OS_CFG_STAT_TASK_STK_CHK_EN                0u 

#define OS_CFG_TASK_CHANGE_PRIO_EN                 1u 
#define OS_CFG_TASK_DEL_EN                         1u 
#define OS_CFG_TASK_Q_EN                           1u 
#define OS_CFG_TASK_Q_PEND_ABORT_EN                1u 

#define OS_CFG_TASK_PROFILE_EN                     0u 
#define OS_CFG_TASK_REG_TBL_SIZE                   1u 
#define OS_CFG_TASK_SEM_PEND_ABORT_EN              1u 
#define OS_CFG_TASK_SUSPEND_EN                     1u 
#define OS_CFG_TLS_TBL_SIZE                        2u 

/* -------------------------- TIME MANAGEMENT -------------------------- */
#define OS_CFG_TIME_DLY_HMSM_EN                    1u 
#define OS_CFG_TIME_DLY_RESUME_EN                  1u 

/* ------------------------- TIMER MANAGEMENT -------------------------- */
#define OS_CFG_TMR_EN                              0u 
#define OS_CFG_TMR_DEL_EN                          0u 

#define ADI_CFG_CORE_TMR_EN                        1u 
#define ADI_CFG_CORE_TMR_COUNT                     500000u 


#endif /* OS_CFG_H_ */ 

