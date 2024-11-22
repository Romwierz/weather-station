/*
 * display.c
 *
 *  Created on: Nov 22, 2024
 *      Author: miczu
 */

#include <stdio.h>
#include "display.h"

static char SH1106_LineBuf[128];

void display_init(void)
{
	SH1106_Init();
	SH1106_Fill(SH1106_COLOR_BLACK);
	display_update();
}

void display_off(void)
{
	// add sh1106 deinit
	SH1106_Fill(SH1106_COLOR_BLACK);
	display_update();
}

void display_update(void)
{
	SH1106_UpdateScreen();
}

void display_goto_xy(uint16_t x, uint16_t y)
{
	SH1106_GotoXY(x, y);
}

void display_show_temperature(SH1106_Font_t font, float temperature)
{
	SH1106_Puts("Temp: ", font, SH1106_COLOR_WHITE);
	sprintf((char*) SH1106_LineBuf, "%.1f", temperature);
	SH1106_Puts(SH1106_LineBuf, font, SH1106_COLOR_WHITE);
	SH1106_PutCustomSymbol(SH1106_DEGREE_SYMBOL, CustomSymbol_11x18, SH1106_COLOR_WHITE);
}

void display_show_humidity(SH1106_Font_t font, float humidity)
{
	SH1106_Puts("Hum:  ", font, SH1106_COLOR_WHITE);
	sprintf((char*) SH1106_LineBuf, "%.1f%%", humidity);
	SH1106_Puts(SH1106_LineBuf, font, SH1106_COLOR_WHITE);
}

void display_show_pressure(SH1106_Font_t font, float pressure)
{
	SH1106_Puts("Pres: ", font, SH1106_COLOR_WHITE);
	sprintf((char*) SH1106_LineBuf, "%.2fhPa", pressure);
	SH1106_Puts(SH1106_LineBuf, font, SH1106_COLOR_WHITE);
}

void display_show_relative_pressure(SH1106_Font_t font, float rel_pressure)
{
	SH1106_Puts("p0:   ", font, SH1106_COLOR_WHITE);
	sprintf((char*) SH1106_LineBuf, "%.2fhPa", rel_pressure);
	SH1106_Puts(SH1106_LineBuf, font, SH1106_COLOR_WHITE);
}

void display_show_error(Error_t error)
{
	switch (error) {
		case LPS25HB_ERROR:
			display_goto_xy(2, 0);
			SH1106_Puts("Brak czujnika cisnienia! ", Font_7x10, SH1106_COLOR_WHITE);
			break;
		default:
			break;
	}
}

