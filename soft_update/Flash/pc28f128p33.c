/*********************************************************************************

Copyright(c) 2012 Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software you agree
to the terms of the associated Analog Devices License Agreement.

*********************************************************************************/

/* includes */
#include "common/dpia.h"
#include "common/flash_errors.h"
#include <stdlib.h>
#include <cdefBF609.h>
#include <ccblkfn.h>
#include "pc28f128P33.h"
#include <services/gpio/adi_gpio.h>

#define ERASE_TIMEOUT			50000000
#define PROGRAM_TIMEOUT			1000000
#define TRUE					true
#define FALSE					false
#define FLASH_START_ADDR		0xB0000000
#define SW_LOCKED_SECTOR		130						/* sector protected by this driver */

#define NUM_SECTORS		131

static char 	*pFlashDesc =		"Numonyx Axcell O33-65nm";
static char 	*pDeviceCompany	=	"Numonyx";

static int		gNumSectors = NUM_SECTORS;

/* function prototypes */
static ERROR_CODE EraseFlash(void);
static ERROR_CODE EraseBlock( int nBlock );
static ERROR_CODE GetCodes(int *pnManCode, int *pnDevCode, unsigned long ulStartAddr);
static ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector );
static ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector );
static ERROR_CODE PollToggleBit(unsigned long ulOffset, unsigned long timeout);
ERROR_CODE ReadFlash(unsigned long ulOffset, unsigned short *pusValue );
static ERROR_CODE ResetFlash(unsigned long ulStartAddr);
static ERROR_CODE WriteFlash(unsigned long ulOffset, unsigned short usValue );
static ERROR_CODE ReadStatusRegister(int *StatValue, unsigned long ulAddr);

bool IsStatusReady( unsigned long ulOffset );

/**
 *****************************************************************************
 * initialize anything needed before accessing the device
 *
 * @return				value if any error occurs
 */
ERROR_CODE pc28f128P33_Open(void)
{
	return (NO_ERR);
}

/**
 *****************************************************************************
 * do cleanup needed to close connection to the device
 *
 * @return				value if any error occurs
 */
ERROR_CODE pc28f128P33_Close(void)
{
	return (NO_ERR);
}

/**
 *****************************************************************************
 * Read from the flash device
 *
 * @param	pusData			pointer to the buffer to store flash contents in
 * @param	ulStartAddress	flash start address to read
 * @param	uiCount			number of locations to read( each byte is stored
 * 							as 16 bits so the upper byte is 0x00 )
 *
 * @return					value if any error occurs
 */
ERROR_CODE pc28f128P33_Read(unsigned short *pusData,
							unsigned long ulStartAddress,
							unsigned int uiCount )
{
	ERROR_CODE Result = NO_ERR;
	unsigned int i = 0;
	unsigned short *pusCurrentData = pusData;
    unsigned long ulCurrentAddress = ulStartAddress;

    /*
     * the address must be align in even, or access error will occur.
     */
	for (i = 0; i < uiCount; i++, ulCurrentAddress+=2, pusCurrentData++)
    {
		Result = ReadFlash( ulCurrentAddress, pusCurrentData );
	}

	return(Result);
}

/**
 *****************************************************************************
 * Write the flash device
 *
 * @param	pusData		pointer to the buffer to write
 * @param	addr		flash start address to write
 * @param	size		number of locations to write
 *
 * @return				value if any error occurs
 */
