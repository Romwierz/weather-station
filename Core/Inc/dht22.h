/*
 * dht22.h
 *
 *  Created on: Jul 12, 2024
 *      Author: miczu
 */

#ifndef INC_DHT22_H_
#define INC_DHT22_H_

#include "stm32l4xx.h"
#include <stdbool.h>

typedef struct {
	float temperature;
	float humidity;
} DHT22_Measurement_t;

//static HAL_StatusTypeDef DHT22_Start(void);
//
//static uint8_t DHT22_ReadByte(void);
//
//static bool DHT22_Checksum(uint8_t rh_byte1, uint8_t rh_byte2, uint8_t temp_byte1, uint8_t temp_byte2, uint16_t sum);

DHT22_Measurement_t DHT22_ReadMeasurement(void);

#endif /* INC_DHT22_H_ */
