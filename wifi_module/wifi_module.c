/*
 * wifi_module.c
 *
 *  Created on: Feb 3, 2025
 *      Author: miczu
 */
#include "wifi_module.h"

#define HUART_ESP8266 huart1

#define SIZE_INFO_LENGTH 3U
#define DIGIT_ZERO_IN_ASCII_OFFSET 48U
#define DIGIT_NINE_IN_ASCII_OFFSET 57U

volatile bool wifiDataReq = false;

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

static void sendRequestToESP8266() {
	HAL_GPIO_WritePin(ESP8266_REQ_GPIO_Port, ESP8266_REQ_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(ESP8266_REQ_GPIO_Port, ESP8266_REQ_Pin, GPIO_PIN_SET);
}

HAL_StatusTypeDef readWiFiWeatherData() {
	char buffer[100] = { 0 };
	char rx_data_size[SIZE_INFO_LENGTH];
	size_t data_size;

	HAL_StatusTypeDef status = HAL_UART_Receive_DMA(&HUART_ESP8266, (uint8_t*)rx_data_size, SIZE_INFO_LENGTH);
	sendRequestToESP8266();
	HAL_DMA_PollForTransfer(&hdma_usart1_rx, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	data_size = convertCharArrayToNumber(rx_data_size);

	status = HAL_UART_Receive_DMA(&HUART_ESP8266, (uint8_t*)buffer, data_size);
	sendRequestToESP8266();
	HAL_DMA_PollForTransfer(&hdma_usart1_rx, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);

	return status;
}
