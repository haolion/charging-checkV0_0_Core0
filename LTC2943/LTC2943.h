/*
 * LTC2943.h
 *
 *  Created on: 2016-2-23
 *      Author: 豪_lion
 */

#ifndef LTC2943_H_
#define LTC2943_H_
#include <drivers/twi/adi_twi.h>
#include "defines.h"
#include "../UARTx/UARTx.h"

extern UART_Data_Struct UART0_Data;//串口结构体，用于缓存

#define Debug_LTC2943
typedef enum LTC2943_Result
{
	LTC2943True,
	LTC2943False
}LTC2943_Result;

#define LTC2943ADDR 	   (0x64u)     /* hardware address for the Monitor (LTC2943) */
#define LTC2943BITRATE     (100u)      /* kHz */

/*Register Map*/
#define LTC2943STATUS	0x00
#define LTC2943CONTROL	0x01
#define LTC2943ACMSB	0x02
#define LTC2943ACLSB	0x03
#define LTC2943ChTHMSB	0x04
#define	LTC2943ChTHLSB	0x05
#define LTC2943ChTLMSB	0x06
#define	LTC2943ChTLLSB	0x07
#define LTC2943VMSB		0x08
#define LTC2943VLSB		0x09
#define LTC2943VTHMSB	0x0a
#define LTC2943VTHLSB	0x0b
#define LTC2943VTLMSB	0x0c
#define LTC2943VTLLSB	0x0d
#define LTC2943CMSB		0x0e
#define LTC2943CLSB		0x0f
#define LTC2943CTHMSB	0x10
#define	LTC2943CTHLSB	0x11
#define LTC2943CTLMSB	0x12
#define LTC2943CTLLSB	0x13
#define LTC2943TMSB		0x14
#define LTC2943TLSB		0x15
#define LTC2943TTH		0x16
#define LTC2943TTL		0x17

/*STATUS REGISTER*/
#define LTC2943ALERT_UVL	BIT0
#define LTC2943ALERT_V		BIT1
#define LTC2943ALERT_CL		BIT2
#define LTC2943ALERT_CH		BIT3
#define LTC2943ALERT_T		BIT4
#define LTC2943ALERT_ACOU	BIT5
#define LTC2943ALERT_A		BIT6

/*CONTROL REGISTER*/
#define	LTC2943MODE_AUTO	~(BIT7|BIT6)



/*BF609 Config*/
#define TWIDEVNUM     (0u)        /* TWI device number */
#define BITRATE       (100u)      /* kHz */
#define DUTYCYCLE     (50u)       /* percent */
#define PRESCALEVALUE (125u/10u)  /* fSCLK/10MHz */


#define BUFFER_SIZE   (8)


LTC2943_Result LTC2943_ini(void);
LTC2943_Result ReadLTC2943Status(uint8_t *value);




#endif /* LTC2943_H_ */
