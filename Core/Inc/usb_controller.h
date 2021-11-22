//
// Created by weiji on 2021/11/22.
//

#ifndef STM32F4_USB_CONTROLLER_H
#define STM32F4_USB_CONTROLLER_H

#include "stdint.h"

#define CMD_RET_SUCCESS 0x51
#define CMD_RET_ABORT 0x5F

#define DIRECTIVE_GPIO GPIOD
#define CS_PIN GPIO_PIN_2
#define WR_PIN GPIO_PIN_0
#define RD_PIN GPIO_PIN_1
#define AO_PIN GPIO_PIN_3

#define INT_GPIO GPIOD
#define INT_PIN GPIO_PIN_5

#define SET_CS CS_PIN
#define RESET_CS (CS_PIN << 16)
#define SET_WR WR_PIN
#define RESET_WR (WR_PIN << 16)
#define RESET_RD (RD_PIN << 16)
#define SET_AO AO_PIN
#define RESET_AO (AO_PIN << 16)

#define DATA_GPIO GPIOC
#define DATA_PIN_OFFSET 0

#define CMD_RESET 0x05
#define CMD_CHECK_EXIST 0x06
#define CMD_SET_MODE 0x15
#define CMD_GET_STATUS 0x22

uint8_t set_usb_mode(uint8_t mode);
void reset_controller();
void reset_device();

#endif //STM32F4_USB_CONTROLLER_H
