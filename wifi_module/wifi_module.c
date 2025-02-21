/*
 * wifi_module.c
 *
 *  Created on: Feb 3, 2025
 *      Author: miczu
 */
#include <string.h>
#include "wifi_module.h"

#define HUART_ESP8266 huart1

#define SIZE_INFO_LENGTH				3U
#define DATA_SIZE_MAX					50U
#define ELEMENT_SIZE_MAX				5U

#define COMMA_IN_ASCII_OFFSET			44U
#define NEGATIVE_SIGN_IN_ASCII_OFFSET	45U
#define DIGIT_ZERO_IN_ASCII_OFFSET		48U
#define DIGIT_NINE_IN_ASCII_OFFSET		57U

/* max time between data size request and corresponding response */
#define REQUEST_RESPONSE_TIME_MAX 		3000U

#define __HAL_UART_FLUSH_RDRREGISTER(__HANDLE__)  \
  do{                \
    SET_BIT((__HANDLE__)->Instance->RQR, UART_RXDATA_FLUSH_REQUEST); \
  }  while(0U)

volatile bool wifiDataReq = false;
WiFi_WeatherData_t wifiData;

volatile bool dma_transfer_cplt = false;
static bool data_size_correct = true;

uint32_t tickstart;

// volatile?
char rx_data_size[SIZE_INFO_LENGTH];
char rx_data[DATA_SIZE_MAX];
uint16_t data_size;

static int16_t myPow(int16_t base, int16_t exponent) {
	int16_t number = 1;

    for (int16_t i = 0; i < exponent; ++i)
        number *= base;

    return number;
}

static bool isNegSign(uint8_t ch) {
	if (ch == NEGATIVE_SIGN_IN_ASCII_OFFSET) {
		return true;
	} else {
		return false;
	}
}

static bool isDigit(uint8_t ch) {
	if ((ch >= DIGIT_ZERO_IN_ASCII_OFFSET) && (ch <= DIGIT_NINE_IN_ASCII_OFFSET)) {
		return true;
	} else {
		return false;
	}
}

static int16_t convertCharArrayToNumber(char* srcArray, int16_t arraySize) {
	int16_t number = 0;
	int16_t digit;
	int16_t sign = 1;

	for (int16_t i = 0; i < arraySize; i++) {
		char ch = *(srcArray + i);
		if (isNegSign(ch)) {
			sign = -1;
			continue;
		}
		else if (!isDigit(ch)) {
			return 0;
		}
		digit = (int16_t)(ch) - DIGIT_ZERO_IN_ASCII_OFFSET;
		// digits are processed from most significant to least
		number += sign * digit * myPow(10, (arraySize - 1 - i));
		sign = 1;
	}
	return number;
}

void esp8266_requestDataSize(void) {
	memset(rx_data_size, 0, sizeof(rx_data_size));
	memset(rx_data, 0, sizeof(rx_data));
	data_size = 0;

	__HAL_UART_FLUSH_RDRREGISTER(&HUART_ESP8266);
	__HAL_UART_CLEAR_OREFLAG(&HUART_ESP8266);

	HAL_UART_Receive_DMA(&HUART_ESP8266, (uint8_t*) rx_data_size, SIZE_INFO_LENGTH);
	HAL_GPIO_WritePin(ESP8266_REQ_GPIO_Port, ESP8266_REQ_Pin, GPIO_PIN_RESET);

	tickstart = HAL_GetTick();
	while (!dma_transfer_cplt) {
		if ((HAL_GetTick() - tickstart) > REQUEST_RESPONSE_TIME_MAX) {
			break;
		};
	}
	dma_transfer_cplt = false;

	data_size = (uint16_t)convertCharArrayToNumber(rx_data_size, SIZE_INFO_LENGTH);
	if ((data_size <= 0U) || (data_size > DATA_SIZE_MAX)) {
		data_size_correct = false;
	} else {
		data_size_correct = true;
	}
}

void esp8266_requestData(void) {
	memset(rx_data_size, 0, sizeof(rx_data_size));
	memset(rx_data, 0, sizeof(rx_data));

	__HAL_UART_FLUSH_RDRREGISTER(&HUART_ESP8266);
	__HAL_UART_CLEAR_OREFLAG(&HUART_ESP8266);

	if (data_size_correct) {
		HAL_UART_Receive_DMA(&HUART_ESP8266, (uint8_t*) rx_data, data_size);
		HAL_GPIO_WritePin(ESP8266_REQ_GPIO_Port, ESP8266_REQ_Pin, GPIO_PIN_SET);
		tickstart = HAL_GetTick();
		while (!dma_transfer_cplt) {
			if ((HAL_GetTick() - tickstart) > REQUEST_RESPONSE_TIME_MAX) {
				break;
			};
		}
		dma_transfer_cplt = false;
	} else {
		HAL_GPIO_WritePin(ESP8266_REQ_GPIO_Port, ESP8266_REQ_Pin, GPIO_PIN_SET);
	}
}

