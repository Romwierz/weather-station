/*
 * lps25hb_spi.c
 *
 *  Created on: Sep 18, 2024
 *      Author: miczu
 */
#include <math.h>
#include "lps25hb_spi.h"
#include "stm32l4xx_hal.h"
#include "gpio.h"

#define SIGN_BIT_VALUE 		0x800000
#define MAX_POSITIVE_VALUE 	0x7FFFFF

static void writeReg(uint8_t reg, uint8_t value);
static uint8_t readReg(uint8_t reg);
static int32_t readPressureRaw(void);
static int16_t readTemperatureRaw(void);

bool lps25hb_init(void)
{
	if (readReg(WHO_AM_I) == WHO_AM_I_DEFAULT) {
		return false;
	}

	// 0xB0 = 0b10110000
	// PD = 1 (active mode);  ODR = 011 (12.5 Hz pressure & temperature output data rate)
	writeReg(CTRL_REG1, 0xB0);
	return true;
}

void lps25hb_deinit(void)
{
	// 0x00 = 0b00000000
	// PD = 0 (power-down mode)
	writeReg(CTRL_REG1, 0x00);
}

static void writeReg(uint8_t reg, uint8_t value)
{
	uint8_t tx_data[2] = {0};
	tx_data[0] = reg;
	tx_data[1] = value;

	HAL_GPIO_WritePin(CS_LPS25HB_GPIO_Port, CS_LPS25HB_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&HSPI_LPS25HB, tx_data, 2, 1000);
	HAL_GPIO_WritePin(CS_LPS25HB_GPIO_Port, CS_LPS25HB_Pin, GPIO_PIN_SET);
}

static uint8_t readReg(uint8_t reg)
{
	uint8_t tx_data [2] = {0};
	uint8_t rx_data [2] = {0};
	tx_data [0] = RW_BIT | reg;

	HAL_GPIO_WritePin(CS_LPS25HB_GPIO_Port, CS_LPS25HB_Pin, GPIO_PIN_RESET);
//	HAL_SPI_Transmit(&HSPI_LPS25HB, &tx_data, 1, 1000);
//	HAL_SPI_Receive(&HSPI_LPS25HB, &rx_data, 1, 1000);
	HAL_SPI_TransmitReceive(&HSPI_LPS25HB, tx_data, rx_data, sizeof(rx_data), 1000);
	HAL_GPIO_WritePin(CS_LPS25HB_GPIO_Port, CS_LPS25HB_Pin, GPIO_PIN_SET);

	return rx_data[1];
}

static int32_t readPressureRaw(void)
{
	uint8_t tx_data[4] = { 0 };
	uint8_t rx_data[4] = { 0 };
	int32_t pressure_raw;
	// MS bit to enable register address auto-increment
	tx_data[0] = RW_BIT | MS_BIT | PRESS_OUT_XL;

	HAL_GPIO_WritePin(CS_LPS25HB_GPIO_Port, CS_LPS25HB_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&HSPI_LPS25HB, tx_data, rx_data, sizeof(rx_data), 1000);
	HAL_GPIO_WritePin(CS_LPS25HB_GPIO_Port, CS_LPS25HB_Pin, GPIO_PIN_SET);

	pressure_raw = (int32_t)(int8_t)rx_data[3] << 16 | (uint16_t)rx_data[2] << 8 | rx_data[1];

	// convert to negative if raw pressure value is greater than max value
	if (pressure_raw > MAX_POSITIVE_VALUE) {
		pressure_raw = (pressure_raw & MAX_POSITIVE_VALUE) - SIGN_BIT_VALUE;
	}

	return pressure_raw;
}

float readPressureMillibars(void)
{
	return (float)readPressureRaw() / 4096.0f;
}

static int16_t readTemperatureRaw(void)
{
	uint8_t tx_data[3] = {0};
	uint8_t rx_data[3] = {0};
	// MS bit to enable register address auto-increment
	tx_data[0] = RW_BIT | MS_BIT | TEMP_OUT_L;

	HAL_GPIO_WritePin(CS_LPS25HB_GPIO_Port, CS_LPS25HB_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&HSPI_LPS25HB, tx_data, rx_data, sizeof(rx_data), 1000);
	HAL_GPIO_WritePin(CS_LPS25HB_GPIO_Port, CS_LPS25HB_Pin, GPIO_PIN_SET);

	return (int16_t)(rx_data[2] << 8 | rx_data[1]);
}

float readTemperatureC(void)
{
	return 42.5f + (float)readTemperatureRaw() / 480.0f;
}

float readTemperatureK(void)
{
	return 273.15f + 42.5f + (float)readTemperatureRaw() / 480.0f;
}

// converts pressure in mbar to altitude in meters, using 1976 US
// Standard Atmosphere model (note that this formula only applies to a
// height of 11 km, or about 36000 ft)
//  If altimeter setting (QNH, barometric pressure adjusted to sea
//  level) is given, this function returns an indicated altitude
//  compensated for actual regional pressure; otherwise, it returns
//  the pressure altitude above the standard pressure level of 1013.25
//  mbar or 29.9213 inHg
double pressureToAltitudeMeters(float pressure_mbar, float altimeter_setting_mbar)
{
  return (1.0 - pow((double)pressure_mbar / (double)altimeter_setting_mbar, 0.190263)) * 44330.8;
}

void lps25hb_test(void)
{
	uint8_t rx_spi = readReg(FIFO_STATUS);
	rx_spi = readReg(WHO_AM_I);
	rx_spi = readReg(CTRL_REG1);
	writeReg(CTRL_REG1, 0x80);
	rx_spi = readReg(CTRL_REG1);
	writeReg(CTRL_REG1, 0x00);
	rx_spi = readReg(CTRL_REG1);
	rx_spi = readReg(FIFO_STATUS);
}
