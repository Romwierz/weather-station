/*
 * wifi_module.c
 *
 *  Created on: Feb 3, 2025
 *      Author: miczu
 */
#include <string.h>
#include "wifi_module.h"

#define HUART_ESP8266 huart1

#define SIZE_INFO_LENGTH 3U
#define DATA_SIZE_MAX 15U

#define DIGIT_ZERO_IN_ASCII_OFFSET 48U
#define DIGIT_NINE_IN_ASCII_OFFSET 57U

#define REQUEST_RESPONSE_TIME_MAX 2U

volatile bool wifiDataReq = false;
volatile bool dma_tranfer_cplt = false;
static bool data_size_correct = true;

uint32_t tickstart;

// volatile?
char rx_data_size[SIZE_INFO_LENGTH];
char rx_data[DATA_SIZE_MAX];
uint8_t data_size;

static uint8_t myPow(uint8_t base, uint8_t exponent) {
	uint8_t number = 1;

    for (uint8_t i = 0; i < exponent; ++i)
        number *= base;

    return number;
}

static bool isDigit(uint8_t ch) {
	if ((ch >= DIGIT_ZERO_IN_ASCII_OFFSET) && (ch <= DIGIT_NINE_IN_ASCII_OFFSET)) {
		return true;
	} else {
		return false;
	}
}

static uint8_t convertCharArrayToNumber(char* srcArray) {
	uint8_t number = 0;
	uint8_t digit;
	// must know array size (3 bytes)
	for (uint8_t i = 0; i < SIZE_INFO_LENGTH; i++) {
		if (!isDigit(*(srcArray + i))) {
			return 0;
		}
		digit = *(srcArray + i) - DIGIT_ZERO_IN_ASCII_OFFSET;
		// digits are processed from most significant to least
		number += digit * myPow(10, (SIZE_INFO_LENGTH - 1 - i));
	}
	return number;
}

void esp8266_requestDataSize(void) {
	memset(rx_data_size, 0, sizeof(rx_data_size));
	memset(rx_data, 0, sizeof(rx_data));
	data_size = 0;

	__HAL_UART_FLUSH_DRREGISTER(&HUART_ESP8266);
	__HAL_UART_CLEAR_OREFLAG(&HUART_ESP8266);

	HAL_UART_Receive_DMA(&HUART_ESP8266, (uint8_t*) rx_data_size, SIZE_INFO_LENGTH);
	HAL_GPIO_WritePin(ESP8266_REQ_GPIO_Port, ESP8266_REQ_Pin, GPIO_PIN_RESET);

	tickstart = HAL_GetTick();
	while (!dma_tranfer_cplt) {
		if ((HAL_GetTick() - tickstart) > REQUEST_RESPONSE_TIME_MAX) {
			break;
		};
	}
	dma_tranfer_cplt = false;

	data_size = convertCharArrayToNumber(rx_data_size);
	if ((data_size <= 0U) || (data_size > DATA_SIZE_MAX)) {
		data_size_correct = false;
	} else {
		data_size_correct = true;
	}
}

void esp8266_requestData(void) {
	memset(rx_data_size, 0, sizeof(rx_data_size));
	memset(rx_data, 0, sizeof(rx_data));

	__HAL_UART_FLUSH_DRREGISTER(&HUART_ESP8266);
	__HAL_UART_CLEAR_OREFLAG(&HUART_ESP8266);

	if (data_size_correct) {
		HAL_UART_Receive_DMA(&HUART_ESP8266, (uint8_t*) rx_data, data_size);
		HAL_GPIO_WritePin(ESP8266_REQ_GPIO_Port, ESP8266_REQ_Pin, GPIO_PIN_SET);
		tickstart = HAL_GetTick();
		while (!dma_tranfer_cplt) {
			if ((HAL_GetTick() - tickstart) > REQUEST_RESPONSE_TIME_MAX) {
				break;
			};
		}
		dma_tranfer_cplt = false;
	} else {
		HAL_GPIO_WritePin(ESP8266_REQ_GPIO_Port, ESP8266_REQ_Pin, GPIO_PIN_SET);
	}
}

void readWiFiWeatherData() {
	esp8266_requestDataSize();
	esp8266_requestData();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	dma_tranfer_cplt = true;
}