void readWiFiWeatherData(void) {
	esp8266_requestDataSize();
	esp8266_requestData();
	parseWiFiWeatherData(rx_data, &wifiData);
}

void parseWiFiWeatherData(char* src, WiFi_WeatherData_t* dst) {
	char tmp[ELEMENT_SIZE_MAX];
	uint16_t i = 0;
	uint16_t src_element_offset = 0;
	uint16_t src_element_size = 0;

	// 1st struct member
	while (*(src + src_element_offset + i) != COMMA_IN_ASCII_OFFSET) {
		if (*(src + src_element_offset + i) == '\0') break;
		tmp[i] = *(src + src_element_offset + i);
		i++;
	}
	src_element_size = i;
	src_element_offset += i + 1;
	i = 0;
	dst->id = (uint16_t)convertCharArrayToNumber(tmp, src_element_size);

	// 2nd struct member
	while (*(src + src_element_offset + i) != COMMA_IN_ASCII_OFFSET) {
		if (*(src + src_element_offset + i) == '\0') break;
		tmp[i] = *(src + src_element_offset + i);
		i++;
	}
	src_element_size = i;
	src_element_offset += i + 1;
	i = 0;
	dst->temperature = (int8_t)convertCharArrayToNumber(tmp, src_element_size);

	// 3rd struct member
	while (*(src + src_element_offset + i) != COMMA_IN_ASCII_OFFSET) {
		if (*(src + src_element_offset + i) == '\0') break;
		tmp[i] = *(src + src_element_offset + i);
		i++;
	}
	src_element_size = i;
	src_element_offset += i + 1;
	i = 0;
	dst->feels_like = (int8_t)convertCharArrayToNumber(tmp, src_element_size);

	// 4th struct member
	while (*(src + src_element_offset + i) != COMMA_IN_ASCII_OFFSET) {
		if (*(src + src_element_offset + i) == '\0') break;
		tmp[i] = *(src + src_element_offset + i);
		i++;
	}
	src_element_size = i;
	src_element_offset += i + 1;
	i = 0;
	dst->pressure = (uint16_t)convertCharArrayToNumber(tmp, src_element_size);

	// 5th struct member
	while (*(src + src_element_offset + i) != COMMA_IN_ASCII_OFFSET) {
		if (*(src + src_element_offset + i) == '\0')
			break;
		tmp[i] = *(src + src_element_offset + i);
		i++;
	}
	src_element_size = i;
	src_element_offset += i + 1;
	i = 0;
	dst->humidity = (uint8_t) convertCharArrayToNumber(tmp, src_element_size);

	// 6th struct member
	while (*(src + src_element_offset + i) != COMMA_IN_ASCII_OFFSET) {
		if (*(src + src_element_offset + i) == '\0')
			break;
		tmp[i] = *(src + src_element_offset + i);
		i++;
	}
	src_element_size = i;
	src_element_offset += i + 1;
	i = 0;
	dst->visibility = (uint16_t) convertCharArrayToNumber(tmp, src_element_size);

	// 7th struct member
	while (*(src + src_element_offset + i) != COMMA_IN_ASCII_OFFSET) {
		if (*(src + src_element_offset + i) == '\0')
			break;
		tmp[i] = *(src + src_element_offset + i);
		i++;
	}
	src_element_size = i;
	src_element_offset += i + 1;
	i = 0;
	dst->wind_speed = (uint8_t) convertCharArrayToNumber(tmp, src_element_size);

	// 8th struct member
	while (*(src + src_element_offset + i) != COMMA_IN_ASCII_OFFSET) {
		if (*(src + src_element_offset + i) == '\0')
			break;
		tmp[i] = *(src + src_element_offset + i);
		i++;
	}
	src_element_size = i;
	src_element_offset += i + 1;
	i = 0;
	dst->wind_deg = (uint16_t) convertCharArrayToNumber(tmp, src_element_size);

	// 9th struct member
	while (*(src + src_element_offset + i) != COMMA_IN_ASCII_OFFSET) {
		if (*(src + src_element_offset + i) == '\0')
			break;
		tmp[i] = *(src + src_element_offset + i);
		i++;
	}
	src_element_size = i;
	src_element_offset += i + 1;
	i = 0;
	dst->clouds = (uint8_t) convertCharArrayToNumber(tmp, src_element_size);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	dma_transfer_cplt = true;
}
