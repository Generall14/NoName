#ifndef _GLOBAL_CLOCK_H_
#define _GLOBAL_CLOCK_H_

#include <stdint.h>

/**
 * FUnctions implementing system time. Base on free run timer
 * (TIM2 in this case).
 *
 * USAGE:
 * 1. Configure timer to run as free run, one tick of this timer
 *    will be base of system time, define it in
 *    GLOBAL_CLOCK_TO_MS_MUL / GLOBAL_CLOCK_TO_US_MUL, define
 *    timer counter in TIMCNT.
 * 2. In timer overflow interrupt call gc_timer_overflow.
 * 3. To get current system time call get_global_clock /
 *    get_global_clock_us / get_global_clock_ms.
 * 4. Call blocking_delay_us / blocking_delay_ms to perform
 *    blocking delay.
 */

#define GLOBAL_CLOCK_TO_MS_MUL .001
#define GLOBAL_CLOCK_TO_US_MUL 1

#define TIMCNT TIM2->CNT

extern uint32_t global_clock;

uint32_t get_global_clock();
uint32_t get_global_clock_us();
uint32_t get_global_clock_ms();

void blocking_delay_us(uint16_t time);
void blocking_delay_ms(uint16_t time);

void gc_timer_overflow();

#endif
