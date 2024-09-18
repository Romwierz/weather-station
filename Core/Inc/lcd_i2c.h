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
#include <i2c.h>

/*
 * PCF8574P <-> HD44780
 *
 * I2C I/O	P7 P6 P5 P4 P3 P2 P1 P0
 * LCD		D7 D6 D5 D4  A EN RW RS
 */

#define HI2C_LCD hi2c3

#define RS_PIN	0x01
#define RW_PIN	0x02
#define EN_PIN	0x04
#define A_PIN	0x08
#define BL_PIN	A_PIN

#define BF_PIN 0x80

#define SET_8BIT_MODE 0x03
#define SET_4BIT_MODE 0x02

#define LINE_1_START	0x00
#define LINE_2_START	0x40

/*
 * Write instructions
 */
#define CLEAR_DISPLAY 			0x01
#define RETURN_HOME 			0x02
#define ENTRY_MODE_SET 			0x04
#define DISPLAY_ON_OFF_CONTROL 	0x08
#define CURSOR_OR_DISPLAY_SHIFT 0x10
#define FUNCTION_SET 			0x20
#define SET_CGRAM_ADDRESS 		0x40
#define SET_DDRAM_ADDRESS 		0x80

/*
 * Bits in write instructions
 */
#define S_BIT	0x01
#define ID_BIT	0x02
#define B_BIT	0x01
#define C_BIT	0x02
#define D_BIT	0x04
#define RL_BIT	0x04
#define SC_BIT	0x08
#define F_BIT 	0x04
#define N_BIT 	0x08
#define DL_BIT 	0x10

/*
 * Commands
 */
#define DISPLAY_ON 	(DISPLAY_ON_OFF_CONTROL | D_BIT)
#define DISPLAY_OFF DISPLAY_ON_OFF_CONTROL

#define SHIFT_DISPLAY_TO_THE_LEFT	(CURSOR_OR_DISPLAY_SHIFT | SC_BIT)
#define SHIFT_DISPLAY_TO_THE_RIGHT	(CURSOR_OR_DISPLAY_SHIFT | SC_BIT | RL_BIT)
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
