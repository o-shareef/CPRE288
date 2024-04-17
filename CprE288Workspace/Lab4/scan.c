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

void cyBot_sendString(char String[])
{
    int i = 0;
    for (i = 0; i < strlen(String); i++)
    {
        cyBot_sendByte(String[i]);
    }

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
 * */
void cyBOT_proxScan(cyBOT_Scan_t *sensors)
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
    float scans[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int loopNumberCounter = 0;
    int scanCycle = 0;
    int previousScanCycle = 0;
    char scanData[50];

    bool scanningObject = false;
    bool objectZeroCompletion = false;
    bool objectOneCompletion = false;
    bool objectTwoCompletion = false;



    char objects[3] = {'F', 'F', 'F'};
    float objectCenterDistances[3];
    float objectStartingAngle[3];
    float objectStartingDistance[3];
    float objectEndingAngle[3];
    float objectRadialWidth[3];

    for (i = 10; i <170 ; i = i + 2)
    {

        //calculate which slot of scans we're in
        scanCycle = loopNumberCounter % 8;
        if (i != 0)
        {
            previousScanCycle = (loopNumberCounter - 1) % 8;
        }

        //do one scan

        scans[scanCycle] = averagePINGScan(i, sensors);
        sprintf(scanData, "%.2f\t%.3f\t%d\n\r", i, scans[scanCycle], scanCycle);
        cyBot_sendString(scanData);

        if (scanCycle == 7)
        {
            cyBot_sendString("-------------------------ONE CYCLE COMPLETE-----------------------\n\r");
        }


        //if i isn't zero, and the separate averaged scans are .800cm apart, and the just scaneed distance isn't really far away, start tracking the object
        if ((i != 0) && (fabs(scans[scanCycle] - scans[previousScanCycle]) <= 1.7) && (scans[scanCycle] < 110) && scanningObject == false)
        {

            //stuff about the object
            if (objects[0] != 'T')
            {
                //we found object 0
                objects[0] = 'T';
                objectStartingAngle[0] = i - 2;
                objectStartingDistance[0] = scans[previousScanCycle];
            }
            else if (objects[1] != 'T')
            {
                //we found object 1
                objects[1] = 'T';
                objectStartingAngle[1] = i - 2;
                objectStartingDistance[1] = scans[previousScanCycle];
            }
            else
            {
                //we found object 2
                objects[2] = 'T';
                objectStartingAngle[2] = i - 2;
                objectStartingDistance[2] = scans[previousScanCycle];
            }
            //we are now scanning an object
            scanningObject = true;
            loopNumberCounter++;
            continue;
        }

        //accounting for space before and after an object

        //problem: each item that is two long is getting reset

        //if an object is initialized but its just space
        if(objects[0] == 'T' && objectZeroCompletion == false && scanningObject == true){
            //object zero could be invalid

            //if the starting angle minus the current angle is greater than or equal to 14 degrees, or if the starting angle minus the current angle
            //is less than or equal to two degrees, we have a phantom object

            //what classifies a small phantom object?
            if(  ((fabs(objectStartingAngle[0] - i) >= 16) || (fabs(objectStartingAngle[0] - i) <= 4)) && ((fabs(objectStartingDistance[0] - scans[scanCycle]) > 1.7))){
                //we know object 0 isn't closed
                objects[0] = 'F';
                objectStartingAngle[0] = 0;
                objectStartingDistance[0] = 0;
                scanningObject = false;
            }
        }
        else if(objects[1] == 'T' && objectOneCompletion == false && scanningObject == true){
                  //object one could be invalid
            if(  ((fabs(objectStartingAngle[1] - i) >= 16) || (fabs(objectStartingAngle[1] - i) <= 4)) && ((fabs(objectStartingDistance[1] - scans[scanCycle]) > 1.7))){
                      //we know object 1 isn't closed
                      objects[1] = 'F';
                      objectStartingAngle[1] = 0;
                      objectStartingDistance[1] = 0;
                      scanningObject = false;
                  }
              }
        else if(objects[2] == 'T' && objectTwoCompletion == false && scanningObject == true){
                  //object two could be invalid
            if(  ((fabs(objectStartingAngle[2] - i) >= 16) || (fabs(objectStartingAngle[2] - i) <= 4)) && ((fabs(objectStartingDistance[2] - scans[scanCycle]) > 1.7))){
                      //we know object two isn't closed
                      objects[2] = 'F';
                      objectStartingAngle[2] = 0;
                      objectStartingDistance[2] = 0;
                      scanningObject = false;
                  }
              }




        //when the average stops being consistent, meaning out next average scan is greater than lets say 2.5cm away
        //stop categorizing and create a struct variable with that object starting point, length, and name

        //if the jump is greater than 2cm, stop scanning the object and calculate information
        if ((fabs(scans[scanCycle] - scans[previousScanCycle]) >= 1.7) && (scanningObject == true))
        {
            //stuff about object 0
            if (objects[0] == 'T' && objectZeroCompletion == false)
            {
               objectEndingAngle[0] = i - 2;
               //CALCULATIONS GO HERE LATER
               cyBot_sendString("We found object 0\n\r");
               scanningObject = false;


               //we are past object now
               objectZeroCompletion = true;
            }
            //stuff about object 1
            else if (objects[1] == 'T' && objectOneCompletion == false)
            {
                objectEndingAngle[1] = i - 2;
                //CALCULATIONS GO HERE LATER
                cyBot_sendString("We found object 1\n\r");
                scanningObject = false;


                //we are past object now
                objectOneCompletion = true;
            }
            //stuff about object 2
            else if (objects[2] == 'T' && objectTwoCompletion == false)
            {
                objectEndingAngle[2] = i - 2;
                //CALCULATIONS GO HERE LATER
                cyBot_sendString("We found object 2\n\r");
                scanningObject = false;


                //we are past object now
                objectTwoCompletion = true;
            }



        }//end of end an object if statement





        loopNumberCounter++;
    }//end of for loop





    //print data for 1st object
    if(objects[0] == 'T') {
    cyBot_sendString("\n\rObject 1: \n\r");
    sprintf(scanData, "Angle: %.3f\t\n\rDistance: %.3f\t\n\rAngular Width: %.3f\t\n\r" , ((objectStartingAngle[0] + objectEndingAngle[0]) / 2), objectStartingDistance[0], (objectEndingAngle[0] - objectStartingAngle[0]) + 2);
    cyBot_sendString(scanData);
    }

    //print data for 2nd object

    cyBot_sendString("\n\rObject 2: \n\r");
    sprintf(scanData, "Angle: %.3f\t\n\rDistance: %.3f\t\n\rAngular Width: %.3f\t\n\r"  , ((objectStartingAngle[1] + objectEndingAngle[1]) / 2), objectStartingDistance[1], (objectEndingAngle[1] - objectStartingAngle[1]) + 2);
    cyBot_sendString(scanData);



/*
    cyBot_sendString("\n\rObject 3: \n\r");
      sprintf(scanData, "Angle: %.3f\t\n\rDistance: %.3f\t\n\rAngular Width: %.3f\t\n\r"  , ((objectStartingAngle[2] + objectEndingAngle[2]) / 2), objectStartingDistance[2], (objectEndingAngle[2] - objectStartingAngle[2]) + 2);
      cyBot_sendString(scanData);
*/

      int angularWidth[3];
      angularWidth[0] = (objectEndingAngle[0] - objectStartingAngle[0]) + 2;
      angularWidth[1] = (objectEndingAngle[1] - objectStartingAngle[1]) + 2;
      angularWidth[2] = (objectEndingAngle[2] - objectStartingAngle[2]) + 2;

     if(angularWidth[0] < angularWidth[1] ){
         cyBOT_Scan(objectStartingAngle[0], sensors);
     }
     else if(angularWidth[1] < angularWidth[0]){
         cyBOT_Scan(objectStartingAngle[1], sensors);
     }



} //end of function

float averagePINGScan(float angle, cyBOT_Scan_t *sensors)
{
float distances[5];
float average = 0;
int i = 0;

for (i = 0; i < 5; i++)
{
    cyBOT_Scan(angle, sensors);
    distances[i] = sensors->sound_dist;
}

for (i = 0; i < 5; i++)
{
    average += distances[i];
}
average /= 5.00;

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

 cyBot_sendString("Degrees\tPING Distance (cm)\n\r");

 for (i = 0; i <= 180; i = i + 2)
 {

 //average scan method, passes angle to function
 currentDistance = averageScan(i, sensors);
 sprintf(scanData, "%d\t%.3f\t\n\r", i, currentDistance);
 cyBot_sendString(scanData);
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
 cyBot_sendString(scanData);
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
 cyBot_sendString("we gotta object baby!!\n\r");
 lcd_printf("we gotta Object!");

 continue;
 }
 }

 oldDistance = currentDistance;
 }

 lcd_printf("%d", objects);
 }
 */
