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

    oi_setWheels(150, 150); //moves forward at full speed

    //minus value for demo
    while (sum <= distance_mm)
    {

        oi_update(sensor_data);

        sum += sensor_data->distance; //use -> notation since pointer

        lcd_printf("%lf", sum);

        if (sensor_data->bumpRight == 1 || sensor_data->bumpLeft == 1){
            //go around smallest object
            if(smallestObjectCenterAngle >= 90){
                bumper_triggered(sensor_data, 2);
            }
            else if(smallestObjectCenterAngle < 90){
                bumper_triggered(sensor_data, 1);
            }
            sum -= 50;
            //if we hit an object, go around it and go to scan again :)) :DDDDDD -______________________________________________-
            break;
        }

        oi_setWheels(150, 150);
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
        move_backward(sensor_data, 50);
        turn_right(sensor_data, 60);
        move_forward(sensor_data, 220);
        turn_left(sensor_data, 60);
    }
    else if (which_Bumper == 2)
    {
        move_backward(sensor_data, 50);
        turn_left(sensor_data, 60);
        move_forward(sensor_data, 220);
        turn_right(sensor_data, 60);
    }
    else
    {
        move_backward(sensor_data, 50);
        turn_right(sensor_data, 60);
        move_forward(sensor_data, 220);
        turn_left(sensor_data, 60);
    }
}

double move_backward(oi_t *sensor_data, double distance_mm)
{
    double sum = 0; //distance

    oi_setWheels(-200, -200);

    while ((distance_mm * -1) < sum)
    {

        oi_update(sensor_data);

        sum += sensor_data->distance;

      //  lcd_printf("%lf", sum);
    }
    oi_setWheels(0, 0);
    return sum;

}

void turn_right(oi_t *sensor, double degrees)
{

    double sum = 0; //distance member in oi_t struct is type double

    oi_setWheels(-130, 130); //turns right at full speed

    while ((degrees * -1) + 7 < sum)
    { //hard coded a value of 10 to make the turn less obtuse

        oi_update(sensor);

        sum += sensor->angle; //use -> notation since pointer

    //    lcd_printf("%lf", sum);
    }

    oi_setWheels(0, 0); //stop

}

void turn_left(oi_t *sensor, double degrees)
{

    double sum = 0; //distance member in oi_t struct is type double

    oi_setWheels(130, -130); //turns right at full speed

    while (sum < degrees - 7)
    {

        oi_update(sensor);

        sum += sensor->angle; //use -> notation since pointer

    }

    oi_setWheels(0, 0); //stop

}

