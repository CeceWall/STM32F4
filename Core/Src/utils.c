//
// Created by weiji on 2021/11/22.
//
#include "utils.h"
#include "stm32f4xx_hal.h"

void delay_us(uint16_t us) {
    TIM1->CNT = us;// set the counter value a 0
    while (TIM1->CNT < us) {
    }// wait for the counter to reach the us input in the parameter
}