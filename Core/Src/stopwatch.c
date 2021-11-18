/*
 * timer.c
 *
 *  Created on: Nov 14, 2021
 *      Author: weiji
 */

#include "stopwatch.h"

static inline uint32_t stopwatch_cpu_ticks() {
	return DWT->CYCCNT;
}

inline void stopwatch_delay_us(uint32_t microseconds)  {
	STOPWATCH_TIM->CNT = 0;
	uint32_t ticks = 168 * microseconds;
	while (STOPWATCH_TIM->CNT < ticks) {
	}
}
inline void stopwatch_delay(uint32_t nanoseconds) {
	STOPWATCH_TIM->CNT = 0;

	uint32_t ticks = nanoseconds / STOPWATCH_NANOSECONDS_PER_TICK;
	while (STOPWATCH_TIM->CNT < ticks) {
	}
}