ERROR_CODE pc28f128P33_Write(unsigned short *pusData,
							 unsigned long ulStartAddress,
							 unsigned int uiCount )
{
    ERROR_CODE Result = NO_ERR;
    unsigned int i = 0;
    unsigned short *pusCurrentData = pusData;
    unsigned long ulCurrentAddress = ulStartAddress;
    unsigned long ulProtectSectStart = 0x0, ulProtectSectEnd = 0x0;

    /* if we are protecting a sector we need to know its boundaries */
#ifdef LOCK_CONFIG_FLASH_SECTOR
	GetSectorStartEnd( &ulProtectSectStart, &ulProtectSectEnd, SW_LOCKED_SECTOR );
	ulProtectSectStart += FLASH_START_ADDR;
	ulProtectSectEnd += FLASH_START_ADDR;
#endif

    for (i = 0; i < uiCount; i++, ulCurrentAddress+=2, pusCurrentData++)
    {
		/* if we are protecting a sector make sure this write doesn't fall in the sector */
#ifdef LOCK_CONFIG_FLASH_SECTOR
		if ( (ulCurrentAddress >= ulProtectSectStart) && (ulCurrentAddress <= ulProtectSectEnd) )
		{
			Result = NO_ACCESS_SECTOR;
			break;
		}
#endif
		/* unlock flash */
		WriteFlash( ulCurrentAddress, 0x60 );
		WriteFlash( ulCurrentAddress, 0xD0 );

		Result = PollToggleBit(ulCurrentAddress, PROGRAM_TIMEOUT);

		WriteFlash( ulCurrentAddress, 0x40 );

		/* program our actual value now */
		Result = WriteFlash( ulCurrentAddress, *pusCurrentData );

		/* make sure the write was successful */
		Result = PollToggleBit(ulCurrentAddress, PROGRAM_TIMEOUT);

		/* add by ChiliWang, strobe LED to indicate the erase process*/
		if(((i % 1024) == 0) && (Result == NO_ERR))
			adi_gpio_Toggle(ADI_GPIO_PORT_G, ADI_GPIO_PIN_13);
	}

	return(Result);
}

#define RM_ASYNC				0x8000
#define RM_SYNC					0x0000
#define BITP_LC					11
#define WP_ACTIVE_LOW			0x0000
#define WP_ACTIVE_HIGH			0x0400
#define DOC_ONE_DATA_CYCLE		0x0000
#define DOC_TWO_DATA_CYCLE		0x0200
#define WD_NO_DELAY				0x0000
#define WD_ONE_DATA_CYCLE		0x0100
#define BS_LINEAR				0x0080
#define CE_FALLING_EDGE			0x0000
#define CE_RISING_EDGE			0x0040
#define BW_WRAP					0x0000
#define BW_NO_WRAP				0x0080
#define BL_4_WORD				0x0001
#define BL_8_WORD				0x0002
#define BL_16_WORD				0x0003
#define BL_CONTINUOUS			0x0007

/**
 *****************************************************************************
 * configure the flash for synchronous reads
 *
 * @param	nor_clk			current nor clk
 * @param	latency_count	ptr to calculated latency count
 *
 * @return				value if any error occurs
 */
ERROR_CODE pc28f128P33_ConfigSyncModeRCR (float nor_clk, int *latency_count)
{
  ERROR_CODE ErrorCode = NO_ERR;
  uint32_t rcr;

  if (nor_clk <= 40)
    *latency_count = 3;
  else
    *latency_count = 4;

  rcr = (RM_SYNC | (*latency_count << BITP_LC) | WP_ACTIVE_HIGH
	 | WD_ONE_DATA_CYCLE | BS_LINEAR | CE_RISING_EDGE | BL_16_WORD);

  /* BW should be same as wrap setting in SMC */

  rcr |= BW_NO_WRAP;

  /* DOC -- Data Output Configuration
     If tCHQV + tDATA <= One NOR_CLK Period, then one data cycle is enough.
     Otherwise, if tCHQV + tDATA <= Two NOR_CLK Period, then two data cycles
     are needed.  Otherwise, NOR_CLK needs be lowered.

     tCHQV is 17ns.  */

  if (17 <= 1000 / nor_clk)
    rcr |= DOC_ONE_DATA_CYCLE;
  else if (17 <= (1000 / nor_clk) * 2)
    rcr |= DOC_TWO_DATA_CYCLE;

  WriteFlash (FLASH_START_ADDR, 0x60);
  WriteFlash (FLASH_START_ADDR + (rcr << 1), 0x03);

  return ErrorCode;
}
ERROR_CODE pc28f128P33_ConfigAsyncModeRCR (void)
{
  ERROR_CODE ErrorCode = NO_ERR;


  WriteFlash (FLASH_START_ADDR, 0x60);
  WriteFlash (FLASH_START_ADDR + (0XBFCF << 1), 0x03);

  return ErrorCode;
}

