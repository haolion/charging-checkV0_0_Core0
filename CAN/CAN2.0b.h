
#ifndef CAN2_0B_H
#define CAN2_0B_H

#include <ccblkfn.h>
#include <sys/exception.h>
#include <signal.h>
#include "os.h"
#include <services/int/adi_int.h>
#include "stdio.h"
#include "../UARTx/UARTx.h"

#include <cdefBF609.h>

#if defined(__DEBUG_FILE__)
#include <string.h>
extern FILE *pDebugFile;				/* debug file when directing output to a file */
#endif

/*--------------------------------------------------------------------------
   constant defines
--------------------------------------------------------------------------*/
#define CAN_RX_MB_LO	0x00FF	/* Mailboxes 0-7 are RX, 8 - 15 are TX */
#define CAN_RX_MB_HI	0x0000	/* Mailboxes 16 - 31 are TX */

#define CAN_EN_MB_LO	0x00FF	/* Mailboxes 0-7 are RX, 8 - 15 are TX */
#define CAN_EN_MB_HI	0xFF00	/* Mailboxes 16 - 31 are TX */

#define UC_WDOG 		0x0002

#define BUF_SIZE		1024

//#define CANMODE_TX

/*--------------------------------------------------------------------------
 data typedefs
--------------------------------------------------------------------------*/
typedef struct g_CAN_BUF_TAG
{
	short zero;
	short one;
}g_CAN_BUF_type;

typedef struct g_CAN_RX_BUF
{
	unsigned short Mailbox_ID0;	//»ù±¾Ö¡£¬11bit
	unsigned int Mailbox_ID1;	//À©Õ¹Ö¡,18bit + 11bit = 29bit
	unsigned short Data0;
	unsigned short Data1;
	unsigned short Data2;
	unsigned short Data3;
}g_CAN_RX_BUF;


#define CAN_RX   100
#define CAN_TX   101
void TEST_CAN(void* arg);

/*--------------------------------------------------------------------------
 test function prototypes
--------------------------------------------------------------------------*/
void CAN_RCV_HANDLER(void);
void CAN_XMT_HANDLER(void);
void CAN_ERR_HANDLER(void);



#endif
