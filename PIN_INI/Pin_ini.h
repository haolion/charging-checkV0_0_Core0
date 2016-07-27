#ifndef PIN_INI_H
#define PIN_INI_H
#include <services/gpio/adi_gpio.h>
#include "../AD7793_SPI/ad7793_SPI.h"

void ad7793_RDY_Int_Init(void);
static void pinIntCallback(ADI_GPIO_PIN_INTERRUPT ePinInt, uint32_t PinIntData,  void *pCBParam);


#endif
