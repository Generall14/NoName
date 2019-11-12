#include "global_clock.h"

uint32_t global_clock = 0;

uint32_t get_global_clock()
{
	uint32_t base, curr;
	__disable_irq();
	base = global_clock;
	curr = TIM2->CNT;
	__enable_irq();
	return base + curr;
}

uint32_t get_global_clock_ms()
{
	return get_global_clock()*GLOBAL_CLOCK_TO_MS_MUL;
}

uint32_t get_global_clock_us()
{
	return get_global_clock()*GLOBAL_CLOCK_TO_US_MUL;
}
