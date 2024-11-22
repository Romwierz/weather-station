/*
 * measurement_system.c
 *
 *  Created on: Nov 22, 2024
 *      Author: miczu
 */

#include "measurement_system.h"
#include "display.h"
#include "dht22.h"
#include "lps25hb.h"

#include "stm32l4xx_hal.h"

volatile enum System_state measurement_system_state = RUNNING;

DHT22_Measurement_t dht22_measurement;
float pressure;
float p0;
float temp;

void measurement_system_init(void)
{
	display_init();

	if (!lps25hb_init()) {
		display_show_error(LPS25HB_ERROR);
		display_update();
		while (1);
	}
	lps25hb_calib(6);
}

void measurement_system(void)
{
	switch (measurement_system_state) {
		case RUNNING:
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
			measurement_system_on();
			break;
		case TURNED_OFF:
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
			measurement_system_off();
			while(measurement_system_state == TURNED_OFF);
			break;
		default:
			measurement_system_state = RUNNING;
			break;
	}
}

void measurement_system_on(void)
{
	dht22_measurement = DHT22_ReadMeasurement();

	display_goto_xy(2, 0);
	display_show_temperature(Font_11x18, dht22_measurement.temperature);

	display_goto_xy(2, 18);
	display_show_humidity(Font_11x18, dht22_measurement.humidity);

	display_update();

	pressure = lps25hb_readPressureMillibars();
	temp = lps25hb_readTemperatureC();
	p0 = lps25hb_pressureToRelativePressure(temp + 273.15f, pressure);

	display_goto_xy(2, 36);
	display_show_pressure(Font_7x10, pressure);

	display_goto_xy(2, 46);
	display_show_relative_pressure(Font_7x10, p0);

	display_update();
}

void measurement_system_off(void)
{
	lps25hb_deinit();
	display_off();
}
