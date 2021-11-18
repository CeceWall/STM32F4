/*
 * timer.h
 *
 *  Created on: Nov 14, 2021
 *      Author: weiji
 */

#ifndef INC_STOPWATCH_H_
#define INC_STOPWATCH_H_

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

//#include "stm32f4xx_hal_rcc.h"
//#include "stm32f4xx_hal_tim.h"
#define STOPWATCH_TIM TIM1
#define STOPWATCH_FREQ 168000000
#define STOPWATCH_NANOSECONDS_PER_TICK (1000 / (STOPWATCH_FREQ / 1000000))

//#define DEMCR (*((volatile uint32_t*)0xE000EDFC))
//#define DEMCR_TRACENA 0x01000000
//#define GET_CPU_ClkFreq HAL_RCC_GetSysClockFreq
//#define DWT_CYCCNT (*((volatile uint32_t*)0xE0001004))
//#define DWT_CTL (*((volatile uint32_t*)0xE0001000))
//#define
//#define CYCCNTENA 0x01

//void stopwatch_reset();
void stopwatch_delay_us(uint32_t microseconds);
void stopwatch_delay(uint32_t delay);

#endif /* INC_STOPWATCH_H_ */
