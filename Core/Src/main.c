/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include "dht22.h"
#include "lcd_i2c.h"
#include "lps25hb_spi.h"
#include "sh1106_i2c.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PCF8574_ADDRESS 	0x4E
#define PCF8574A_ADDRESS 	0x7E
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
DHT22_Measurement_t dht22_measurement;
float pressure;
float p0;
float temp;
lcd_display_t display;
volatile enum System_state measurement_system_state;
int blink;

enum System_state {
  RUNNING,
  TURN_OFF,
  IDLE
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  MX_I2C3_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim6);

  SH1106_Init();
  SH1106_Fill(SH1106_COLOR_BLACK);
  SH1106_UpdateScreen();

//  uint8_t command = 0xAE;
//  uint8_t data[128] = {0};
//  SH1106_WRITE_CMD_SINGLE(command);
//  command = 0xAF;
//  HAL_Delay(500);
//  SH1106_WRITE_CMD_SINGLE(command);
//  HAL_Delay(500);
//  SH1106_WRITE_DATA_STREAM(data, sizeof(data));
//  HAL_Delay(500);
//  command = 0xAE;
//  SH1106_WRITE_CMD_SINGLE(command);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  SH1106_Fill(SH1106_COLOR_WHITE);
	  SH1106_UpdateScreen();
	  HAL_Delay(1000);
	  SH1106_Fill(SH1106_COLOR_BLACK);
	  SH1106_UpdateScreen();
	  HAL_Delay(1000);
	  SH1106_DrawPixel(2, 0, SH1106_COLOR_WHITE);
	  SH1106_DrawPixel(3, 0, SH1106_COLOR_WHITE);
	  SH1106_DrawPixel(4, 0, SH1106_COLOR_WHITE);
	  SH1106_DrawPixel(5, 0, SH1106_COLOR_WHITE);
	  SH1106_DrawPixel(6, 0, SH1106_COLOR_WHITE);
	  SH1106_DrawPixel(2, 4, SH1106_COLOR_WHITE);
	  SH1106_DrawPixel(3, 4, SH1106_COLOR_WHITE);
	  SH1106_DrawPixel(4, 4, SH1106_COLOR_WHITE);
	  SH1106_DrawPixel(5, 4, SH1106_COLOR_WHITE);
	  SH1106_DrawPixel(6, 4, SH1106_COLOR_WHITE);
	  SH1106_UpdateScreen();
	  HAL_Delay(1000);
	  SH1106_FillWithLines();
	  SH1106_UpdateScreen();
	  HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void delay_us(uint32_t us)
{
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	while (__HAL_TIM_GET_COUNTER(&htim6) < us);
}

int __io_putchar(int ch)
{
	if (ch == '\n') {
		uint8_t ch2 = '\r';
		HAL_UART_Transmit(&huart2, &ch2, 1, HAL_MAX_DELAY);
	}

	HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, HAL_MAX_DELAY);
	return 1;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == B1_Pin)
		blink = 1;
	else
		blink = 0;
}

void on_start_animation(void)
{
	sprintf((char*) display.first_line, "Enviromental monitor");
	lcd_display(&display);
	HAL_Delay(1000);
	sprintf((char*) display.first_line, "       %c%c", DEGREE_SYMBOL, DEGREE_SYMBOL);
	sprintf((char*) display.second_line, " ");
	lcd_display(&display);
	HAL_Delay(500);
	sprintf((char*) display.first_line, " ");
	sprintf((char*) display.second_line, "       %c%c", DEGREE_SYMBOL, DEGREE_SYMBOL);
	lcd_display(&display);
	HAL_Delay(500);
}

void measurement_system_on(void)
{
	dht22_measurement = DHT22_ReadMeasurement();
	printf("Temp: %.1f\n", dht22_measurement.temperature);
	printf("Hum:  %.1f%%\n", dht22_measurement.humidity);
	sprintf((char*) display.first_line, "Temp: %.1f%cC", dht22_measurement.temperature, DEGREE_SYMBOL);
	sprintf((char*) display.second_line, "Hum:  %.1f%%", dht22_measurement.humidity);
	lcd_display(&display);
	HAL_Delay(1000);

	pressure = readPressureMillibars();
	temp = readTemperatureC();
	sprintf((char*) display.first_line, "Temp: %.1f%cC", temp, DEGREE_SYMBOL);
	sprintf((char*) display.second_line, "Pres: %.2fhPa", pressure);
	lcd_display(&display);
	HAL_Delay(1000);

	p0 = pressureToRelativePressure(temp + 273.15f, pressure);
	sprintf((char*) display.first_line, "Altitude: %.1f", pressureToAltitudeMeters(temp + 273.15f, pressure, 1000));
	sprintf((char*) display.second_line, "p0 = %.2f hPa", p0);
	lcd_display(&display);
	HAL_Delay(1000);
}
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
  while (1)
  {
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
