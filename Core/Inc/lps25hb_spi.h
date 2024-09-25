/*
 * lps25hb_spi.h
 *
 *  Created on: Sep 18, 2024
 *      Author: miczu
 */

#ifndef INC_LPS25HB_SPI_H_
#define INC_LPS25HB_SPI_H_

#include <stdint.h>
#include <spi.h>
#include <stdbool.h>

#define HSPI_LPS25HB hspi2

#define RW_BIT 0x80
#define MS_BIT 0x40

/*
 * Register adresses
 */
#define REF_P_XL 		0x08
#define REF_P_L 		0x09
#define REF_P_H 		0x0A

#define WHO_AM_I 		0x0F

#define RES_CONF 		0x10

#define CTRL_REG1 		0x20
#define CTRL_REG2 		0x21
#define CTRL_REG3 		0x22
#define CTRL_REG4 		0x23

#define INTERRUPT_CFG 	0x24
#define INT_SOURCE 		0x25

#define STATUS_REG 		0x27

#define PRESS_OUT_XL 	0x28
#define PRESS_OUT_L 	0x29
#define PRESS_OUT_H 	0x2A

#define TEMP_OUT_L 		0x2B
#define TEMP_OUT_H 		0x2C

#define FIFO_CTRL 		0x2E
#define FIFO_STATUS 	0x2F

#define THS_P_L 		0x30
#define THS_P_H 		0x31

#define RPDS_L 			0x39
#define RPDS_H 			0x3A

#define WHO_AM_I_DEFAULT 0xBD

bool lps25hb_init(void);
void lps25hb_deinit(void);
//static void writeReg(uint8_t reg, uint8_t value);
//static uint8_t readReg(uint8_t reg);
//static int32_t readPressureRaw(void);
float readPressureMillibars(void);
//static int16_t readTemperatureRaw(void);
float readTemperatureC(void);
float readTemperatureK(void);
double pressureToAltitudeMeters(float pressure_mbar, float altimeter_setting_mbar);
void lps25hb_test(void);

#endif /* INC_LPS25HB_SPI_H_ */
