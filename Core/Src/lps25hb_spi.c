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

#define SIGN_BIT_VALUE 			0x800000
#define NON_NEGATIVE_BITS_MSK 	0x7FFFFF

static void writeReg(uint8_t reg, uint8_t value);
static uint8_t readReg(uint8_t reg);
static int32_t readPressureRaw(void);
static int16_t readTemperatureRaw(void);

bool lps25hb_init(void)
{
	if (readReg(WHO_AM_I) != WHO_AM_I_DEFAULT) {
		return false;
	}

	// 0xC0 = 0b11000000
	// PD = 1 (active mode);  ODR[2:0] = 100 (25 Hz pressure & temperature output data rate)
	writeReg(CTRL_REG1, 0xC0);
	// FIFO_EN = 1
	writeReg(CTRL_REG2, 0x40);
	// F_MODE[2:0] = 110 (FIFO Mean mode); WTM_POINT[4:0] = 11111 (32-sample moving average)
	writeReg(FIFO_CTRL, 0xDF);
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

	// convert to negative if raw pressure value is greater than max positive value
	if (pressure_raw > NON_NEGATIVE_BITS_MSK) {
		pressure_raw = (pressure_raw & NON_NEGATIVE_BITS_MSK) - SIGN_BIT_VALUE;
	}

	return pressure_raw;
}

float readPressureMillibars(void)
{
	return (float)readPressureRaw() / 4096.0f;
}

float pressureToRelativePressure(float temp, float p)
{
	const float h = 80; // nasza wysokość n.p.m.

	return p * exp(0.034162608734308f * h / temp);
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

/*
 * @param temp is in Kelvin units
 * @param p0 must be taken from a weather forecast
 */
float pressureToAltitudeMeters(float temp, float p, float p0)
{
  return -29.271769f * temp * log(p / p0);
}

/*
 * @param value is the difference between relative pressure measured without calibration and pressure from
 * weather forecast
 */
void lps25hb_calib(uint16_t value)
{
	// bit resolution is 1/4096 (2^(-12))
	// data length is 8 bit less than measurement (2^(24-16))
	// so value must:
	// 1) multiplied by 4096 (2^12)
	// 2) divided by 256 (2^8) (why???)
	// so overall it must be multiplied by 16 (2^12/2^8)
	value *= 16;
	writeReg(RPDS_L, value);
	writeReg(RPDS_H, value >> 8);
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
