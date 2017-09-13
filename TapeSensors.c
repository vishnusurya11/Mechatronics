/*
 * File:   TapeSensors.c
 * Author: vnandi
 *
 * Created on November 13, 2016, 2:03 PM
 */



#include "xc.h"
#include "AD.h"
#include "BOARD.h"
#include "IO_Ports.h"
#include "LED.h"
#include <stdio.h>
#include "stdlib.h"
#include "pwm.h"
#include "serial.h"
#include "math.h"
#include "ES_Configure.h"

#include "ES_Framework.h"

#include "HSM.h"
#include "FindingTapeSubHSM.h"

#include "TapeSensors.h"

/*returns a 4 bit character. corresponding to the values of each sensor. 1 bit for each of the following 
 * [Back,front left, front mid, front right]
 */
unsigned char CheckTapeSensors(int timerNumber) {
    unsigned char fl = CheckFrontLeftSensor(timerNumber);
    unsigned char fm = CheckFrontMidSensor(timerNumber);
    unsigned char fr = CheckFrontRightSensor(timerNumber);
    unsigned char b = CheckBackSensor(timerNumber);

    return (fr + (fm << 1)+(fl << 2)+(b << 3));

}

//--------------------------------private functions------------------------------------------//
//unsigned char CheckFrontLeftSensor(void) {
//
//    return AD_ReadADPin(FRONT_LEFT_PIN);
//
//}
//
//unsigned char CheckFrontMidSensor(void) {
//
//    return AD_ReadADPin(FRONT_MID_PIN);
//
//}
//
//unsigned char CheckFrontRightSensor(void) {
//
//    return AD_ReadADPin(FRONT_RIGHT_PIN);
//
//}
//
//unsigned char CheckBackSensor(void) {
//
//    return AD_ReadADPin(BACK_PIN);
//
//}

//zero to 123 
int CheckFrontLeftSensor(int timerNumber) {

    
    static int ReturnVal= 0;
    static int sample1;
    static int sample2;
    static int diff;

    static int Threshold = WHITE_THRESHOLD;

    
    switch (timerNumber) {
        case 0:
            IO_PortsSetPortBits(SYNCHRONOUS_PORT, SYNCHRONOUS_PIN); //turn on LED
            //printf("should be pwming");
            break;

        case 1:
            sample1 = AD_ReadADPin(FRONT_LEFT_PIN); //sample 1

            break;
        case 2:
            IO_PortsClearPortBits(SYNCHRONOUS_PORT, SYNCHRONOUS_PIN); //turn off LED 

            break;
        case 3:
            sample2 = AD_ReadADPin(FRONT_LEFT_PIN); //sample 2
            diff = abs(sample2 - sample1);
            //printf("diff1= %d \n", diff);
            if (diff > Threshold) {//then there is a large difference, indicating you are on whiteboard
              //  printf("on white1 \n");
                Threshold = BLACK_THRESHOLD;
                
				ReturnVal = 0;
				
            } else {//if below threshold, then there is small difference indicating you are on black tape
               // printf("on tape1 \n");
                Threshold = WHITE_THRESHOLD;
               
				ReturnVal = 1;
            }

            break;


    }
    return ReturnVal;

}

int CheckFrontMidSensor(int timerNumber) {

    
    static int ReturnVal= 0;
    static int sample1;
    static int sample2;
    static int diff;

    static int Threshold = WHITE_THRESHOLD;

    
    switch (timerNumber) {
        case 0:
            IO_PortsSetPortBits(SYNCHRONOUS_PORT, SYNCHRONOUS_PIN); //turn on LED

            break;

        case 1:
            sample1 = AD_ReadADPin(FRONT_MID_PIN); //sample 1

            break;
        case 2:
            IO_PortsClearPortBits(SYNCHRONOUS_PORT, SYNCHRONOUS_PIN); //turn off LED 

            break;
        case 3:
            sample2 = AD_ReadADPin(FRONT_MID_PIN); //sample 2
            diff = abs(sample2 - sample1);
            //printf("diff2= %d \n", diff);
            if (diff > Threshold) {//then there is a large difference, indicating you are on whiteboard
               // printf("on white1 \n");
                Threshold = BLACK_THRESHOLD;
                
				ReturnVal = 0;
				
            } else {//if below threshold, then there is small difference indicating you are on black tape
             //   printf("on tape1 \n");
                Threshold = WHITE_THRESHOLD;
               
				ReturnVal = 1;
            }

            break;


    }
    return ReturnVal;

}

