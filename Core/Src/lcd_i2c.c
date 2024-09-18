/*
 * lcd_i2c.c
 *
 *  Created on: Sep 10, 2024
 *      Author: miczu
 */

#include "lcd_i2c.h"

static void lcd_write(uint8_t address, uint8_t data, uint8_t xpin);

void lcd_init(lcd_display_t* display)
{
	uint8_t xpin;
	if (display->backlight) xpin = BL_PIN;

	/* Wait for more than 40 ms after VCC rises to 2.7 V */
	HAL_Delay(50);

	lcd_write(display->address, SET_8BIT_MODE, xpin);
	HAL_Delay(5);
	lcd_write(display->address, SET_8BIT_MODE, xpin);
	HAL_Delay(1);
	lcd_write(display->address, SET_8BIT_MODE, xpin);

	/* Set 4-bit mode */
	lcd_write(display->address, SET_4BIT_MODE, xpin);

	/* Specify the number of display lines and character font */
	lcd_write(display->address, FUNCTION_SET | N_BIT, xpin);

	/* Display off */
	lcd_write(display->address, DISPLAY_ON_OFF_CONTROL, xpin);

	/* Display clear */
	lcd_write(display->address, CLEAR_DISPLAY, xpin);
}

static void lcd_write(uint8_t address, uint8_t data, uint8_t xpin)
{
	uint8_t tx_data[4];
	tx_data[0] = (data & 0xF0) | EN_PIN | xpin;
	tx_data[1] = (data & 0xF0) | xpin;
	tx_data[2] = (data << 4) | EN_PIN | xpin;
	tx_data[3] = (data << 4) | xpin;

	HAL_I2C_Master_Transmit(&HI2C_LCD, address, (uint8_t*)&tx_data, 4, 100);

	HAL_Delay(5);
}

bool lcd_check_BF(uint8_t address, uint8_t xpin)
{
	uint8_t tx_data[4];
	uint8_t rx_data[2];
	uint8_t bf_and_ac;

	tx_data[0] = RW_PIN | EN_PIN | xpin;
	tx_data[1] = RW_PIN | xpin;
	tx_data[2] = RW_PIN | EN_PIN | xpin;
	tx_data[3] = RW_PIN | xpin;

	HAL_I2C_Master_Transmit(&HI2C_LCD, address, (uint8_t*) &tx_data, 2, 100);
	HAL_Delay(5);
	HAL_I2C_Master_Receive(&HI2C_LCD, address, (uint8_t*) &rx_data[0], 1, 100);
	HAL_Delay(5);
	HAL_I2C_Master_Transmit(&HI2C_LCD, address, (uint8_t*) &tx_data, 2, 100);
	HAL_Delay(5);
	HAL_I2C_Master_Receive(&HI2C_LCD, address, (uint8_t*) &rx_data[1], 1, 100);
	HAL_Delay(5);
//	HAL_I2C_Master_Transmit(&HI2C_LCD, address, (uint8_t*) &rw_low, 2, 100);
//	HAL_Delay(5);

//	HAL_I2C_Master_Transmit(&HI2C_LCD, address, (uint8_t*) &tx_data[0], 1, 100);
//	HAL_Delay(5);
//	HAL_I2C_Master_Receive(&HI2C_LCD, address, (uint8_t*) &rx_data[0], 1, 100);
//	HAL_Delay(5);
//	HAL_I2C_Master_Transmit(&HI2C_LCD, address, (uint8_t*) &tx_data[1], 1, 100);
//	HAL_Delay(5);
//
//	HAL_I2C_Master_Transmit(&HI2C_LCD, address, (uint8_t*) &tx_data[0], 1, 100);
//	HAL_Delay(5);
//	HAL_I2C_Master_Receive(&HI2C_LCD, address, (uint8_t*) &rx_data[1], 1, 100);
//	HAL_Delay(5);
//	HAL_I2C_Master_Transmit(&HI2C_LCD, address, (uint8_t*) &tx_data[1], 1, 100);
//	HAL_Delay(5);

	bf_and_ac = (rx_data[0] & 0xF0) | (rx_data[1] >> 4);

	return bf_and_ac & BF_PIN;
}

void lcd_display(lcd_display_t* display)
{
	uint8_t xpin, i = 0;
	if (display->backlight) xpin = BL_PIN;

	/* Display clear */
	lcd_write(display->address, CLEAR_DISPLAY, xpin);

	/* Display on */
	lcd_write(display->address, DISPLAY_ON, xpin);

	/* Entry mode set */
	lcd_write(display->address, ENTRY_MODE_SET | ID_BIT, xpin);

	/* Set address */
	lcd_write(display->address, SET_DDRAM_ADDRESS | LINE_1_START, xpin);
//	if (lcd_check_BF(display->address, xpin)) {
//		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
//	}
	while (display->first_line[i])
	{
		lcd_write(display->address, display->first_line[i], xpin | RS_PIN);
		if (i > 15) {
			lcd_write(display->address, SHIFT_DISPLAY_TO_THE_LEFT, xpin);
			HAL_Delay(350);
		}
		if (i > 39)
			break;
		i++;
	}

	i = 0;

	lcd_write(display->address, SET_DDRAM_ADDRESS | LINE_2_START, xpin);
//	if (lcd_check_BF(display->address, xpin)) {
//		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
//	}
	while (display->second_line[i]) {
		lcd_write(display->address, display->second_line[i], xpin | RS_PIN);
		if (i > 15) {
			lcd_write(display->address, SHIFT_DISPLAY_TO_THE_LEFT, xpin);
			HAL_Delay(350);
		}
		if (i > 39)
			break;
		i++;
	}
}

void lcd_clear(lcd_display_t* display)
{
	uint8_t xpin;
	if (display->backlight) xpin = BL_PIN;

	lcd_write(display->address, CLEAR_DISPLAY, xpin);
}

void lcd_off(lcd_display_t* display)
{
	lcd_write(display->address, DISPLAY_OFF, 0);
}
