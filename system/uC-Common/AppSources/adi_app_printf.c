#include <app_cfg.h>

#if APP_CFG_TRACE_LEVEL != TRACE_LEVEL_OFF

#include <stdio.h>
#include <stdarg.h> 
/* 
   This function is called to print Application-specific Trace in uC-USB and 
   uC-FS. 
   The application can change the contents of the function but cannot change
   its name or prototype
*/

int adi_app_printf(const char* txt, ... )
{
   va_list p_vargs;
  
   va_start (p_vargs,txt);
   (void)vfprintf(stdout, txt, p_vargs);
   va_end (p_vargs);
   return 0;
}

#endif
