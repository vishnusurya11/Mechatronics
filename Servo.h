                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   /* 
 * File:   MotorTestHarness.h
 * Author: jdieckma
 *
 * Created on November 6, 2016, 10:43 PM
 */

#ifndef SERVO_H
#define	SERVO_H

#include "xc.h"
#include <BOARD.h>
#include <stdio.h>
#include "AD.h"
#include "RC_Servo.h"
#include "IO_Ports.h"
#include "LED.h"
#include "pwm.h"




#define RCPIN RC_PORTZ09

void ReleaseBall(void);

void RetainBall(void);
#endif	/* SERVO_H */
