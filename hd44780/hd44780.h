/*
 * lcd_i2c.h
 *
 *  Created on: Sep 10, 2024
 *      Author: miczu
 */

#ifndef INC_LCD_I2C_H_
#define INC_LCD_I2C_H_

#include <stdbool.h>
#include <stdint.h>
#include "i2c.h"

extern I2C_HandleTypeDef hi2c3;
#define HI2C_LCD hi2c3

#define PCF8574_ADDRESS 	0x4E
#define PCF8574A_ADDRESS 	0x7E

/*
 * PCF8574P <-> HD44780
 *
 * I2C I/O	P7 P6 P5 P4 P3 P2 P1 P0
 * LCD		D7 D6 D5 D4  A EN RW RS
 */

/*
 * 17th character in array is for \0 character
 */
typedef struct {
	uint8_t address;
	char first_line[41];
	char second_line[41];
	bool backlight;
} lcd_display_t;

void lcd_init(lcd_display_t* display);
bool lcd_check_BF(uint8_t address, uint8_t xpin);
void lcd_display(lcd_display_t* display);
void lcd_clear(lcd_display_t* display);
void lcd_off(lcd_display_t* display);

#define DEGREE_SYMBOL	0b11011111

#endif /* INC_LCD_I2C_H_ */
