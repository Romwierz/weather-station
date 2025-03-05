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
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rtc.h"
#include <stdio.h>
#include "measurement_system.h"
#include "wifi_module.h"
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

/* USER CODE BEGIN PV */
bool wakeup_from_btn = false;
bool exited_from_standby = false;

bool new_local_data = false;
bool new_wifi_data = false;

uint8_t rtc_wakeup_cnt;

uint32_t bkup_register[BKUP_DATA_COUNT];

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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  MX_I2C3_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  my_RTC_Init();
  HAL_TIM_Base_Start(&htim6);

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

  check_wakeup_source();

  if (exited_from_standby) {
	  read_bkup_registers();
  }
  else {
	  measurement_system_init();
  }

  if ((wakeup_from_btn == false) || (__HAL_RTC_WAKEUPTIMER_GET_FLAG(&hrtc, RTC_FLAG_WUTF) == 1U)) {
	  rtc_wakeup_cnt++;
  }
  if (wakeup_from_btn) {
	  HAL_GPIO_EXTI_Callback(B1_Pin);
	  write_bkup_registers();
	  enter_standby_mode();
  }

  measurement_system();

  write_bkup_registers();
  enter_standby_mode();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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
void my_RTC_Init(void)
{
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
}

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

void check_wakeup_source(void)
{
	if (!(__HAL_PWR_GET_FLAG(PWR_FLAG_SB))) {
		return;
	}
	exited_from_standby = true;

	if (__HAL_PWR_GET_FLAG(PWR_FLAG_WUF2)) {
		wakeup_from_btn = true;
	}
	// wakeup from RTC or Power-on reset
	else {
		wakeup_from_btn = false;
	}
}

void read_bkup_registers(void)
{
	for (uint32_t i = 0; i < BKUP_DATA_COUNT; i++) {
		bkup_register[i] = HAL_RTCEx_BKUPRead(&hrtc, i);
	}

	measurement_system_state = bkup_register[MEAS_SYS_STATE];

	dht22_measurement.temperature = (float)bkup_register[DHT22_TEMP];
	dht22_measurement.humidity = (float)bkup_register[DHT22_HUM];

	pressure_local = (float)bkup_register[LPS25HB_PRESS];
	temp_local = (float)bkup_register[LPS25HB_TEMP];
	p0_local = (float)bkup_register[LPS25HB_P0];

	wifiData.id = (uint16_t)bkup_register[WIFI_ID];
	wifiData.temperature = (uint16_t)bkup_register[WIFI_TEMP];
	wifiData.feels_like = (uint16_t)bkup_register[WIFI_FLS_LK];
	wifiData.pressure = (uint16_t)bkup_register[WIFI_PRESS];
	wifiData.humidity = (uint16_t)bkup_register[WIFI_HUM];
	wifiData.visibility = (uint16_t)bkup_register[WIFI_VIS];
	wifiData.wind_speed = (uint16_t)bkup_register[WIFI_WIND_SPD];
	wifiData.wind_deg = (uint16_t)bkup_register[WIFI_WIND_DEG];
	wifiData.clouds = (uint16_t)bkup_register[WIFI_CLOUDS];

	rtc_wakeup_cnt = (uint8_t)bkup_register[RTC_WAKEUP_COUNT];
}

void write_bkup_registers(void)
{
	HAL_PWR_EnableBkUpAccess();
	HAL_RTCEx_BKUPWrite(&hrtc, MEAS_SYS_STATE, (uint32_t)measurement_system_state);

	if (new_local_data) {
		HAL_RTCEx_BKUPWrite(&hrtc, DHT22_TEMP, (uint32_t)dht22_measurement.temperature);
		HAL_RTCEx_BKUPWrite(&hrtc, DHT22_HUM, (uint32_t)dht22_measurement.humidity);
		HAL_RTCEx_BKUPWrite(&hrtc, LPS25HB_PRESS, (uint32_t)pressure_local);
		HAL_RTCEx_BKUPWrite(&hrtc, LPS25HB_TEMP, (uint32_t)temp_local);
		HAL_RTCEx_BKUPWrite(&hrtc, LPS25HB_P0, (uint32_t)p0_local);
	}

	if (new_wifi_data) {
		HAL_RTCEx_BKUPWrite(&hrtc, WIFI_ID, (uint32_t)wifiData.id);
		HAL_RTCEx_BKUPWrite(&hrtc, WIFI_TEMP, (uint32_t)wifiData.temperature);
		HAL_RTCEx_BKUPWrite(&hrtc, WIFI_FLS_LK, (uint32_t)wifiData.feels_like);
		HAL_RTCEx_BKUPWrite(&hrtc, WIFI_PRESS, (uint32_t)wifiData.pressure);
		HAL_RTCEx_BKUPWrite(&hrtc, WIFI_HUM, (uint32_t)wifiData.humidity);
		HAL_RTCEx_BKUPWrite(&hrtc, WIFI_VIS, (uint32_t)wifiData.visibility);
		HAL_RTCEx_BKUPWrite(&hrtc, WIFI_WIND_SPD, (uint32_t)wifiData.wind_speed);
		HAL_RTCEx_BKUPWrite(&hrtc, WIFI_WIND_DEG, (uint32_t)wifiData.wind_deg);
		HAL_RTCEx_BKUPWrite(&hrtc, WIFI_CLOUDS, (uint32_t)wifiData.clouds);
	}

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_WAKEUP_COUNT, (uint32_t)rtc_wakeup_cnt);

	HAL_PWR_DisableBkUpAccess();
}

void rtc_clear_wakeup_flag(void)
{
	do {
		__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
		__ISB();
	} while (__HAL_RTC_WAKEUPTIMER_GET_FLAG(&hrtc, RTC_FLAG_WUTF));
}

void enter_standby_mode(void)
{
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
	HAL_PWR_EnableSEVOnPend();

	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_LOW);

	if (wakeup_from_btn) {
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	}
	else {
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		/* wakeup line event flag must also be cleared
		 * even if wakeup source is rtc,
		 * because this flag can be set even during run mode */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

		/* reset wakeup timer only after rtc wakeup */
		HAL_PWR_EnableBkUpAccess();
		rtc_clear_wakeup_flag();
		HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 5, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
		HAL_PWR_DisableBkUpAccess();
	}

	HAL_DBGMCU_EnableDBGStandbyMode();
	HAL_PWR_EnterSTANDBYMode();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == B1_Pin){
		measurement_system_state++;
		if (measurement_system_state > TURNED_OFF) {
			measurement_system_state = RUNNING_LOCAL_DATA;
		}
		measurement_system_screen_change();
	}
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
