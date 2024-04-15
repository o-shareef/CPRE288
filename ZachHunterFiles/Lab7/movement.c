/*
 * movement.c
 *
 *  Created on: Jan 31, 2024
 *      Author: shareef
 */

#include "Timer.h"
#include "lcd.h"

#include "open_interface.h"
#include "movement.h"
#include "uart-interrupt.h"

extern float smallestObjectCenterAngle;
extern float smallestObjectDistance;
char balls[50];

double move_forward(oi_t *sensor_data, double distance_mm)
{


    double sum = 0; //distance member in oi_t struct is type double

    oi_setWheels(100, 100); //moves forward at full speed

    //minus value for demo
    while ( sum < distance_mm && sensor_data -> bumpRight == 0 && sensor_data -> bumpLeft == 0) {
                oi_update(sensor_data);
                sum+= sensor_data -> distance;

            }
    if (sum < distance_mm) {
        if (sensor_data-> bumpRight == 1){
            bumper_triggered(sensor_data, 2);
        } else if (sensor_data-> bumpLeft == 1){
            bumper_triggered(sensor_data, 1);
        }

    }

    oi_setWheels(0, 0); //stop
    return sum;
}

void bumper_triggered(oi_t *sensor_data, int which_Bumper)
{

    //1 = left bumper
    //2 = right bumper
    //3 = both bumpers
    if (which_Bumper == 1)
    {
        move_backward(sensor_data);
        turn_right(sensor_data, -60);
        move_forward(sensor_data, 220);
        turn_left(sensor_data, 80);
    }
    else if (which_Bumper == 2)
    {
        move_backward(sensor_data);
        turn_left(sensor_data, 60);
        move_forward(sensor_data, 220);
        turn_right(sensor_data, -80);
    }

}



void move_backward (oi_t *sensor_data){

    double sum = 0;

    oi_setWheels(-100,-100);
        while (sum > -150) {
            oi_update(sensor_data);
            sum+= sensor_data -> distance;

        }
        oi_setWheels(0,0);
}


void turn_right(oi_t *sensor_data, double degrees){
    double sum = 0;
    oi_update(sensor_data);
        oi_setWheels(-50,50);
            while ( sum >= degrees) {
                oi_update(sensor_data);
                sum += sensor_data -> angle;

            }
            oi_setWheels(0,0);


}

void turn_left(oi_t *sensor_data, double degrees) {

    double sum = 0;
    oi_update(sensor_data);
            oi_setWheels(50,-50);
                while ( sum <= degrees*.8) {
                    oi_update(sensor_data);
                    sum+= sensor_data -> angle;

                }
                oi_setWheels(0,0);
    }


