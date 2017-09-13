/*
 * File:   MotorTestHarness.c
 * Author: jdieckma
 *
 * Created on November 6, 2016, 12:53 PM
 */

#include "xc.h"
#include "AD.h"
#include "BOARD.h"
#include "IO_Ports.h"
#include "LED.h"
#include "pwm.h"
#include "serial.h"
#include "stdio.h"
#include "math.h"
#include "Motors.h"


//------------------------function declarations------------------------------

//main for testing motors 

//int main(void) {
//    PWM_Init();
//    BOARD_Init();
//    LED_Init();
//    SERIAL_Init();
//    unsigned int wait;
//
//    PWM_AddPins(ENABLE_PIN_A); //Enable A
//    PWM_AddPins(ENABLE_PIN_B); //Enable B    
//    IO_PortsSetPortOutputs(DIRECTION_PORT, DIRECTION_PIN_B); //Direction B
//    IO_PortsSetPortOutputs(DIRECTION_PORT, DIRECTION_PIN_A); //Direction A
//
//    int input = 700;
//    int scaled_input;
//    int i;
//    int j;
//    PWM_SetFrequency(MIN_PWM_FREQ);
//
//    LED_AddBanks(LED_BANK1);
//    LED_AddBanks(LED_BANK2);
//    LED_AddBanks(LED_BANK3);
//    LED_OffBank(LED_BANK1, 0x0F);
//    LED_OffBank(LED_BANK2, 0x0F);
//    LED_OffBank(LED_BANK3, 0x0F);
//
//    while (1) {
//        for (wait = 0; wait <= 18300; wait++)
//
//            DriveForward();
//        printf("Drive forward ");
//        for (wait = 0; wait <= 18300; wait++)
//
//
//            DriveBackward();
//        printf("Drive backward ");
//        for (wait = 0; wait <= 18300; wait++)
//
//
//            CWTank();
//        printf("cw tank ");
//        for (wait = 0; wait <= 18300; wait++)
//
//
//            CCWTank();
//        printf("ccw tank ");
//        for (wait = 0; wait <= 18300; wait++)
//
//            scaled_input = input / 85;
//        int pattern = pow(2, scaled_input) - 1;
//        LED_SetBank(LED_BANK1, (pattern & 0x00f));
//        LED_SetBank(LED_BANK2, (pattern & 0xf0) >> 4);
//        LED_SetBank(LED_BANK3, (pattern & 0xf00) >> 8);
//
//    }
//}

void DriveBackward(unsigned int speed) {
    unsigned int newDuty = AdjustPWM(speed);
    PWM_SetDutyCycle(ENABLE_PIN_A, newDuty);
    PWM_SetDutyCycle(ENABLE_PIN_B, newDuty);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_A);
    IO_PortsSetPortBits(DIRECTION_PORT, DIRECTION_PIN_B);
}

void ShimmyLeft(unsigned int speed) {
    unsigned int newDuty = AdjustPWM(speed);
    PWM_SetDutyCycle(ENABLE_PIN_A, newDuty);
    PWM_SetDutyCycle(ENABLE_PIN_B, newDuty - 70);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_A);
    IO_PortsSetPortBits(DIRECTION_PORT, DIRECTION_PIN_B);
}

void ShimmyRight(unsigned int speed) {
    unsigned int newDuty = AdjustPWM(speed);
    PWM_SetDutyCycle(ENABLE_PIN_A, newDuty - 70);
    PWM_SetDutyCycle(ENABLE_PIN_B, newDuty);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_A);
    IO_PortsSetPortBits(DIRECTION_PORT, DIRECTION_PIN_B);
}

void DriveForward(unsigned int speed) {
    unsigned int newDuty = AdjustPWM(speed);
    PWM_SetDutyCycle(ENABLE_PIN_A, newDuty);
    PWM_SetDutyCycle(ENABLE_PIN_B, newDuty);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_B);
    IO_PortsSetPortBits(DIRECTION_PORT, DIRECTION_PIN_A);

}

void CCWTank(unsigned int speed) {
    unsigned int newDuty = AdjustPWM(speed);
    PWM_SetDutyCycle(ENABLE_PIN_A, newDuty);
    PWM_SetDutyCycle(ENABLE_PIN_B, newDuty);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_A);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_B);

}

