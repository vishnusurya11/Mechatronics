


#ifndef TapeSensors_H
#define TapeSensors_H

#include "AD.h"
#include "IO_Ports.h"
#include <stdio.h>

/*******************************************************************************
 * PUBLIC #DEFINES                                                             *
 ******************************************************************************/


#define WHITE_THRESHOLD 200
#define BLACK_THRESHOLD 50

#define FRONT_LEFT_PIN AD_PORTV5
#define FRONT_MID_PIN AD_PORTV4
#define FRONT_RIGHT_PIN AD_PORTV3
#define BACK_PIN AD_PORTV6

#define SYNCHRONOUS_PORT PORTY
#define SYNCHRONOUS_PIN PIN11

/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

/*returns a 4 bit character. corresponding to the values of each sensor. 1 bit for each of the following 
 * [Back,front left, front mid, front right]
 */
unsigned char CheckTapeSensors(int timerNumber);

int CheckFrontLeftSensor(int timerNumber);

int CheckFrontMidSensor(int timerNumber);

int CheckFrontRightSensor(int timerNumber);

int CheckBackSensor(int timerNumber);

#endif
