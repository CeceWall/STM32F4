/*
 * usb_controller.c
 *
 *  Created on: Nov 14, 2021
 *      Author: weiji
 */

#include <stm32f4xx_hal.h>
#include "usb_controller.h"
#include "utils.h"

#define NOP() asm("NOP")
#define REPEAT5(a) a;a;a;a;a
#define WAIT_CS() REPEAT5(NOP())

static void WriteUSBCommand(int command_code);
static void WriteUSBCommandAndData(int command_code, uint8_t *pData,
		uint16_t dataSize);
static void ReadUSBCommand(uint8_t *pData, uint16_t dataSize);

static CH375_t ch;

CH375_t* ch375_init() {
	CH375_t ch;
}

static inline void SetUsbOutput() {
//	uint32_t tmp = DATA_GPIO->MODER;
//	tmp &= 0xFFFFF555;
	DATA_GPIO->MODER |= 0x00005555;
	DATA_GPIO->OTYPER &= 0xFFFFFF00;
}

static inline void SetUsbInput() {
	DATA_GPIO->MODER &= 0xFFFF0000;
	DATA_GPIO->OTYPER |= 0x000000FF;
}

uint8_t wait_interrupt() {
	uint8_t ret;
	while (INT_GPIO->IDR & INT_PIN) {
		NOP();
	}
	WriteUSBCommand(CMD_GET_STATUS);
	delay_us(2);
	ReadUSBCommand(&ret, 1);
	return ret;
}

void WriteUSBCommandAndData(int command_code, uint8_t *pData, uint16_t dataSize) {
	SetUsbOutput();
	DIRECTIVE_GPIO->BSRR |= SET_AO;
	DIRECTIVE_GPIO->BSRR |= RESET_CS | RESET_WR;

	DATA_GPIO->BSRR = command_code | (~command_code) << 16;
	WAIT_CS();
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
	DIRECTIVE_GPIO->BSRR |= RESET_AO | SET_WR;
	NOP();
	while (n < dataSize) {
		DIRECTIVE_GPIO->BSRR |= RESET_CS | RESET_RD;
		WAIT_CS();
		*(pData + n) = (DATA_GPIO->IDR >> DATA_PIN_OFFSET) & 0xFF;
		WAIT_CS();
		DIRECTIVE_GPIO->BSRR |= SET_CS | SET_RD;
		delay_us(1);
		n += 1;
	}
	DIRECTIVE_GPIO->BSRR |= RESET_AO | RESET_WR;

}

uint8_t check_exists(uint8_t code) {
	uint8_t pData[] = { code };
	WriteUSBCommandAndData(CMD_CHECK_EXIST, pData, 1);
	delay_us(2);
	ReadUSBCommand(pData, 1);
	return pData[0];
}

/* 设置USB通讯速度为低速，如果不设置，默认为全速 */
uint8_t set_freq(USBSpeed speed) {
	uint8_t pData[] = { speed & 0xFF };
	WriteUSBCommandAndData(CMD_SET_USB_SPEED, pData, 1);
}

uint8_t get_freq(void) {
	uint8_t pData[] = { 0x07 };
	WriteUSBCommandAndData(0x0a, pData, 1);
	delay_us(2);
	ReadUSBCommand(pData, 1);
	return pData[0];
}

uint8_t read_id() {
	WriteUSBCommand(CMD_GET_IC_VER);
	delay_us(2);
	uint8_t version;
	ReadUSBCommand(&version, 1);
	return version;
}

/* 读写内部缓冲区 */
uint8_t read_usb_data(uint8_t *data, uint8_t *len) { /* 从CH37X读出数据块 */
	WriteUSBCommand(CMD_RD_USB_DATA); /* 从CH375的端点缓冲区读取接收到的数据 */
	delay_us(2);
	ReadUSBCommand(len, 1);
	ReadUSBCommand(data, *len);
	return 0;
}

uint8_t get_description(USBDescriptionType type) {
	uint8_t pData[64] = { type & 0xFF };
	uint8_t status;
	WriteUSBCommandAndData(CMD_GET_DESCR, pData, 1);
	status = wait_interrupt();
	printf("get description status 0x%02x\n", status);
	if (status == USB_INT_SUCCESS) {
		uint8_t i, len = 64;
		read_usb_data(pData, &len);
		printf("len %d\n", len);
		for (i = 0; i != len; i++)
			printf("%02x ", (unsigned int) pData[i]);
		printf("\n");
	}
}

uint8_t set_addr(uint8_t addr) { /* 设置设备端的USB地址 */
	uint8_t status;
	WriteUSBCommandAndData(CMD_SET_ADDRESS, &addr, 1); /* 设置USB设备端的USB地址 */
	status = wait_interrupt(); /* 等待CH375操作完成 */
	if (status == USB_INT_SUCCESS) { /* 操作成功 */
		delay_us(2);
		WriteUSBCommandAndData(CMD_SET_USB_ADDR, &addr, 1); /* 设置USB主机端的USB地址 */
	}
	return status;
}

uint8_t set_usb_mode(uint8_t mode) {
	uint8_t pData[] = { mode };
	WriteUSBCommandAndData(CMD_SET_MODE, pData, 1);
	delay_us(25);
	ReadUSBCommand(pData, 1);
	return pData[0];
}

void reset_controller() {
	WriteUSBCommand(CMD_RESET);
}

void reset_device() {
	set_usb_mode(0x07);
	HAL_Delay(20);
	set_usb_mode(0x06);
	HAL_Delay(100);
}