void CWTank(unsigned int speed) {
    unsigned int newDuty = AdjustPWM(speed);
    PWM_SetDutyCycle(ENABLE_PIN_A, newDuty);
    PWM_SetDutyCycle(ENABLE_PIN_B, newDuty);
    IO_PortsSetPortBits(DIRECTION_PORT, DIRECTION_PIN_A);
    IO_PortsSetPortBits(DIRECTION_PORT, DIRECTION_PIN_B);
}

void CCWTurn(unsigned int speed) {
    unsigned int newDuty = AdjustPWM(speed);
    PWM_SetDutyCycle(ENABLE_PIN_A, newDuty);
    PWM_SetDutyCycle(ENABLE_PIN_B, 0);
    IO_PortsSetPortBits(DIRECTION_PORT, DIRECTION_PIN_A);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_B);
}

void CWTurn(unsigned int speed) {
    unsigned int newDuty = AdjustPWM(speed);

    PWM_SetDutyCycle(ENABLE_PIN_A, 0);
    PWM_SetDutyCycle(ENABLE_PIN_B, newDuty);
    IO_PortsSetPortBits(DIRECTION_PORT, DIRECTION_PIN_A);
    IO_PortsSetPortBits(DIRECTION_PORT, DIRECTION_PIN_B);
}

void CCWTurnBack(unsigned int speed) {
    unsigned int newDuty = AdjustPWM(speed);
    PWM_SetDutyCycle(ENABLE_PIN_A, newDuty);
    PWM_SetDutyCycle(ENABLE_PIN_B, 0);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_A);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_B);
}

void CWTurnBack(unsigned int speed) {
    unsigned int newDuty = AdjustPWM(speed);

    PWM_SetDutyCycle(ENABLE_PIN_A, 0);
    PWM_SetDutyCycle(ENABLE_PIN_B, newDuty);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_A);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_B);
}

void StopMotors(void) {
    PWM_SetDutyCycle(ENABLE_PIN_A, 0);
    PWM_SetDutyCycle(ENABLE_PIN_B, 0);
}

void CurvyReverse(unsigned int lspeed, unsigned int rspeed) {
    unsigned int newlDuty = AdjustPWM(lspeed);
    unsigned int newrDuty = AdjustPWM(rspeed);
    PWM_SetDutyCycle(ENABLE_PIN_A, newlDuty);
    PWM_SetDutyCycle(ENABLE_PIN_B, newrDuty);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_A);
    IO_PortsSetPortBits(DIRECTION_PORT, DIRECTION_PIN_B);
}

void CurvyForward(unsigned int lspeed, unsigned int rspeed) {
    unsigned int newlDuty = AdjustPWM(lspeed);
    unsigned int newrDuty = AdjustPWM(rspeed);
    PWM_SetDutyCycle(ENABLE_PIN_A, newlDuty);
    PWM_SetDutyCycle(ENABLE_PIN_B, newrDuty);
    IO_PortsSetPortBits(DIRECTION_PORT, DIRECTION_PIN_A);
    IO_PortsClearPortBits(DIRECTION_PORT, DIRECTION_PIN_B);
}

unsigned int AdjustPWM(unsigned int speed) {
    float newSpeed = (float) speed;
    float thousand = 1000;
    float nine = 9.9;
    float tentwothree = 1023;
    float thirtythree = 33;
    float six = .6;
    float simVoltage = (speed / thousand) * nine;
    //printf("/n *****************SimVOlt= %f************ /n", simVoltage );
    float batVolt = AD_ReadADPin(BAT_VOLTAGE);
    //printf("/n *****************BatVolt= %f************ /n", batVolt );
    float newDuty = simVoltage / (((batVolt / tentwothree) * thirtythree) - six);
    newDuty = newDuty * 1000;
    //printf("/n *****************NewDuty= %f************ /n", newDuty );
    unsigned int u_newDuty = (unsigned int) newDuty;
    //printf("/n *****************uNewDuty= %d************ /n", u_newDuty );

    if (u_newDuty > 1000) {
        u_newDuty = 1000;
    }
    return u_newDuty;

}