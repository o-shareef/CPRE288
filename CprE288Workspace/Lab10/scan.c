/*
 * cyBot_uart.c
 *
 *  Created on: Feb 5, 2024
 *      Author: shareef
 */

#include "cyBot_uart.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inc/tm4c123gh6pm.h>
#include "Timer.h"
#include "lcd.h"
#include <math.h>
#include "open_interface.h"
#include "movement.h"
#include "cyBot_Scan.h"
#include "scan.h"
#include "uart.h"
#include "uart-interrupt.h"
#include "adc.h"

float smallestObjectCenterAngle = 0;
float smallestObjectDistance = 0;

/**
 *
 * Ways to detect an object
 * -comapring change in distance - dealing with variables there
 * -compare change in distance from a rolling average count
 *
 *
 * New Idea:
 * do a rough 180 degree scan, keeping track of potential objects
 * go back
 * */
void cyBOT_proxScan(cyBOT_Scan_t *sensors, oi_t *sensor_data)
{
    /*
     *
     *do scanning stuff
     *
     * if(scanning stuff is the same or close to it two times in a row)
     *          categorize it as object and keep counting it
     *
     *
     *   when the average stops being consistent, meaning out next average scan is greater than lets say 2.5cm away
     *   stop categorizing and create a struct variable with that object starting point, length, and name
     *
     *    If it passes, we categorize it as an object
     *   and return back to our normal scan operations
     */

    float i = 0;
    int irScans[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    int loopNumberCounter = 0;
    int scanCycle = 0;
    int previousScanCycle = 0;
    int previousPreviousScanCycle = 0;
    char scanData[50];
    int j;
    bool scanningObject = false;
    bool objectCompletion[10] = { false, false, false, false, false, false,
                                  false, false, false, false };
    char objects[10] = { 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F' };
    int objectCenterDistance[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float objectCenterAngle[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float objectStartingAngle[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float objectEndingAngle[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float linearWidth[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float arcLength[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float angularWidth[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float smallestObjectWidth = 0;
    int pastThreeScanAvg = 0;
    int smallestObjectIndex = 0;

    smallestObjectCenterAngle = 0;
    smallestObjectDistance = 0;

    for (i = 0; i <= 180; i = i + 2)
    {

        //calculate which slot of scans we're in
        scanCycle = loopNumberCounter % 8;
        if (i != 0)
        {
            previousScanCycle = (loopNumberCounter - 1) % 8;
        }
        if (i > 1)
        {
            previousPreviousScanCycle = (loopNumberCounter - 2) % 8;
        }

        //do the IR and PING scans and print them
        irScans[scanCycle] = averageRawIRScan(i, sensors);
        sprintf(scanData, "%.2f\t%d\t%d\n\r", i, irScans[scanCycle], scanCycle);
        uart_sendStr(scanData);

        if (scanCycle == 7)
        {
            uart_sendStr(
                    "-------------------------ONE CYCLE COMPLETE-----------------------\n\r");
        }

        //if it isn't our first scan, and we detect an edge of an object using the IR sensor, and we aren't already scanning an object...
        if ((i != 0)
                && (abs(irScans[scanCycle] - irScans[previousScanCycle]) >= 100)
                && scanningObject == false && irScans[scanCycle] > 825)
        {
            for (j = 0; j < 10; j++)
            {
                if (objects[j] != 'T')
                {
                    //populating objects array with objects
                    objects[j] = 'T';
                    objectStartingAngle[j] = i - 2;

                    //now scanning an object
                    scanningObject = true;
                    loopNumberCounter++;
                    break;
                }
            }
            continue;
        }

        //when the back edge of an object is detected, meaning the IR sensor changes in a value greater or equal to 100
        //stop tracking the object

        pastThreeScanAvg = (irScans[scanCycle] + irScans[previousScanCycle]
                + irScans[previousPreviousScanCycle]) / 3;

        //old logic:
        //if the jump is greater than 100, and the ADC differences aren't negative, and the adc value is below 620 (we're scanning space again)  and we are currently scanning an object, stop scanning the object and calculate information
        //  if ((abs(irScans[scanCycle] - irScans[previousScanCycle]) >= 100) && (abs(irScans[previousScanCycle] - irScans[previousPreviousScanCycle]) >= 60) && ((irScans[scanCycle] - irScans[previousScanCycle]) < 0) && (scanningObject == true))

        //if the last three scans are less than 800 ADC value, and we aren't on the front side of the object, and we are currently scanning an object
        if ((pastThreeScanAvg <= 800) && (pastThreeScanAvg > 400)
                && ((irScans[scanCycle] - irScans[previousScanCycle]) < 0)
                && (scanningObject == true))
        {
            for (j = 0; j < 10; j++)
            {

                if (objects[j] == 'T' && objectCompletion[j] == false)
                {
                    objectEndingAngle[j] = i - 6;

                    //find center distance (accurate radius)
                    //LOW+(HIGH-LOW)/2
                    objectCenterAngle[j] = objectStartingAngle[j]
                            + (objectEndingAngle[j] - objectStartingAngle[j])
                                    / 2;
                    objectCenterDistance[j] = averagePINGScan(objectCenterAngle[j], sensors);

                    //send object notification to putty

                    sprintf(scanData, "We found object %d\n\r", j);
                    uart_sendStr(scanData);
                    scanningObject = false;

                    //we are past object now
                    objectCompletion[j] = true;

                    //break from ending object loop
                    break;
                }

            }

        }                   //end of object if statement

        loopNumberCounter++;
    }                   //end of for loop
                        //***END OF SCANNING***

                        //do all calculations for each object detected
    for (j = 0; j < 10; j++)
    {
        angularWidth[j] = (objectEndingAngle[j] - objectStartingAngle[j]);
        arcLength[j] = (angularWidth[j] * (M_PI / 180))
                * objectCenterDistance[j];
        linearWidth[j] = fabs(
                (2 * objectCenterDistance[j])
                        * sin((arcLength[j]) / (2 * objectCenterDistance[j])));

        //print object linear width
        if (objectCompletion[j] == true)
        {
            sprintf(scanData, "\n\rLinear Width of Object %d: %.3lfcm\n\r", j,
                    linearWidth[j]);
            uart_sendStr(scanData);

            sprintf(scanData, "Distance from Object %d: %dcm\n\r", j,
                    objectCenterDistance[j]);
            uart_sendStr(scanData);

            sprintf(scanData, "Center Angle of Object %d: %.3lf\n\r", j,
                    objectCenterAngle[j]);
            uart_sendStr(scanData);

        }
    }

    smallestObjectWidth = linearWidth[0];

    //if object linear width is smallest, set object to 'S' (for smallest)
    for (j = 0; j < 10; j++)
    {
        if (linearWidth[j] <= smallestObjectWidth && linearWidth[j] != 0)
        {
            smallestObjectWidth = linearWidth[j];
            smallestObjectIndex = j;
        }
    }

    //drive to the smallest object

    smallestObjectCenterAngle = objectCenterAngle[smallestObjectIndex];
    smallestObjectDistance = objectCenterDistance[smallestObjectIndex];

    /*if (smallestObjectCenterAngle < 90)
    {
        turn_right(sensor_data, abs(90 - smallestObjectCenterAngle));
    }
    else if (smallestObjectCenterAngle > 90)
    {
        turn_left(sensor_data, abs(90 - smallestObjectCenterAngle));
    }*/


} //end of function

float averagePINGScan(float angle, cyBOT_Scan_t *sensors)
{
    float distances[6];
    float average = 0;
    int i = 0;

    for (i = 0; i < 6; i++)
    {
        cyBOT_Scan(angle, sensors);
        distances[i] = sensors->sound_dist;
    }

    for (i = 0; i < 6; i++)
    {
        average += distances[i];
    }
    average /= 6.00;

    return average;
}

uint16_t averageRawIRScan(float angle, cyBOT_Scan_t *sensors)
{
    uint16_t irVals[2];
    uint16_t average = 0;
    int i = 0;

    for (i = 0; i < 2; i++)
    {
        cyBOT_Scan(angle, sensors);
//        irVals[i] = sensors->IR_raw_val;
          irVals[i] = adc_read();
    }

    for (i = 0; i < 2; i++)
    {
        average += irVals[i];
    }
    average /= 2;

    return average;

}


/**
 *
 * Ways to detect an object
 * -comapring change in distance - dealing with variables there
 * -compare change in distance from a rolling average count
 *
 *
 * New Idea:
 * do a rough 180 degree scan, keeping track of potential objects
 * go back
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *

 void cyBOT_proxScan(cyBOT_Scan_t *sensors)
 {
 int i = 0;
 float oldDistance = 0;
 float currentDistance = 0;
 int objectScan = 0;
 int objects = 0;
 char scanData[25];
 int j = 0;
 bool passedObject = false;

 uart_sendStr("Degrees\tPING Distance (cm)\n\r");

 for (i = 0; i <= 180; i = i + 2)
 {

 //average scan method, passes angle to function
 currentDistance = averageScan(i, sensors);
 sprintf(scanData, "%d\t%.3f\t\n\r", i, currentDistance);
 uart_sendStr(scanData);
 scanData[0] = 0;

 //if we detect a "change in distance less than 4cm, and the old distance wasn't really far away, increment object scan"
 if ((fabs(oldDistance - currentDistance) <= 4) && (oldDistance <= 110))
 {
 objectScan++;
 lcd_printf("%d", objectScan);

 //if the amount of objectScans is greater than 3, we have an object in our sound cone and we need to start tracking it
 if (objectScan >= 8)
 {

 j = i;

 //loop for when an object is in our sound cone
 while (objectScan <= 12)
 { //Detecting Object Width within 5to12 degrees incrementing by two

 //update j to value of i, making it a mock i
 j = j + 2;


 //update scan value
 currentDistance = averageScan(j, sensors);
 sprintf(scanData, "%d\t%.3f\t\n\r", j, currentDistance);
 uart_sendStr(scanData);
 scanData[0] = 0;

 //if we are past the object
 if (fabs(oldDistance - currentDistance) <= 4)
 // {
 //     missedScan = true;
 // }

 //we have object still
 objectScan++;
 lcd_printf("%d", objectScan);

 //update old distance
 oldDistance = currentDistance;

 //if we are past our object
 if (passedObject)
 {
 break;
 }

 }

 i = j;
 j = 0;
 objectScan = 0;
 objects++;

 //let da people know we gotta object baby
 uart_sendStr("we gotta object baby!!\n\r");
 lcd_printf("we gotta Object!");

 continue;
 }
 }

 oldDistance = currentDistance;
 }

 lcd_printf("%d", objects);
 }
 */
