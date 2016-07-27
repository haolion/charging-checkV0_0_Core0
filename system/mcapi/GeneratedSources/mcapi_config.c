/*
**
** mcapi_config.c source file generated on ÆßÔÂ 3, 2015 at 14:45:58.	
**
** Copyright (C) 2000-2015 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically based upon the options selected in 
** the MCAPI configuration editor. Changes to the MCAPI configuration should
** be made by changing the appropriate options rather than editing this file.
**
** For descriptions of each parameter, please refer to the CCES product Help.
**
*/

#include <services/mcapi/mcapi.h>
#include <adi_types.h>
#include <stddef.h>

#if defined (_MISRA_RULES)
#pragma diag(suppress:misra_rule_8_8:\
             "An external object or function shall be declared in one and only one file")
#pragma diag(suppress:misra_rule_8_10:\
             "All declarations and definitions of objects or functions at file scope\
             shall have internal linkage unless external linkage is required")
#endif /* _MISRA_RULES */

int32_t adi_mcapi_Init(void);

const mcapi_uint32_t _adi_mcapi_numPorts = (24u + 8u);
const mcapi_uint32_t _adi_mcapi_numAnonPorts = 8u;

#pragma align(4)
int8_t  _adi_mcapi_vLocalEndptSpace[(24u + 8u)*_ADI_MCAPI_ENDPOINT_SIZE];
size_t _adi_mcapi_LocalEndptSpaceSize = sizeof(_adi_mcapi_vLocalEndptSpace);

const mcapi_uint32_t _adi_mcapi_num_api_requests = 31u;

#pragma align(4)
int8_t _adi_mcapi_vAPIRequestSpace[31u*_ADI_MCAPI_REQUEST_SIZE];
size_t _adi_mcapi_APIRequestSpaceSize = sizeof(_adi_mcapi_vAPIRequestSpace);

const mcapi_uint32_t _adi_mcapi_num_rx_buffers = 1u;
const mcapi_uint32_t _adi_mcapi_rx_buffer_size = 256u;

#pragma align(4)
int8_t _adi_mcapi_vReceiveBufferSpace[1u * 256u];

#if defined(__NUM_CORES__) && (__NUM_CORES__ > 1)

const mcapi_uint32_t _adi_mcapi_num_isr_requests = 16u;

#pragma align(4)
int8_t _adi_mcapi_vISRRequestSpace[16u*_ADI_MCAPI_REQUEST_SIZE];
size_t _adi_mcapi_ISRRequestSpaceSize = sizeof(_adi_mcapi_vISRRequestSpace);

const mcapi_uint32_t _adi_mcapi_num_tx_buffers = 4u;
const mcapi_uint32_t _adi_mcapi_tx_buffer_size = 256u;

#pragma section ("L2_sram_uncached")
#pragma align(4)
int8_t _adi_mcapi_vTransmitBufferSpace[4u * 256u];

#endif /* __NUM_CORES__ */

/*
 * API called by adi_initComponents to initialize MCAPI. The parameters to mcapi_initialize 
 * are fixed and cannot be modified in any way. 
 */

mcapi_info_t  adi_mcapi_info;
mcapi_param_t adi_mcapi_init_parameters;

int32_t adi_mcapi_Init(void)
{
	mcapi_status_t mcapi_status;
	int32_t result = 0;

	/* Initialize MCAPI */

	mcapi_initialize(
		0u,                         /* the domain is always 0 */
		0u,                         /* Node ID */
		NULL,                       /* MCAPI node attributes */
		&adi_mcapi_init_parameters, /* MCAPI init parameters */
		&adi_mcapi_info,
		&mcapi_status );

	/* MCAPI status codes begin with 1 for MCAPI_SUCCESS, so it can't be 
	 * returned as a zero == success.
	 */
	if (MCAPI_SUCCESS != mcapi_status)
	{
		 result = mcapi_status; 
	} 

	return (result);
}

