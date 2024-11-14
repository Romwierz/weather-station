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

/* 1 extra byte is control byte*/
static uint8_t SH1106_CommandBuffer[(SH1106_WIDTH * SH1106_HEIGHT / 8) + 1];
static uint8_t SH1106_DataBuffer[(SH1106_WIDTH * SH1106_HEIGHT / 8) + 1];

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
//	HAL_I2C_Master_Transmit(&HI2C_SH1106, SH1106_I2C_ADDR,
//			(uint8_t*) &SH1106_Init_Buffer, sizeof(SH1106_Init_Buffer), 100);
	SH1106_WriteCmdStream(SH1106_Init_Buffer, sizeof(SH1106_Init_Buffer));

	HAL_Delay(5);
}

uint8_t SH1106_WriteCmdSingle(const uint8_t *pData)
{
	SH1106_CommandBuffer[0] = SH1106_CMD_SINGLE;
	SH1106_CommandBuffer[1] = *pData;
	HAL_I2C_Master_Transmit(&HI2C_SH1106, SH1106_I2C_ADDR,
					(uint8_t*) &SH1106_CommandBuffer, sizeof(SH1106_CommandBuffer), 100);
	return 0;
}

uint8_t SH1106_WriteCmdStream(const uint8_t *pData, uint16_t Size)
{
	if (Size > sizeof(SH1106_CommandBuffer)) {
		return -1;
	}

	SH1106_CommandBuffer[0] = SH1106_CMD_STREAM;
	for (uint16_t i = 0; i < Size; i++) {
		SH1106_CommandBuffer[i + 1] = *(pData + i);
	}

	HAL_I2C_Master_Transmit(&HI2C_SH1106, SH1106_I2C_ADDR,
			(uint8_t*) &SH1106_CommandBuffer, Size + 1, 100);

//	HAL_I2C_Mem_Write(&HI2C_SH1106, SH1106_I2C_ADDR, SH1106_CMD_STREAM, 1,
//			(uint8_t*) &SH1106_CommandBuffer, Size, 100);

	HAL_Delay(5);
	return 0;
}

uint8_t SH1106_WriteDataSingle(const uint8_t *pData)
{
	SH1106_DataBuffer[0] = SH1106_DATA_SINGLE;
	SH1106_DataBuffer[1] = *pData;
	HAL_I2C_Master_Transmit(&HI2C_SH1106, SH1106_I2C_ADDR,
					(uint8_t*) &SH1106_DataBuffer, sizeof(SH1106_DataBuffer), 100);
	return 0;
}

uint8_t SH1106_WriteDataStream(const uint8_t *pData, uint16_t Size)
{
	if (Size > sizeof(SH1106_DataBuffer)) {
		return -1;
	}

	SH1106_DataBuffer[0] = SH1106_DATA_STREAM;
	for (uint16_t i = 0; i < Size; i++) {
		SH1106_DataBuffer[i + 1] = *(pData + i);
	}

	HAL_I2C_Master_Transmit(&HI2C_SH1106, SH1106_I2C_ADDR,
			(uint8_t*) &SH1106_DataBuffer, Size + 1, 100);

	HAL_Delay(5);
	return 0;
}

void SH1106_Fill(SH1106_Color_t color)
{
	uint8_t commands[3] = {
			SET_PAGE_ADDRESS,
			SET_COLUMN_ADDR_LOW_BITS,
			SET_COLUMN_ADDR_HIGH_BITS,
	};

	uint8_t fill[(SH1106_WIDTH * SH1106_HEIGHT / 16)];

//    memset(SH1106_DataBuffer, SH1106_DATA_STREAM, 1);
    memset(fill, (color == SH1106_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(fill));

    for (uint8_t page = 0; page < 8; page++) {
    	commands[0] = SET_PAGE_ADDRESS + page;
    	SH1106_WriteCmdStream(commands, sizeof(commands));
//    	HAL_I2C_Master_Transmit(&HI2C_SH1106, SH1106_I2C_ADDR, (uint8_t*)(&SH1106_DataBuffer + page),
//    	    					sizeof(SH1106_DataBuffer) / 8, 100);
    	SH1106_WriteDataStream(fill, sizeof(fill));
	}
}
