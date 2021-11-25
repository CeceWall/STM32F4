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
#define SET_RD RD_PIN
#define RESET_RD (RD_PIN << 16)
#define SET_AO AO_PIN
#define RESET_AO (AO_PIN << 16)

#define DATA_GPIO GPIOC
#define DATA_PIN_OFFSET 0

#define CMD_RESET 0x05
#define CMD_SET_USB_SPEED 0x04
#define CMD_CHECK_EXIST 0x06
#define CMD_SET_MODE 0x15
#define CMD_GET_STATUS 0x22
#define CMD_RD_USB_DATA 0x28
#define CMD_GET_IC_VER 0x01
#define CMD_GET_DESCR 0x46
#define CMD_SET_ADDRESS 0x45
#define CMD_SET_USB_ADDR 0x13

#define USB_INT_SUCCESS 0x14 // 事务或者传输操作成功
#define USB_INT_CONNECT 0x15 // 检测到 USB 设备连接事件
#define USB_INT_DISCONNECT 0x16 // 检测到 USB 设备断开事件
#define USB_INT_BUF_OVER USB 0x17 //传输的数据有误或者数据太多缓冲区溢出

typedef enum {
	READY, INITIAL
} CH375Status;

typedef struct {

} CH375_t;

CH375_t* ch375_init();

uint8_t ch375_reset();

typedef enum {
	DESCRIPTOR_DEVICE = 1, DESCRIPTOR_CONFIGURATION = 2,
} USBDescriptionType;
typedef enum {
	USB_SPEED_LOW = 0x02, USB_SPEED_FULL = 0,
} USBSpeed;

uint8_t read_id();
uint8_t check_exists(uint8_t code);
uint8_t set_usb_mode(uint8_t mode);
uint8_t wait_interrupt();
uint8_t set_freq(USBSpeed speed);
uint8_t get_freq(void);
uint8_t set_addr(uint8_t addr);
uint8_t get_description(USBDescriptionType type);
uint8_t read_usb_data(uint8_t *data, uint8_t *len);

void reset_controller();
void reset_device();

#endif //STM32F4_USB_CONTROLLER_H
