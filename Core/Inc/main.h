/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
enum {
	MEAS_SYS_STATE	= RTC_BKP_DR0,

	DHT22_TEMP		= RTC_BKP_DR1,
	DHT22_HUM		= RTC_BKP_DR2,

	LPS25HB_PRESS	= RTC_BKP_DR3,
	LPS25HB_TEMP	= RTC_BKP_DR4,
	LPS25HB_P0		= RTC_BKP_DR5,

	WIFI_ID			= RTC_BKP_DR6,
	WIFI_TEMP		= RTC_BKP_DR7,
	WIFI_FLS_LK		= RTC_BKP_DR8,
	WIFI_PRESS		= RTC_BKP_DR9,
	WIFI_HUM 		= RTC_BKP_DR10,
	WIFI_VIS		= RTC_BKP_DR11,
	WIFI_WIND_SPD	= RTC_BKP_DR12,
	WIFI_WIND_DEG	= RTC_BKP_DR13,
	WIFI_CLOUDS		= RTC_BKP_DR14,

	BKUP_DATA_COUNT	= RTC_BKP_DR15
};
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void my_RTC_Init(void);
void delay_us(uint32_t us);
void check_reset_source(void);
void read_bkup_registers(void);
void write_bkup_registers(void);
void rtc_clear_wakeup_flag(void);
void enter_standby_mode(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define DHT22_Pin GPIO_PIN_6
#define DHT22_GPIO_Port GPIOA
#define CS_LPS25HB_Pin GPIO_PIN_11
#define CS_LPS25HB_GPIO_Port GPIOB
#define ESP8266_NRST_Pin GPIO_PIN_7
#define ESP8266_NRST_GPIO_Port GPIOC
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define EXT_BTN_Pin GPIO_PIN_12
#define EXT_BTN_GPIO_Port GPIOC
#define EXT_BTN_EXTI_IRQn EXTI15_10_IRQn
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define ESP8266_REQ_Pin GPIO_PIN_8
#define ESP8266_REQ_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define BKUP_DATA_MAX_CNT 32U

extern bool wakeup_from_btn;
extern bool exited_from_standby;

extern bool new_local_data;
extern bool new_wifi_data;

extern uint32_t bkup_register[BKUP_DATA_COUNT];
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