/**
 *****************************************************************************
 * This function takes a command and calls the appropriate function
 *
 * @param	uiCmd			command being processed
 * @param	pCmdStruct		information needed to process command
 *
 * @return					value if any error occurs
 */
ERROR_CODE pc28f128P33_Control(	unsigned int uiCmd, COMMAND_STRUCT *pCmdStruct )
{
	ERROR_CODE ErrorCode = NO_ERR;

	/* switch on the command */
	switch ( uiCmd )
	{
		/* erase all */
		case CNTRL_ERASE_ALL:
			ErrorCode = EraseFlash();
			break;

		/* erase sector */
		case CNTRL_ERASE_SECT:
			ErrorCode = EraseBlock( pCmdStruct->SEraseSect.nSectorNum );
			break;

		/* get manufacturer and device codes */
		case CNTRL_GET_CODES:
			ErrorCode = GetCodes((int *)pCmdStruct->SGetCodes.pManCode, (int *)pCmdStruct->SGetCodes.pDevCode, (unsigned long)pCmdStruct->SGetCodes.ulFlashStartAddr);
			break;

		case CNTRL_GET_DESC:
			/*Filling the contents with data */
			pCmdStruct->SGetDesc.pTitle = "ADSP-BF609 EZ-Board";
			pCmdStruct->SGetDesc.pDesc  = pFlashDesc;
			pCmdStruct->SGetDesc.pFlashCompany  = pDeviceCompany;
			break;

		/* get sector number based on address */
		case CNTRL_GET_SECTNUM:
			ErrorCode = GetSectorNumber( pCmdStruct->SGetSectNum.ulOffset, (int *)pCmdStruct->SGetSectNum.pSectorNum );
			break;

		/* get sector number start and end offset */
		case CNTRL_GET_SECSTARTEND:
			ErrorCode = GetSectorStartEnd( pCmdStruct->SSectStartEnd.pStartOffset, pCmdStruct->SSectStartEnd.pEndOffset, pCmdStruct->SSectStartEnd.nSectorNum );
			break;

		/* get the number of sectors */
		case CNTRL_GETNUM_SECTORS:
			pCmdStruct->SGetNumSectors.pnNumSectors[0] = gNumSectors;
			break;

		/* reset */
		case CNTRL_RESET:
			ErrorCode = ResetFlash(pCmdStruct->SReset.ulFlashStartAddr);
			break;

		/* no command or unknown command do nothing */
		default:
			/* set our error */
			ErrorCode = UNKNOWN_COMMAND;
			break;
	}

	/* return */
	return(ErrorCode);
}


/**
 *****************************************************************************
 * Reset the flash device
 *
 * @return			value if any error occurs
 */
ERROR_CODE ResetFlash(unsigned long ulAddr)
{
	/* send the reset command to the flash */
	WriteFlash( ulAddr, 0xff );

	/* reset should be complete */
	return NO_ERR;
}

/**
 *****************************************************************************
 * Reads a value from flash
 *
 * @param	ulAddr			address to read
 * @param	pusValue		ptr to the value read
 *
 * @return					value if any error occurs
 */
ERROR_CODE ReadFlash( unsigned long ulAddr, unsigned short *pusValue )
{

	/*
	* disable interrupts before performing the load or store operation
	* [refer warning: page 6-71 BF533 HRM]
	*/
	unsigned int uiSaveInts = cli();

	/* set our flash address to where we want to read */
	unsigned short *pFlashAddr = (unsigned short *)(ulAddr);

	/* read the value */
	*pusValue = (unsigned short)*pFlashAddr;

	/* Enable Interrupts */
    sti(uiSaveInts);

	/* ok */
	return NO_ERR;
}


/**
 *****************************************************************************
 * Writes a value to flash
 *
 * @param	ulAddr		address to write
 * @param	usValue		value that will be written
 *
 * @return				value if any error occurs
 */
