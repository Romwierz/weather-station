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
	float temperature;
	float humidity;
} ESP8266_WeatherData_t;

extern volatile bool wifiDataReq;

HAL_StatusTypeDef readWiFiWeatherData();

#endif /* WIFI_MODULE_H_ */
