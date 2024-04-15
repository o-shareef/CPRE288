

#include "Timer.h"
#include "lcd.h"
#include "open_interface.h"
#include "movement.h"
#include "cyBot_Scan.h"
#include "cyBot_uart.h"
#include "uart.h"
#include "math.h"

void sendBack(char str[]);
void scan(oi_t *sensor_data, cyBOT_Scan_t *sound);
void startDrive(double finalTurn, double finalDistance, oi_t *sensor_data);
double finalDistance = 0;
double finalTurn = 0;

int main(void)
{
    timer_init();
    lcd_init();
    uart_interrupt_init();

    // cyBot_uart_init();
    // cyBot_uart_init_clean();
    // uart_init();
    cyBOT_init_Scan(0b0011);
    cyBOT_Scan_t* sound = malloc(sizeof(cyBOT_Scan_t));
    //cyBOT_Scan_t *sound = malloc(sizeof(float) + sizeof(int));
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
   // oi_free(sensor_data);
    // cyBOT_SERVO_cal();
    // cyBOT_SERVRO_cal_t* calibrate = malloc(sizeof(cyBOT_SERVRO_cal_t));
    //right_calibration_value = 248500;
    //left_calibration_value =1256500;
    //cyBOT_Scan_t* sound = malloc(sizeof(cyBOT_Scan_t));
    //oi_free(sensor_data);
    // printf("Degrees\tPING Distance\n\r");
    scan(sensor_data,sound);
    //while loop will need a condition

    //               double opp = 1.0 * sin(finalTurn*(3.1415/180))*distToObj;
    //               double adj = 1.0 * cos(finalTurn*(3.1415/180))*distToObj+10;
    //               double actualTurn = atan(opp/adj)*(180/3.1415);

    // maybe use final turn value

    //    while(1){
    //        int ascii = cyBot_getByte();
    //        lcd_printf("%c",ascii);
    //        sendBack(ascii);
    //    }

    oi_free(sensor_data);
    // NOTE: It is recommended that you use only lcd_init(), lcd_printf(), lcd_putc, and lcd_puts from lcd.h.
    // NOTE: For time functions, see Timer.h
}
void startDrive(double finalTurn, double finalDistance, oi_t *sensor_data)
{
    // double actualTurn = atan((sin(finalTurn)-cos(finalTurn)/10)*finalDistance)*(180.0/3.145)*-1;
    // sprintf(str, "%.3lf",actualTurn);


    timer_waitMillis(1000);
    if (finalTurn < 90)
    {

        turn_right(sensor_data, -(90 - finalTurn));
        timer_waitMillis(1000);
        move_forward(sensor_data, finalDistance * 10 - 100);
    }
    if (finalTurn > 91)
    {

        turn_left(sensor_data, finalTurn - 90);
        timer_waitMillis(1000);
        move_forward(sensor_data, finalDistance * 10 - 100);
    }
    // oi_free(sensor_data);
}




void scan(oi_t *sensor_data, cyBOT_Scan_t *sound)
{
    char wait = 'l';
    int turnDegree = 0;

    double distToObj = 0;
    int secondAngle = 0;
    int newObj = 0;
    int objNum = 1;
    int startDegrees = 0;
    int endDegrees = 0;
    double centerDegrees = 0;
    double widthDegrees = 0;
    char str[] = ' ';
    int smallestWidth = 180;
    //double finalTurn = 0;
    double distFound=0;
    double arclength=0;
    double smallestLength = 1000;
    double previousScan = 0;
    double distSum = 0;
    //double finalDistance = 0;

    while (turnDegree < 180)
    {
        int i = 0;
        distSum = 0;
        for(i = 0; i<2; i++){
            cyBOT_Scan(turnDegree, sound);

            if (sound->sound_dist - previousScan < 10 || sound->sound_dist - previousScan > -10) {
                distSum = distSum + sound->sound_dist;
                previousScan = sound->sound_dist;
            }
        }
        distSum=distSum/2;
        // sendBack(turnDegree, sound->sound_dist);
        sprintf(str, "%d \t \t %f \n \r", turnDegree, distSum);
        uart_sendStr(str);

        if (secondAngle == 1) {
            distToObj = distSum;
            secondAngle = 0;
        }

        if (distSum < 70 && newObj == 0 && turnDegree <= 178)
        {
            startDegrees = turnDegree;
            newObj = 1;
            secondAngle = 1;
            
        }


        if ((distSum > 70) && newObj == 1)
        {

            endDegrees = turnDegree;
            centerDegrees = (startDegrees + endDegrees) / 2.0;

            widthDegrees = endDegrees - startDegrees;

            arclength = 2 * 3.1415 * distToObj * (widthDegrees / 360.0);

            // start degrres maybe not reseting properly
            if (widthDegrees > 6)
            {
                sprintf(str, "Object Number: %d\n\r Angle to Object: %.2f\n\r Distance to Object: %.2f\n\r Arc Length: %.2f\n\n\r", objNum, centerDegrees, distToObj, arclength);
                uart_sendStr(str);
                if (arclength < smallestLength)
                {
                    finalDistance = distToObj;
                    smallestLength = arclength;
                    finalTurn = centerDegrees;
                }

                objNum++;
            }
            newObj = 0;
        }
        turnDegree += 2;
    }
    //uart_sendStr(str);
   cyBOT_Scan(finalTurn, sound);
   //oi_free(sensor_data);
   if (finalDistance > 15) {
       startDrive(finalTurn, finalDistance-2, sensor_data);
       scan(sensor_data, sound);
   }
}



void sendBack(char str[])
{
    int i = 0;
    int length = strlen(str);
    for (i = 0; i < length; i++)
    {
        cyBot_sendByte(str[i]);
    }
    // return 0;
    return 0;
}
