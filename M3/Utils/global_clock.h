#ifndef _GLOBAL_CLOCK_H_
#define _GLOBAL_CLOCK_H_

#include "stm32f1xx_hal.h"

#define GLOBAL_CLOCK_TO_MS_MUL .001
#define GLOBAL_CLOCK_TO_US_MUL 1

extern uint32_t global_clock;

uint32_t get_global_clock();
uint32_t get_global_clock_us();
uint32_t get_global_clock_ms();

#endif
