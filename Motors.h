                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   /* 
 * File:   MotorTestHarness.h
 * Author: jdieckma
 *
 * Created on November 6, 2016, 10:43 PM
 */

#ifndef MOTORS_H
#define	MOTORS_H

#include "BOARD.h"
#include "IO_Ports.h"
#include "LED.h"
#include "pwm.h"
#include "serial.h"


#define DIRECTION_PORT PORTY
#define DIRECTION_PIN_A PIN9
#define DIRECTION_PIN_B PIN7

#define ENABLE_PIN_A PWM_PORTZ06
#define ENABLE_PIN_B PWM_PORTY12


void DriveForward(unsigned int speed);

void DriveBackward(unsigned int speed);

void CWTank(unsigned int speed);

void CCWTank(unsigned int speed);

void CWTurn(unsigned int speed);

void CCWTurn(unsigned int speed);

void StopMotors(void);
void CurvyForward(unsigned int lspeed, unsigned int rspeed);
void CurvyBackward(unsigned int lspeed, unsigned int rspeed);

unsigned int AdjustPWM(unsigned int speed);
#endif	/* MOTORS_H */
