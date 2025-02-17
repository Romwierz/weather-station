/*
 * wifi_module.h
 *
 *  Created on: Feb 3, 2025
 *      Author: miczu
 */

#ifndef WIFI_MODULE_H_
#define WIFI_MODULE_H_

#include <usart.h>
#include <stdbool.h>

typedef struct {
	int8_t temperature;
	int8_t feels_like;
	int8_t humidity;
	uint16_t pressure;
	uint8_t wind_speed;
} WiFi_WeatherData_t;

extern volatile bool wifiDataReq;
extern WiFi_WeatherData_t wifiData;

void esp8266_requestDataSize(void);
void esp8266_requestData(void);
void readWiFiWeatherData(void);
void parseWiFiWeatherData(char* src, WiFi_WeatherData_t* dst);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* WIFI_MODULE_H_ */
