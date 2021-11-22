/*
 * usb_controller.c
 *
 *  Created on: Nov 14, 2021
 *      Author: weiji
 */

#include <stm32f4xx_hal.h>
#include "usb_controller.h"
#include "utils.h"

static void WriteUSBCommand(int command_code);
static void WriteUSBCommandAndData(int command_code, uint8_t *pData,
		uint16_t dataSize);
static void ReadUSBCommand(uint8_t *pData, uint16_t dataSize);

static inline void SetUsbOutput() {
//	uint32_t tmp = DATA_GPIO->MODER;
//	tmp &= 0xFFFFF555;
	DATA_GPIO->MODER = DATA_GPIO->MODER & 0xFFFFF555;
//	DATA_GPIO->OTYPER &= 0xFFFFF000;
}

static inline void SetUsbInput() {
	DATA_GPIO->MODER &= 0xFFFFF000;
//	DATA_GPIO->OTYPER &= 0xFFFFF000;
}

static uint8_t wait_interrupt() {
	while (INT_GPIO->IDR & INT_PIN) {
	}
	WriteUSBCommand(CMD_GET_STATUS);
	uint8_t ret;
	ReadUSBCommand(&ret, 1);
	return ret;
//	CH375_WR_CMD_PORT(CMD_GET_STATUS);
//	return (CH375_RD_DAT_PORT());
}

void WriteUSBCommandAndData(int command_code, uint8_t *pData, uint16_t dataSize) {
	SetUsbOutput();
	DIRECTIVE_GPIO->BSRR |= SET_AO;
	DIRECTIVE_GPIO->BSRR |= RESET_CS | RESET_WR;

	DATA_GPIO->BSRR = command_code | (~command_code) << 16;
	asm("NOP");
	DIRECTIVE_GPIO->BSRR |= SET_CS | SET_WR;
	DIRECTIVE_GPIO->BSRR |= RESET_AO;
	if (dataSize <= 0) {
		return;
	}
	delay_us(2);
	uint16_t n = 0;
	while (n < dataSize) {
		DIRECTIVE_GPIO->BSRR |= RESET_CS | RESET_WR;
		DATA_GPIO->BSRR = (pData[0] << DATA_PIN_OFFSET)
				| ((~pData[0]) & 0xff) << (16 + DATA_PIN_OFFSET);
		delay_us(1);
		DIRECTIVE_GPIO->BSRR |= SET_CS | SET_WR;
		n += 1;
	}
}

inline void WriteUSBCommand(int command_code) {
	WriteUSBCommandAndData(command_code, NULL, 0);
}

void ReadUSBCommand(uint8_t *pData, uint16_t dataSize) {
	SetUsbInput();
	uint16_t n = 0;
	while (n < dataSize) {
		DIRECTIVE_GPIO->BSRR |= RESET_AO;
		asm("NOP");
		DIRECTIVE_GPIO->BSRR |= RESET_CS | RESET_RD;
		asm("NOP");
		asm("NOP");
		asm("NOP");
		asm("NOP");
		asm("NOP");
		*(pData + n) = (DATA_GPIO->IDR >> DATA_PIN_OFFSET) & 0xFF;
		delay_us(1);
		n += 1;
	}

}

static uint8_t check_exists() {
	uint8_t pData[] = { 0x57 };
	WriteUSBCommandAndData(CMD_CHECK_EXIST, pData, 1);
	ReadUSBCommand(pData, 1);
	return pData[0];
}

uint8_t set_usb_mode(uint8_t mode) {
//	check_exists();
	uint8_t pData[] = { 0x06 };
	WriteUSBCommandAndData(CMD_SET_MODE, pData, 1);
	delay_us(20);
	ReadUSBCommand(pData, 1);
	return pData[0];
//	return wait_interrupt();
}

void reset_controller() {
	WriteUSBCommand(CMD_RESET);
	HAL_Delay(40);
}

void reset_device() {
	set_usb_mode(0x07);
	HAL_Delay(20);
	set_usb_mode(0x06);
}
