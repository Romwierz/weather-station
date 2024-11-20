/*
 * sh1106_i2c.h
 *
 *  Created on: Nov 12, 2024
 *      Author: miczu
 */

#ifndef INC_SH1106_I2C_H_
#define INC_SH1106_I2C_H_

#include <stdbool.h>
#include <stdint.h>
#include "i2c.h"
#include "fonts.h"

extern I2C_HandleTypeDef hi2c3;
#define HI2C_SH1106 hi2c3

#define SH1106_I2C_ADDR		0x78
//#define SH1106_I2C_ADDR		0x7A

#define SH1106_WIDTH			132
#define SH1106_HEIGHT			64
#define SH1106_X_AXIS_OFFSET	2

typedef enum {
    SH1106_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
    SH1106_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SH1106_Color_t;

/**
 * SH1106 I2C control byte is:
 * Co DC D5 D4 D3 D2 D1 D0
 */
#define DC_BIT	0x40
#define Co_BIT	0x80

typedef enum {
	SH1106_CMD_STREAM =	0x00,
	SH1106_DATA_STREAM = DC_BIT,
	SH1106_DATA_SINGLE = (Co_BIT | DC_BIT),
	SH1106_CMD_SINGLE = Co_BIT
} SH1106_ControlByte_t;

void SH1106_Init();
uint8_t SH1106_Write(SH1106_ControlByte_t ctrl_byte, const uint8_t *pData, uint16_t Size);
void SH1106_UpdateScreen();
void SH1106_DrawPixel(uint16_t x, uint16_t y, SH1106_Color_t color);
void SH1106_Putc(char ch, SH1106_Font_t font, SH1106_Color_t color);
void SH1106_Puts(char *str, SH1106_Font_t font, SH1106_Color_t color);
void SH1106_PutCustomSymbol(SH1106_CustomSymbol_t symbol, SH1106_Font_t font, SH1106_Color_t color);
void SH1106_Fill(SH1106_Color_t color);
void SH1106_FillWithLines(void);
void SH1106_GotoXY(uint16_t x, uint16_t y);

#endif /* INC_SH1106_I2C_H_ */
