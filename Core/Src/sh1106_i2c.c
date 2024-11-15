/*
 * sh1106_i2c.c
 *
 *  Created on: Nov 12, 2024
 *      Author: miczu
 */
#include <string.h>
#include "sh1106_i2c.h"

/**
 * Commands
 */
#define SET_COLUMN_ADDR_LOW_BITS 		0x00
#define SET_COLUMN_ADDR_HIGH_BITS 		0x10
#define SET_PUMP_VOLTAGE_VALUE 			0x30
#define SET_DISPLAY_START_LINE			0x40
#define CONTRAST_CONTROL_MODE_SET 		0x81

#define SET_SEGMENT_REMAP 				0xA0
/* 0xa4, Output follows RAM content; 0xa5, Output ignores RAM content */
#define SET_ENTIRE_DISPLAY_OFF_ON 		0xA4
#define SET_NORMAL_REVERSE_DISPLAY 		0xA6
#define MULTIPLEX_RATIO_MODE_SET 		0xA8
#define DC_DC_CONTROL_MODE_SET 			0xAD

#define DISPLAY_OFF 					0xAE
#define DISPLAY_ON 						0xAF
#define SET_PAGE_ADDRESS 				0xB0
#define SET_COMMON_OUTPUT_SCAN_DIR 		0xC0
#define DISPLAY_OFFSET_MODE_SET 		0xD3
#define DISP_DIV_RAT_OSC_FREQ_MODE_SET 	0xD5

#define DISCHAR_PRECHAR_PERIOD_MODE_SET 0xD9
#define COMMON_PADS_HARDWARE_CONF_SET 	0xDA
#define VCOM_DESELECT_LEVEL_MODE_SET 	0xDB

static uint8_t SH1106_DataBuffer[SH1106_WIDTH * SH1106_HEIGHT / 8];

typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
} SH1106_t;

static SH1106_t SH1106;

static const uint8_t SH1106_Init_Buffer[26] = {
		DISPLAY_OFF,
		SET_COLUMN_ADDR_LOW_BITS | 0x00,
		SET_COLUMN_ADDR_HIGH_BITS | 0x00,
		SET_PAGE_ADDRESS | 0x00,
		SET_DISPLAY_START_LINE | 0x00,
		SET_SEGMENT_REMAP | 0x00,	// remap
		COMMON_PADS_HARDWARE_CONF_SET, 0x12,
		DISPLAY_OFFSET_MODE_SET, 0x00,
		SET_COMMON_OUTPUT_SCAN_DIR | 0x00,
		SET_NORMAL_REVERSE_DISPLAY,
		SET_ENTIRE_DISPLAY_OFF_ON,
		CONTRAST_CONTROL_MODE_SET, 0x50,
		MULTIPLEX_RATIO_MODE_SET, 0x3F,	// 1/64 duty
		DISP_DIV_RAT_OSC_FREQ_MODE_SET, 0x80,
		DISCHAR_PRECHAR_PERIOD_MODE_SET, 0xF1,
		VCOM_DESELECT_LEVEL_MODE_SET, 0x40,
		0x8D,	// charge pump
		0x14,
		DISPLAY_ON,
};

void SH1106_Init()
{
	SH1106_Write(SH1106_CMD_STREAM, SH1106_Init_Buffer, sizeof(SH1106_Init_Buffer));

	SH1106_Fill(SH1106_COLOR_BLACK);

	HAL_Delay(5);
}

uint8_t SH1106_Write(SH1106_ControlByte_t ctrl_byte, const uint8_t *pData, uint16_t Size)
{
	if (Size > SH1106_WIDTH) {
		return -1;
	}

	HAL_I2C_Mem_Write(&HI2C_SH1106, SH1106_I2C_ADDR, ctrl_byte, 1,
			(uint8_t*)pData, Size, 100);

	HAL_Delay(5);
	return 0;
}

void SH1106_UpdateScreen()
{
	uint8_t commands[3] = {
		SET_PAGE_ADDRESS,
		SET_COLUMN_ADDR_LOW_BITS,
		SET_COLUMN_ADDR_HIGH_BITS,
	};

	for (uint8_t page = 0; page < 8; page++) {
		commands[0] = SET_PAGE_ADDRESS + page;
		SH1106_Write(SH1106_CMD_STREAM, commands, sizeof(commands));
		SH1106_Write(SH1106_DATA_STREAM, &SH1106_DataBuffer[SH1106_WIDTH * page], SH1106_WIDTH);
	}
}

/* Screen offset not handled */
void SH1106_DrawPixel(uint16_t x, uint16_t y, SH1106_Color_t color)
{
	if (x >= SH1106_WIDTH ||
		y >= SH1106_HEIGHT) {
		return;
	}

	if (color == SH1106_COLOR_WHITE) {
		SH1106_DataBuffer[x + ((y / 8) * SH1106_WIDTH)] |= 1 << (y % 8);
	} else {
		SH1106_DataBuffer[x + ((y / 8) * SH1106_WIDTH)] &= ~(1 << (y % 8));
	}
}

void SH1106_Fill(SH1106_Color_t color)
{
    memset(SH1106_DataBuffer, (color == SH1106_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SH1106_DataBuffer));
}

void SH1106_FillWithLines(void)
{
	memset(SH1106_DataBuffer, 0x0F, sizeof(SH1106_DataBuffer));
}

void SH1106_SetXY(uint16_t x, uint16_t y)
{
	SH1106.CurrentX = x;
	SH1106.CurrentY = y;
}
