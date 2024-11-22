/*
 * display.h
 *
 *  Created on: Nov 22, 2024
 *      Author: miczu
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "sh1106.h"

typedef enum {
    LPS25HB_ERROR
} Error_t;

void display_init(void);
void display_off(void);
void display_update(void);

void display_goto_xy(uint16_t x, uint16_t y);

void display_show_temperature(SH1106_Font_t font, float temperature);
void display_show_humidity(SH1106_Font_t font, float humidity);
void display_show_pressure(SH1106_Font_t font, float pressure);
void display_show_relative_pressure(SH1106_Font_t font, float rel_pressure);

void display_show_error(Error_t error);

#endif /* DISPLAY_H_ */
