/*
 * fonts.h
 *
 *  Created on: Nov 14, 2024
 *      Author: miczu
 */

#ifndef INC_FONTS_H_
#define INC_FONTS_H_

#include <stdint.h>

#define ASCII_SPACE_OFFSET	32
#define ASCII_TILDE_OFFSET	126

typedef struct {
    uint16_t width;
    uint16_t height;
    const uint16_t *const data;
} SH1106_Font_t;

extern const SH1106_Font_t Font_7x10;
extern const SH1106_Font_t Font_11x18;

#endif /* INC_FONTS_H_ */
