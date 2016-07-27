///*
// * update.c
// *
// *  Created on: 2014-8-27
// *      Author: ChiliWang
// */
//
///*
// * version update module
// */
//#include "comm_pc_protocol.h"
//
//#include "Flash/common/dpia.h"
//#include "Flash/common/flash_errors.h"
//#include "flash/pc28f128P33.h"
//#include "Flash/Flash_Init.h"
//#include "VerUpgrade.h"
//#include <services/gpio/adi_gpio.h>
//
//#include <adi_osal.h>
//#define FLASH_START_ADDR 0xB0000000
//#include <stdlib.h>
//
//#include "post_debug.h"
//#include "dri_adi_gemac.h"
//
//char VerDesc[512];
//
//uint8_t     *g_pLdrFileBuffer = NULL;//Global parameter, pointer of ldr data buffer
//uint32_t	g_LdrFileBufSize = 0;
//VER_UPDATE_ACK_INFO VerAckInfo[7]=
//{
//	{ ACK_OK, "ok : CRC Pass !\n\n"},
//	{ ACK_FRM_OK,"ok: current frm!\n\n "},
//	{ NAK_ERROR_MEM_ALLOC_FAILED, "failed: alloc memory for ldr file data buffer!"},
//	{ NAK_ERROR_INDEX, "failed : LDR_Index > LDR_Len !\n\n"},
//	{ NAK_ERROR_FILE_CRC, "failed : CRC error !\n\n"},
//	{ NAK_ERROR_UNKNOWN_COMMAND, 0},
//	{ NAK_ERROR_DESTADDR_UNMATCH, 0},
//};
//
///* init the LED service
// * used for indicating the update process
//  */
//void static  myLED_KEY_Init(void)
// {
//	 ADI_GPIO_RESULT result;
//
//	/* set GPIO output LED 1 */
//	result = adi_gpio_SetDirection(
//		ADI_GPIO_PORT_G,
//		ADI_GPIO_PIN_13,
//		ADI_GPIO_DIRECTION_OUTPUT);
//	if (result != ADI_GPIO_SUCCESS) {
//		printf("%s failed\n", result);
//	}
//	/* LED1 */
//	result = adi_gpio_Set(ADI_GPIO_PORT_G, ADI_GPIO_PIN_13);
//	if (result != ADI_GPIO_SUCCESS) {
//		printf("%s failed\n", result);
//	}
// }
///*
// * CRC16 calculate (int start, char *p, int n)
// */
//static uint16_t GetCrc16(int start,unsigned char *p, int n)
//{
//  int crc = start;
//  register int r;
//  static uint16_t crc_16_table[16] = {
//	  0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
//	  0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
//	};
//   /* while there is more data to process */
//   while (n-- > 0) {
//     /* compute checksum of lower four bits of *p */
//     r = crc_16_table[crc & 0xF];
//     crc = (crc >> 4) & 0x0FFF;
//     crc = crc ^ r ^ crc_16_table[*p & 0xF];
//
//     /* now compute checksum of upper four bits of *p */
//     r = crc_16_table[crc & 0xF];
//     crc = (crc >> 4) & 0x0FFF;
//     crc = crc ^ r ^ crc_16_table[(*p >> 4) & 0xF];
//
//     /* next... */
//     p++;
//   }
//   return(crc);
//}
//
//
///*
// * compare the buffer
// */
//bool static compare(uint8_t *Flash_Src_Buffer, uint8_t *Flash_Dest_Buffer, size_t len)
//{
//	int i;
//	for(i = 0; i < len; i++)
//	{
//		if(Flash_Src_Buffer[i] != Flash_Dest_Buffer[i])
//			return false;
//	}
//	return true;
//}
///*
// * flash erase use buffer
// */
//void FlashErase(uint8_t *pBuffer, size_t len)
//{
//	 COMMAND_STRUCT CmdBuffer;
//	 ERROR_CODE Result = NO_ERR;
//
////	 Flash_Init();//setup the pin mux and init flash
////
////	 myLED_KEY_Init();
//
//	 adi_gpio_Clear(ADI_GPIO_PORT_G, ADI_GPIO_PIN_13);/* on the LED to indicate the erase Flash process */
//
//
//	 /*
//	 * 0-0x20000 (16K*2Byte*4Block),
//	 * 0x20000- 7FFFFF(64K*2Byte*127Block)
//     * calculate the number of block to be erased.
//     */
//	 CmdBuffer.SEraseSect.ulFlashStartAddr = FLASH_START_ADDR;
//	 for(int block = 0; block < (len /(128*1024) + 4); block++)
//	 {
//		 CmdBuffer.SEraseSect.nSectorNum       = block;
//		 Result = pc28f128P33_Control(CNTRL_ERASE_SECT, &CmdBuffer);
//	 }
//
//	 if(Result == NO_ERR) //write the buffer to 16-width flash
//		 Result = pc28f128P33_Write((uint16_t *)pBuffer, FLASH_START_ADDR, (len+1) >> 1);
//
//	 adi_gpio_Set(ADI_GPIO_PORT_G, ADI_GPIO_PIN_13);/*indicate the erase process.*/
//
//	if(Result == NO_ERR) //Software Reset
//	{
//		// Flash_Reset();
//		*pREG_RCU0_CTL |= 0x01;
//	}
//
//}
//
//
///*
// * Upgrade the program.
// * notes:
// * 		  before call this function, user must has ldr data saved in global parameter g_pLdrFileBuffer.
// */
//void VerUpgrade(void)
//{
//	/*
//	 * Erase the flash
//	 */
//
//	 adi_osal_EnterCriticalRegion();
//
//	 FlashErase(g_pLdrFileBuffer, g_LdrFileBufSize);
//
//	 adi_osal_ExitCriticalRegion();
//}
//
//
//
//
///*
// * Get the ldr file data.
// * the ldr file data saved in global parameter g_pLdrFileBuffer.
// *
// * return value:
// *            Return_Value > 0, means entire ldr data is received completely, return_value = ldr_len;
// *            Return_Value = 0, means part ldr data is received completely;
// *            Return_value < 0, means errorCode.
// * Notes:
// * 			this function need call many times until the return value > 0.
// */
//VER_UPDATE_ACK_CODE GetLdrFileData(void *pLdrDataIn)
//{
//	VER_UPDATE_ACK_CODE ret = ACK_FRM_OK;
//
//	LDR_DATA *pLdrData = (LDR_DATA *)pLdrDataIn;
//	uint8_t *pLdrPartData;
//	uint8_t  CheckSum;
//	uint32_t DataLen;
//	uint32_t LDR_Index;
//	uint32_t LDR_Len;
//	uint16_t CRC16 ;
//
//	DataLen      = pLdrData->Segment_Len;
//	LDR_Index    = pLdrData->Segment_Index_In_File;
//	pLdrPartData = pLdrData->Segment_Data;
//
//	// get the length of ldr file, and malloc memory to save
//	LDR_Len = pLdrData->LDR_File_Len;
//
//	if( LDR_Index > LDR_Len )
//	{
//		DEBUG_STATEMENT("failed : LDR_Index > LDR_Len !\n\n");
//		ret = NAK_ERROR_INDEX;
//
//		goto ERROR_END;
//	}
//
//
//	if((g_pLdrFileBuffer == NULL) || g_LdrFileBufSize < LDR_Len )  // malloc only in the first time
//	{
//		g_pLdrFileBuffer = (uint8_t *)heap_malloc(HEAPID_LDR, LDR_Len);
//		g_LdrFileBufSize = LDR_Len;
//
//		if(g_pLdrFileBuffer == NULL)
//		{
//			 ret = NAK_ERROR_MEM_ALLOC_FAILED;
//			 DEBUG_STATEMENT("failed: alloc memory for ldr file data buffer!\n\n");
//
//			 goto ERROR_END;
//		}
//	}
//
//	 //copy frame data to update buffer.
//	memcpy( g_pLdrFileBuffer + LDR_Index, pLdrPartData, DataLen);
//
//
//	//last frame,completed receive LDR file data, Check CRC and then erase the flash.
//	if(DataLen == (LDR_Len-LDR_Index))
//	{
//		CRC16 = GetCrc16(0, g_pLdrFileBuffer, LDR_Len);
//
//		if(CRC16 == (pLdrData->LDR_CRC)) //erase flash if CRC correct
//		{
//			ret = ACK_OK;
//			DEBUG_STATEMENT("ok : CRC Pass !\n\n");
//
//			return ret;
//		}
//		else
//		{
//			ret = NAK_ERROR_FILE_CRC;
//			DEBUG_STATEMENT("failed : CRC error !\n\n");
//			goto ERROR_END;
//		}
//
//	}
//
//	ret = ACK_FRM_OK;
//
//ERROR_END:
//
//	return ret;
//}
//
