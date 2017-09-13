#include <BOARD.h>
#include <xc.h>
#include <stdio.h>
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "Motors.h"
#include "Bumpers.h"
#include "TapeSensors.h"
#include "AD.h"
#include "TrackWireSensors.h"
#include "Servo.h"
#include "RC_Servo.h"
#include "Beacon1.h"


#define SYNC_TICS 200

void main(void) {
    ES_Return_t ErrorType;
    PWM_Init();
    BOARD_Init();
    RC_Init();

    printf("Starting ES Framework Template\r\n");
    printf("using the 2nd Generation Events & Services Framework\r\n");


    // Your hardware initialization function calls go here-------------------------------------------------------//

    //for bumpers
    IO_PortsSetPortInputs(BUMPERPORT, PIN3 | PIN4 | PIN5 | PIN6);

    //for motors
    PWM_AddPins(ENABLE_PIN_A); //Enable A
    PWM_AddPins(ENABLE_PIN_B); //Enable B    
    IO_PortsSetPortOutputs(DIRECTION_PORT, DIRECTION_PIN_B); //Direction B
    IO_PortsSetPortOutputs(DIRECTION_PORT, DIRECTION_PIN_A); //Direction A

    //for tape sensors
    AD_AddPins(FRONT_LEFT_PIN | FRONT_MID_PIN | FRONT_RIGHT_PIN | BACK_PIN | FRONT_TRACK_PIN | MID_TRACK_PIN);
    IO_PortsSetPortOutputs(SYNCHRONOUS_PORT, SYNCHRONOUS_PIN); //set port to output

    //For Servo
    RC_AddPins(RCPIN);
    RC_SetPulseTime(RCPIN,2000);
//    while(1){
//    int k;
//    for ( k = 0; k < 180000; k++)
//      RC_SetPulseTime(RCPIN, 1100);
//    
//    for ( k = 0; k < 180000; k++)
//        RC_SetPulseTime(RCPIN, 2000);
//    }
//    
    

    ES_Timer_InitTimer(TAPE_SENSOR_SERVICE_TIMER, SYNC_TICS);
    
    //For Beacon
    IO_PortsSetPortInputs(BEACON_PORT, BEACON_PIN);

    
//    while(1){
//        DriveForward(700);
//    }
    //-------------------------------------------------------------------------------------------------------------//
    // now initialize the Events and Services Framework and start it running
    ErrorType = ES_Initialize();
    if (ErrorType == Success) {
        ErrorType = ES_Run();

    }
    //if we got to here, there was an error
    switch (ErrorType) {
        case FailedPointer:
            printf("Failed on NULL pointer");
            break;
        case FailedInit:
            printf("Failed Initialization");
            break;
        default:
            printf("Other Failure: %d", ErrorType);
            break;
    }
    for (;;)
        ;

};

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/