ERROR_CODE WriteFlash( unsigned long ulAddr, unsigned short usValue )
{

	/*
	* disable interrupts before performing the load or store operation
	* [refer warning: page 6-71 BF533 HRM]
	*/
	unsigned int uiSaveInts = cli();

	/* set the address */
	unsigned short *pFlashAddr = (unsigned short *)(ulAddr);

	*pFlashAddr = usValue;

	/* Enable Interrupts */
    sti(uiSaveInts);

	/* ok */
	return NO_ERR;
}

/**
 *****************************************************************************
 * check the flash status to see if the transaction is done
 *
 * @param	ulOffset	address to write to check status
 *
 * @return				value if any error occurs
 */
bool IsStatusReady( unsigned long ulOffset )
{
    unsigned short status = 0;

 	WriteFlash(ulOffset, 0x0070 );

    ReadFlash(ulOffset, &status );

    if( ( status & 0x80 ) == 0x80 )
    {
        WriteFlash(ulOffset, 0x0050 );
    	return TRUE;
    }
    else
    	return FALSE;
}


/**
 *****************************************************************************
 * Erase just the block specified
 *
 * @param	pnManCode		pointer to manufacture code
 * @param	pnDevCode		pointer to device code
 * @param	ulAddr			flash start address
 *
 * @return					value if any error occurs
 */
ERROR_CODE GetCodes(int *pnManCode, int *pnDevCode, unsigned long ulAddr)
{

	unsigned long ulFlashStartAddr;
	unsigned short usLockConfig = 0;
	unsigned short usReadConfig = 0;

	/* set the flash start address */
	ulFlashStartAddr = ulAddr;

	/* send the auto select command to the flash */
	WriteFlash( ulFlashStartAddr, 0x90 );

 	/* now we can read the codes */
	ReadFlash( ulFlashStartAddr + 0x0000,(unsigned short *)pnManCode );
	*pnManCode &= 0x00FF;

	ReadFlash( ulFlashStartAddr + 0x0002, (unsigned short *)pnDevCode );
	*pnDevCode &= 0xFFFF;

	/*
	* we need to issue another command to get the part out
	* of auto select mode so issue a reset which just puts
	* the device back in read mode
	*/
	ResetFlash(ulAddr);

	/* ok */
	return NO_ERR;
}


/**
 *****************************************************************************
 * Read flash status register
 *
 * @param	StatValue		pointer to the status
 * @param	ulAddr			flash start address
 *
 * @return					value if any error occurs
 */
ERROR_CODE ReadStatusRegister(int *StatValue, unsigned long ulAddr)
{
	unsigned long ulFlashStartAddr = ulAddr;

	/* send the auto select command to the flash */
	WriteFlash( ulFlashStartAddr, 0x70 );

 	/* now we can read the codes */
	ReadFlash( ulFlashStartAddr, (unsigned short *)StatValue );

	/* now clear the status register */
	WriteFlash(ulFlashStartAddr, 0x0050 );

	/* ok */
	return NO_ERR;
}

/**
 *****************************************************************************
 * Erase just the block specified
 *
 * @param	nBlock		block to erase
 *
 * @return				value if any error occurs
 */
ERROR_CODE EraseBlock( int nBlock )
{

	ERROR_CODE 	  ErrorCode   = NO_ERR;		/* tells us if there was an error erasing flash */
 	unsigned long ulSectStart = 0x0;		/* stores the sector start offset */
 	unsigned long ulSectEnd   = 0x0;		/* stores the sector end offset(however we do not use it here) */
	unsigned long ulFlashStartAddr;			/* flash start address */

	/* if we are protecting a sector in software and it's this one, return */
#ifdef LOCK_CONFIG_FLASH_SECTOR
	if( nBlock == SW_LOCKED_SECTOR )
		return NO_ACCESS_SECTOR;
#endif

 	/* get flash start address from absolute address */
	ulFlashStartAddr = FLASH_START_ADDR;

	if ( (nBlock < 0) || (nBlock > gNumSectors)  )
		return INVALID_BLOCK;

	ErrorCode = GetSectorStartEnd( &ulSectStart, &ulSectEnd, nBlock );

	if( ErrorCode == NO_ERR )
	{
		/* unlock this block */
		WriteFlash( (ulFlashStartAddr + ulSectStart), 0x60 );
		WriteFlash( (ulFlashStartAddr + ulSectStart), 0xD0 );

		ErrorCode = PollToggleBit((ulFlashStartAddr + ulSectStart), ERASE_TIMEOUT);

		/* erase block */
		WriteFlash( (ulFlashStartAddr + ulSectStart), 0x20 );
		WriteFlash( (ulFlashStartAddr + ulSectStart), 0xD0 );

		ErrorCode = PollToggleBit((ulFlashStartAddr + ulSectStart), ERASE_TIMEOUT);

		WriteFlash( (ulFlashStartAddr + ulSectStart), 0xFF );
	}

	/* block erase should be complete */
	return ErrorCode;
}


