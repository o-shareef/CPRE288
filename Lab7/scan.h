/*
 * scan.h
 *
 *  Created on: Feb 12, 2024
 *      Author: shareef
 */

#ifndef SCAN_H_
#define SCAN_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inc/tm4c123gh6pm.h>
#include "cyBot_Scan.h"
#include "movement.h"

extern float smallestObjectCenterAngle;
extern float smallestObjectDistance;

/*
typedef struct{
    int startingAngle;
    int endingAngle;
    int centerAngle;
    float centerDistance;
    double linearWidth;
    double radialWidth;
}potentialObject;
*/

void cyBot_sendString(char String[]);

void cyBOT_proxScan(cyBOT_Scan_t* sensors, oi_t *sensor_data);

float averagePINGScan(float angle, cyBOT_Scan_t *sensors);

int averageIRScan(float angle, cyBOT_Scan_t *sensors);


#endif /* SCAN_H_ */
