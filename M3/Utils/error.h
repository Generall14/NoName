#ifndef ___ERROR_H_M_
#define ___ERROR_H_M_

#include "stm32f1xx_hal.h"

extern uint8_t error_code;

#define ERROR_UNDEFINED 0x55

void raise_error(uint8_t code);
void error_handler();

#endif