/**
 *****************************************************************************
 * Erase the entire flash device
 *
 * @return				value if any error occurs
 */
ERROR_CODE EraseFlash(void)
{
	ERROR_CODE ErrorCode = NO_ERR;	/* tells us if there was an error erasing flash */
	int nBlock = 0;

	for( nBlock = 0; ( nBlock < (gNumSectors - 1) ) && ( ErrorCode == NO_ERR ); nBlock++ )
		ErrorCode = EraseBlock( nBlock );

	/* erase should be complete */
	return ErrorCode;
}

/**
 *****************************************************************************
 * Gets a sector number based on the offset.
 *
 * @param	ulAddr			absolute address
 * @param	pnSector		pointer to sector number
 *
 * @return					value if any error occurs
 */
ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector )
{
	int nSector = 0;
	unsigned long ulMask;					/*offset mask */
	unsigned long ulOffset;					/*offset */
	unsigned long ulStartOff;
	unsigned long ulEndOff;
	int i = 0;

	ulMask      	  = 0x7FFFFF;
	ulOffset		  = ulAddr & ulMask;

	for(i = 0; i < gNumSectors; i++)
	{
	    GetSectorStartEnd(&ulStartOff, &ulEndOff, i);
		if ( (ulOffset >= ulStartOff)
			&& (ulOffset <= ulEndOff) )
		{
			nSector = i;
			break;
		}
	}

	/* if it is a valid sector, set it */
	if ( (nSector >= 0) && (nSector < gNumSectors) )
		*pnSector = nSector;

	/* else it is an invalid sector */
	else
		return INVALID_SECTOR;

	/* ok */
	return NO_ERR;
}


/**
 *****************************************************************************
 * Gets a sector start and end address based on the sector number.
 *
 * @param	ulStartOff		pointer to the start offset
 * @param	ulEndOff		pointer to the end offset
 * @param	nSector			sector number
 *
 * @return					value if any error occurs
 */
ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector )
{
	long lSmallSectorSize = 0x8000;
	long lLargeSectorSize = 0x20000;
	long lLargeSectorStart = 0x20000;

	if( ( nSector >= 0 ) && ( nSector <= 3 ) )
	{
		*ulStartOff = nSector * (lSmallSectorSize );
		*ulEndOff = ( (*ulStartOff) + (lSmallSectorSize ) );
	}
	else if( ( nSector >= 4 ) && ( nSector < gNumSectors ) )
	{
		*ulStartOff = lLargeSectorStart + (lLargeSectorSize *  (nSector - 4));
		*ulEndOff = ( (*ulStartOff) + (lLargeSectorSize) );
	}
	/* no such sector */
	else
		return INVALID_SECTOR;


	*ulEndOff -= 1;

	/* ok */
	return NO_ERR;
}

/**
 *****************************************************************************
 * Checks to see if the operationg has completed
 *
 * @return				value if any error occurs
 */
ERROR_CODE PollToggleBit(unsigned long ulOffset, unsigned long timeout)
{

	while ( timeout )
    {

        if ( IsStatusReady( ulOffset ) )
        {
            WriteFlash( ulOffset, 0xFF );
            return( NO_ERR );
        }
        timeout--;
    }

    return( POLL_TIMEOUT );

}


