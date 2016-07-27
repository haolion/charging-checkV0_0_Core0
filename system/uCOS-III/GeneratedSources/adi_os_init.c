
/*
**
** adi_os_init.c source file generated on ÆßÔÂ 3, 2015 at 14:46:00.
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

#include <os.h>
#include <stdint.h>



int32_t adi_OS_Init(void);

/*!
  ****************************************************************************
    @brief Initializes uC/OS-III.

    This function initializes the OS. This is called from adi_initComponents
    called during the system startup.
    adi_initComponents requires a prototype int func(void)

    @return 0          - Initialization is done successfully.
    @return integer    - OS failed to initialize

*****************************************************************************/

int32_t adi_OS_Init(void){

    OS_ERR initError;
    OSInit(&initError);
    /* 
       OS_ERR_NONE is 0 so this function can always return the value that
       OSInit returned 
     */
     
    return initError;
}


