#include <stdint.h>
#include "global_clock.h"

uint32_t fake_global_clock_us = 0;

uint32_t get_global_clock()
{
	return fake_global_clock_us/GLOBAL_CLOCK_TO_US_MUL;
}

uint32_t get_global_clock_us()
{
	return fake_global_clock_us;
}

uint32_t get_global_clock_ms()
{
	return fake_global_clock_us*GLOBAL_CLOCK_TO_MS_MUL;
}
