/*
 * measurement_system.c
 *
 *  Created on: Nov 22, 2024
 *      Author: miczu
 */

#include "main.h"
#include "measurement_system.h"
#include "display.h"
#include "lps25hb.h"
#include "wifi_module.h"

#include "stm32l4xx_hal.h"

volatile enum System_state measurement_system_state = RUNNING_LOCAL_DATA;

DHT22_Measurement_t dht22_measurement;
float pressure_local;
float p0_local;
float temp_local;

void measurement_system_init(void)
{
	display_init();

	while (!lps25hb_init()) {
		display_show_error(LPS25HB_ERROR);
		display_update();
		display_off();
		while (1);
	}
	lps25hb_calib(6);
}

static void perform_local_measurements(void)
{
	dht22_measurement = DHT22_ReadMeasurement();

	pressure_local = lps25hb_readPressureMillibars();
	temp_local = lps25hb_readTemperatureC();
	p0_local = lps25hb_pressureToRelativePressure(temp_local + 273.15f, pressure_local);

	new_local_data = true;
}

static void screen_local_data(void)
{
	display_clear();

	display_goto_xy(2, 0);
	display_puts(Font_7x10, "Local");

	display_goto_xy(2, 10);
	display_show_temperature(Font_7x10, dht22_measurement.temperature);

	display_goto_xy(2, 20);
	display_show_humidity(Font_7x10, dht22_measurement.humidity);

	display_goto_xy(2, 30);
	display_show_pressure(Font_7x10, pressure_local);

	display_goto_xy(2, 40);
	display_show_relative_pressure(Font_7x10, p0_local);

	display_update();
}

static void screen_wifi_data(void)
{
	display_clear();

	display_goto_xy(2, 0);
	display_puts(Font_7x10, "WiFi ");

	display_goto_xy(2, 10);
	display_show_temperature(Font_7x10, (float) wifiData.temperature);

	display_goto_xy(2, 20);
	display_show_humidity(Font_7x10, (float) wifiData.humidity);

	display_goto_xy(2, 30);
	display_show_pressure(Font_7x10, (float) wifiData.pressure);

	display_update();
}

static void screen_off(void)
{
	measurement_system_off();
}

void measurement_system(void)
{
	perform_local_measurements();

		readWiFiWeatherData();

	// update screen if there is new data
	if ((measurement_system_state == RUNNING_LOCAL_DATA) && (new_local_data == true)) {
		screen_local_data();
	}
	else if ((measurement_system_state == RUNNING_WIFI_DATA) && (new_wifi_data == true)) {
		screen_wifi_data();
	}
}

void measurement_system_screen_change(void)
{
		switch (measurement_system_state) {
		case RUNNING_LOCAL_DATA:
			screen_local_data();
			break;
		case RUNNING_WIFI_DATA:
			screen_wifi_data();
			break;
		case TURNED_OFF:
			screen_off();
			break;
		default:
			measurement_system_state = TURNED_OFF;
			break;
	}
}

void measurement_system_off(void)
{
	lps25hb_deinit();
	display_off();
}
