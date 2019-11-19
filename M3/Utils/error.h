#ifndef ___ERROR_H_M_
#define ___ERROR_H_M_

#include "stm32f1xx_hal.h"

/**
 * Function raise_error allow user to stop processor in case
 * of fatal error. Function should show error code by LEDs.
 */

extern uint8_t error_code;

#define ERROR_UNDEFINED 0x5A

void raise_error(uint8_t code);
void error_handler();

#endif
