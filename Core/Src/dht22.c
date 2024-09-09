/*
 * dht22.c
 *
 *  Created on: Jul 12, 2024
 *      Author: miczu
 */

#include <stdio.h>
#include "main.h"
#include "dht22.h"
#include "gpio.h"
#include "tim.h"

static HAL_StatusTypeDef DHT22_Start(void)
{
	HAL_GPIO_WritePin(DHT22_GPIO_Port, DHT22_Pin, GPIO_PIN_RESET);
	delay_us(1100);
	HAL_GPIO_WritePin(DHT22_GPIO_Port, DHT22_Pin, GPIO_PIN_SET);
	delay_us(80);

	if (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) == GPIO_PIN_RESET) {
		delay_us(80);
		if (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) == GPIO_PIN_SET) {
			delay_us(40);
			return HAL_OK;
		} else {
			return HAL_ERROR;
		}

	} else {
		return HAL_ERROR;
	}
}

static uint8_t DHT22_ReadByte(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j++) {
		while (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) != GPIO_PIN_SET);   // wait for the pin to go high
		delay_us(40);   // wait for 40 us

		if (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) == GPIO_PIN_RESET) // if the pin is low
		{
			i &= ~(1 << (7 - j));   // write 0
		} else
			i |= (1 << (7 - j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin)));  // wait for the pin to go low
	}

	return i;
}

static bool DHT22_Checksum(uint8_t rh_byte1, uint8_t rh_byte2, uint8_t temp_byte1, uint8_t temp_byte2, uint16_t sum)
{
	uint16_t calculated_sum = rh_byte1 + rh_byte2 + temp_byte1 + temp_byte2;
	if ((calculated_sum & 0xf) == sum) {
		return true;
	}
	return false;
}

DHT22_Measurement_t DHT22_ReadMeasurement(void)
{
	uint8_t RH_Byte1, RH_Byte2, TEMP_Byte1, TEMP_Byte2;
	uint8_t SUM;
	uint16_t RH, TEMP;

	DHT22_Measurement_t dht22_measurement;
	dht22_measurement.temperature = 0.0f;
	dht22_measurement.humidity = 0.0f;

	if (DHT22_Start() == HAL_ERROR) {
		printf("Initialization failed!\n\r");
		return dht22_measurement;
	}
	RH_Byte1 = DHT22_ReadByte();
	RH_Byte2 = DHT22_ReadByte();
	TEMP_Byte1 = DHT22_ReadByte();
	TEMP_Byte2 = DHT22_ReadByte();
	SUM = DHT22_ReadByte();

	TEMP = ((TEMP_Byte1 << 8) | TEMP_Byte2);
	RH = ((RH_Byte1 << 8) | RH_Byte2);

	dht22_measurement.temperature = (float) (TEMP / 10.0f);
	dht22_measurement.humidity = (float) (RH / 10.0f);

	return dht22_measurement;
}
