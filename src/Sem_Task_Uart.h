#ifndef   SEM_TASK_UART_H
#define   SEM_TASK_UART_H



#include <drivers\uart\adi_uart.h>
#include <os.h>
#include <cdefBF609.h>
#include "stdio.h"
#include <services/gpio/adi_gpio.h>
#include <services/pwr/adi_pwr.h>
#include <string.h>
#include <services\stdio\adi_stdio.h>

/* UART Device Number to test */
#define UART_DEVICE_NUM     0u
/* Baud rate to be used for char echo */
#define BAUD_RATE           115200u
/*Number of Tx and Rx buffers*/
#define NUM_BUFFERS 2

#define UART0_TX_PORTD_MUX  ((uint16_t) ((uint16_t) 1<<14))
#define UART0_RX_PORTD_MUX  ((uint32_t) ((uint32_t) 1<<16))

#define UART0_TX_PORTD_FER  ((uint16_t) ((uint16_t) 1<<7))
#define UART0_RX_PORTD_FER  ((uint32_t) ((uint32_t) 1<<8))

/* default power settings */
#define MHZTOHZ       (1000000)
#define DF_DEFAULT    (0x0)
#define MSEL_DEFAULT  (0x10)
#define SSEL_DEFAULT  (0x8)
#define CSEL_DEFAULT  (0x4)

#define myCLKIN         (20 * MHZTOHZ)   /* should corresponding to the fact input osc. */
#define myCORE_MAX      (400 * MHZTOHZ)
#define mySYSCLK_MAX    (200 *  MHZTOHZ)
#define mySCLK_MAX      (200 * MHZTOHZ)
#define myVCO_MIN       (72     * MHZTOHZ)


extern OS_SEM g_Sem_Uart_1;
extern OS_SEM g_Sem_Uart_2;
//ADI_UART_HANDLE Uart00_Init(void);
//void Sem_Task_Uart(void* arg);
void test(void* ConsumerArg);
void Sem_Control_Led1(void* arg);
void Sem_Control_Led2(void* arg);


#endif



