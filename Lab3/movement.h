/*
 * movement.h
 *
 *  Created on: Jan 31, 2024
 *      Author: shareef
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_



double move_forward(oi_t *sensor_data,double distance_mm);

double move_backward(oi_t *sensor_data, double distance_mm);

void bumper_triggered(oi_t *sensor_data, int whicih_Bumper);

void turn_right(oi_t *sensor, double degrees);

void turn_left(oi_t *sensor, double degrees);

//int bumper_triggered(oi_t *sensor_data);



#endif /* MOVEMENT_H_ */
