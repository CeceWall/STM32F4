/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define DIRECTIVE_GPIO GPIOD

#define CS_PIN GPIO_PIN_2
#define CS_GPIO GPIOD
#define WR_PIN GPIO_PIN_0
#define WR_GPIO GPIOD
#define RD_PIN GPIO_PIN_1
#define RD_GPIO GPIOD
#define AO_PIN GPIO_PIN_3
#define AO_GPIO GPIOD

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
#define CMD_CHECK_EXIST 0x06
#define CMD_SET_MODE 0x15
#define CMD_TEST_CONNECT 0x16

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

void delay_us(uint16_t us) {
	__HAL_TIM_SET_COUNTER(&htim1, 0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim1) < us) {
	}  // wait for the counter to reach the us input in the parameter
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

//void WriteToUSBController(int command, uint8_t *pData, uint16_t dataSize) {
//	uint32_t tmp;
//	SetUsbOutput();
//
//	tmp = 0;
//	tmp |= RESET_CS | RESET_WR | SET_RD;
//	tmp |= command ? SET_AO : RESET_AO;
//
//	DIRECTIVE_GPIO->BSRR |= SET_AO;
//	DIRECTIVE_GPIO->BSRR = tmp;
//	delay_us(2);
//
//	tmp = 0;
//	tmp |= pData[0];
//	tmp |= (~pData[0]) << 16;
//	DATA_GPIO->BSRR = tmp;
//
//	delay_us(2);
//	DIRECTIVE_GPIO->BSRR = SET_CS;
//	delay_us(2);
//
//}

void ReadFromUSBController() {
	SetUsbInput();
	delay_us(1);
	DIRECTIVE_GPIO->BSRR = RESET_CS | SET_WR | RESET_AO | RESET_RD;
	delay_us(1);
	uint8_t tmp = DATA_GPIO->IDR & 0xFF;

	DIRECTIVE_GPIO->BSRR = SET_CS;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start(&htim1);

	DATA_GPIO->BSRR = 0xFF;
	uint8_t pData[1];
//	pData[0] = CMD_RESET;
	WriteUSBCommand(CMD_RESET);
	HAL_Delay(40);
	pData[0] = 0x06;
	WriteUSBCommandAndData(CMD_SET_MODE, pData, 1);
	delay_us(2);
	pData[0] = 0x57;
	WriteUSBCommandAndData(CMD_CHECK_EXIST, pData, 1);
	delay_us(2);
	ReadUSBCommand(pData, 1);
//	WriteToUSBController(1, pData, 1);
//	delay_us(2);
//	pData[0] = 0x06;
//	WriteToUSBController(0, pData, 1);

//	pData[0] = 0x57;
//	WriteToUSBController(0, pData, 1);

//	ReadFromUSBController();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		delay_us(60000);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
		delay_us(60000);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
//		HAL_Delay(50);
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 168 - 1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3
                           PC4 PC5 PC6 PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD1 PD2 PD3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PD5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
