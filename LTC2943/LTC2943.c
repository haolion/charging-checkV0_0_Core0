/*
 * LTC2943.c
 *
 *  Created on: 2016-2-23
 *      Author: ºÀ_lion
 */
#include "LTC2943.h"

static uint8_t deviceMemory[ADI_TWI_MEMORY_SIZE];
static ADI_TWI_HANDLE masterDev;


static LTC2943_Result ReadRegister(uint8_t reg, uint8_t *value, uint8_t size);


LTC2943_Result LTC2943_ini(void)
{
	ADI_TWI_RESULT result;

	result = adi_twi_Open(TWIDEVNUM, ADI_TWI_MASTER, &deviceMemory[0],
			ADI_TWI_MEMORY_SIZE, &masterDev);
	if(result != ADI_TWI_SUCCESS)
	{
		return LTC2943False;
	}

	result = adi_twi_SetPrescale(masterDev, PRESCALEVALUE);
	if(result != ADI_TWI_SUCCESS)
	{
		return LTC2943False;
	}

	result = adi_twi_SetBitRate(masterDev, BITRATE);
	if(result != ADI_TWI_SUCCESS)
	{
		return LTC2943False;
	}

	result = adi_twi_SetDutyCycle(masterDev, DUTYCYCLE);
	if(result != ADI_TWI_SUCCESS)
	{
		return LTC2943False;
	}

	/* set LTC2943 slave address */
	result = adi_twi_SetHardwareAddress(masterDev, LTC2943ADDR);
	if(result != ADI_TWI_SUCCESS)
	{
		return LTC2943False;
	}

	return LTC2943True;

}

/*
 * Read a LTC2943 register value
 */
static LTC2943_Result ReadRegister(uint8_t reg, uint8_t *value, uint8_t size)
{
	uint8_t masterBuffer[BUFFER_SIZE];
	ADI_TWI_RESULT result;

	/* write register address */
	masterBuffer[0] = reg;
	result = adi_twi_Write(masterDev, masterBuffer, 1u, true);
	if(result != ADI_TWI_SUCCESS)
	{
		return LTC2943False;
	}

	/* read register value */
	if(size == 1)
	{
		result = adi_twi_Read(masterDev, value, 1u, false);
		if(result != ADI_TWI_SUCCESS)
		{
			return LTC2943False;
		}
	}else
	{
		result = adi_twi_Read(masterDev, value, 2u, false);
		if(result != ADI_TWI_SUCCESS)
		{
			return LTC2943False;
		}
	}

	return LTC2943True;

}

/*
 * Read a LTC2943 register value
 */
LTC2943_Result ReadLTC2943Status(uint8_t *value)
{
	LTC2943_Result result;

	result = ReadRegister(LTC2943STATUS, value, 1);
	if(result == LTC2943False)
	{
		Uart0_Transmit(&UART0_Data, "ReadRegister error.\n",  strlen("ReadRegister error.\n"));
		return LTC2943False;
	}

	return LTC2943True;

}


