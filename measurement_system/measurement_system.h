/*
 * measurement_system.h
 *
 *  Created on: Nov 22, 2024
 *      Author: miczu
 */

#ifndef MEASUREMENT_SYSTEM_H_
#define MEASUREMENT_SYSTEM_H_

#include "dht22.h"

enum System_state {
	RUNNING_LOCAL_DATA, RUNNING_WIFI_DATA, TURNED_OFF
};

extern volatile enum System_state measurement_system_state;

extern DHT22_Measurement_t dht22_measurement;
extern float pressure_local;
extern float p0_local;
extern float temp_local;

void measurement_system_init(void);
void measurement_system(void);
void measurement_system_screen_change(void);
void measurement_system_off(void);

#endif /* MEASUREMENT_SYSTEM_H_ */
