/*
 * measurement_system.h
 *
 *  Created on: Nov 22, 2024
 *      Author: miczu
 */

#ifndef MEASUREMENT_SYSTEM_H_
#define MEASUREMENT_SYSTEM_H_

enum System_state {
	RUNNING, TURNED_OFF
};

extern volatile enum System_state measurement_system_state;

void measurement_system_init(void);
void measurement_system(void);
void measurement_system_on(void);
void measurement_system_off(void);

#endif /* MEASUREMENT_SYSTEM_H_ */