int CheckFrontRightSensor(int timerNumber) {

    
    static int ReturnVal= 0;
    static int sample1;
    static int sample2;
    static int diff;

    static int Threshold = WHITE_THRESHOLD;

    
    switch (timerNumber) {
        case 0:
            IO_PortsSetPortBits(SYNCHRONOUS_PORT, SYNCHRONOUS_PIN); //turn on LED

            break;

        case 1:
            sample1 = AD_ReadADPin(FRONT_RIGHT_PIN); //sample 1

            break;
        case 2:
            IO_PortsClearPortBits(SYNCHRONOUS_PORT, SYNCHRONOUS_PIN); //turn off LED 

            break;
        case 3:
            sample2 = AD_ReadADPin(FRONT_RIGHT_PIN); //sample 2
            diff = abs(sample2 - sample1);
            //printf("diff3= %d \n", diff);
            if (diff > Threshold) {//then there is a large difference, indicating you are on whiteboard
               // printf("on white1 \n");
                Threshold = BLACK_THRESHOLD;
                
				ReturnVal = 0;
				
            } else {//if below threshold, then there is small difference indicating you are on black tape
              //  printf("on tape1 \n");
                Threshold = WHITE_THRESHOLD;
               
				ReturnVal = 1;
            }

            break;


    }
    return ReturnVal;

}

//int CheckBackSensor(int timerNumber) {
//    static int ReturnVal= 0;
//    static int sample1;
//    static int sample2;
//    static int diff;
//    
//    static int Threshold = WHITE_THRESHOLD;
//
//    
//    switch (timerNumber) {
//        case 0:
//            IO_PortsSetPortBits(SYNCHRONOUS_PORT, SYNCHRONOUS_PIN); //turn on LED
//
//            break;
//
//        case 1:
//            sample1 = AD_ReadADPin(BACK_PIN); //sample 1
//
//            break;
//        case 2:
//            IO_PortsClearPortBits(SYNCHRONOUS_PORT, SYNCHRONOUS_PIN); //turn off LED 
//
//            break;
//        case 3:
//            sample2 = AD_ReadADPin(BACK_PIN); //sample 2
//            diff = abs(sample2 - sample1);
//            printf("*****************************diff4= %d **************************** \n", diff);
//            if (diff > Threshold) {//then there is a large difference, indicating you are on whiteboard
//                printf("on white4 \n");
//                Threshold = BLACK_THRESHOLD;
//                
//				ReturnVal = 0;
//				
//            } else {//if below threshold, then there is small difference indicating you are on black tape
//                printf("on tape4 \n");
//                Threshold = WHITE_THRESHOLD;
//               
//				ReturnVal = 1;
//            }
//
//            break;
//    }
//    return ReturnVal;
//
//}

int CheckBackSensor(int timerNumber) {

    
    static int ReturnVal= 0;
    static int sample1;
    static int sample2;
    static int diff;

    static int Threshold = WHITE_THRESHOLD;

    
    switch (timerNumber) {
        case 0:
            IO_PortsSetPortBits(SYNCHRONOUS_PORT, SYNCHRONOUS_PIN); //turn on LED
            //printf("should be pwming");
            break;

        case 1:
            sample1 = AD_ReadADPin(BACK_PIN); //sample 1

            break;
        case 2:
            IO_PortsClearPortBits(SYNCHRONOUS_PORT, SYNCHRONOUS_PIN); //turn off LED 

            break;
        case 3:
            sample2 = AD_ReadADPin(BACK_PIN); //sample 2
            diff = abs(sample2 - sample1);
            //printf("diff4= %d \n", diff);
            if (diff > Threshold) {//then there is a large difference, indicating you are on whiteboard
                //printf("on white4 \n");
                Threshold = BLACK_THRESHOLD;
                
				ReturnVal = 0;
				
            } else {//if below threshold, then there is small difference indicating you are on black tape
               // printf("on tape4 \n");
                Threshold = WHITE_THRESHOLD;
               
				ReturnVal = 1;
            }

            break;


    }
    return ReturnVal;

}