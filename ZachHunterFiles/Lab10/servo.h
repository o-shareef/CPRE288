/*
 * servo.h
 *
 *  Created on: Apr 3, 2024
 *      Author: merwi002
 */
#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include <stdlib.h>

#include "lcd.h"
#include "timer.h"
#ifndef SERVO_H_
#define SERVO_H_


void servo_init(void);
void servo_move(uint16_t degrees);


#endif /* SERVO_H_ */
