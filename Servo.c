/*
 * File:   Servo.c
 * Author: vnandi
 *
 * Created on November 18, 2016, 11:21 PM
 */


#include "xc.h"
#include <BOARD.h>
#include <stdio.h>
#include "AD.h"
#include "RC_Servo.h"
#include "IO_Ports.h"
#include "LED.h"
#include "pwm.h"
#include "Servo.h"

//void ReleaseBall(void) {
//    int k;
//    for ( k = 0; k < 180000; k++)
//        RC_SetPulseTime(RCPIN, 1100);
//    
//    for ( k = 0; k < 180000; k++)
//        RC_SetPulseTime(RCPIN, 2000);
//}

void RetainBall(void){
    RC_SetPulseTime(RCPIN,2000);
}
void ReleaseBall(void){
    RC_SetPulseTime(RCPIN,1100);
}