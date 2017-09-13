/*
 * File: FindingTarget1SubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 *  file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
 *
 * This is provided as an example and a good place to start.
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunSM()
 * 10/23/11 18:20 jec      began conversion from SM.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "HSM.h"
#include "FindingTarget1SubHSM.h"
#include "Motors.h"
#include "TapeSensors.h"
#include "ES_Events.h"
#include "ES_Timers.h"
#include "Beacon1.h"
#include "Bumpers.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    DrivingForward,
    LookingForBeacon,
    GoToTarget,
    Relocate,
    AvoidTape,
} FindingTarget1SubHSMState_t;

static const char *StateNames[] = {
    "DrivingForward",
    "LookingForBeacon",
    "GoToTarget",
    "Relocate",
    "AvoidTape",
};

#define OOO 0
#define OOI 1
#define OIO 2
#define OII 3
#define IOO 4
#define IOI 5
#define IIO 6
#define III 7

/*returns a 4 bit character. corresponding to the values of each sensor. 1 bit for each of the following 
 * [Back,front left, front mid, front right]
 */

#define FORWARD_TICS 4000
#define TURN_TICS 1000
#define BEACON_SPIN_TICS 7500
#define BOUNCE_OFF_TICS 2000
#define OFFTIME 500
#define TURNTIME 7000//7 sec for one full rotation at 800

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static FindingTarget1SubHSMState_t CurrentState = DrivingForward; // <- change name to match ENUM
static uint8_t MyPriority;
static int toggleBit = 0;
static unsigned char bumpers = 0x0;
static int turnDir = 0; // for checking the direction of turn after target1
static int timerCount = 0; //for checking the second timer after bump
static newBeacon = 0;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitFindingTarget1SubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitFindingTarget1SubHSM(void) {
    ES_Event returnEvent;
    toggleBit = 0;
    bumpers = 0x0;
    turnDir = 0; // for checking the direction of turn after target1
    timerCount = 0; //for checking the second timer after bump

    CurrentState = DrivingForward;
    returnEvent = RunFindingTarget1SubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

void ReInitFindingTarget1SubHSM(void) {
    toggleBit = 0;
    timerCount = 0; //for checking the second timer after bump
    newBeacon = 0;
    turnDir = 0; // for checking the direction of turn after target1
    bumpers = 0x0;
}

/**
 * @Function RunFindingTarget1SubHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunFindingTarget1SubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    FindingTarget1SubHSMState_t nextState; // <- change type to correct enum
    //static unsigned char bumpers = 0x0;
    ES_Tattle(); // trace call stack

    static FindingTarget1SubHSMState_t lastState;
    //int Beacon = CheckBeacon();

    switch (CurrentState) {

        case AvoidTape:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    break;
                case TAPE_EVENT:
                    if (lastState == DrivingForward) {
                        switch (ThisEvent.EventParam) {
                            case OOI:
                            case OII:
                                CCWTank(700);
                                ES_Timer_InitTimer(BOUNCE_OFF_TIMER, BOUNCE_OFF_TICS);
                                break;
                            case IOO:
                            case IIO:
                                CWTank(700);
                                ES_Timer_InitTimer(BOUNCE_OFF_TIMER, BOUNCE_OFF_TICS);
                                break;
                            case IOI:
                            case OIO:
                            case III:

                                DriveBackward(700);
                                ES_Timer_InitTimer(BOUNCE_OFF_TIMER, BOUNCE_OFF_TICS);
                                break;

                        }
                    }
                    if (lastState == GoToTarget) {
                        switch (ThisEvent.EventParam) {
                            case OOI:
                            case OII:
                                CWTank(700);
                                ES_Timer_InitTimer(BOUNCE_OFF_TIMER, BOUNCE_OFF_TICS);
                                break;
                            case IOO:
                            case IIO:
                                CCWTank(700);
                                ES_Timer_InitTimer(BOUNCE_OFF_TIMER, BOUNCE_OFF_TICS);
                                break;
                            case IOI:
                            case OIO:
                            case III:
                                DriveForward(700);
                                ES_Timer_InitTimer(BOUNCE_OFF_TIMER, BOUNCE_OFF_TICS);
                                break;
                        }
                    }
                    break;
                case ES_TIMEOUT:
                    nextState = lastState;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
            }
            break;

        case DrivingForward: // in the first state, replace this with correct names
            //printf("TankTurn\n");
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_InitTimer(FORWARD_TIMER, FORWARD_TICS);
                    DriveForward(1000);
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == FORWARD_TIMER && (bumpers == 0)) {
                        nextState = LookingForBeacon;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    } else if (ThisEvent.EventParam == BUMP_TURN_TIMER && (timerCount == 1)) {
                        DriveForward(1000);
                        ES_Timer_InitTimer(FORWARD_TIMER, FORWARD_TICS / 2);
                        timerCount = 0;
                        bumpers = 0;
                    } else if (ThisEvent.EventParam == BUMP_TURN_TIMER && (timerCount == 0)) {
                        ES_Timer_InitTimer(BUMP_TURN_TIMER, TURNTIME / 8);
                        timerCount = 1;
                        if ((bumpers) & (FLBUMP)) {
                            CWTank(800);
                        } else if ((bumpers) & (FRBUMP)) {
                            CCWTank(800);
                        } else if ((bumpers) & (FBUMP)) {
                            CWTank(800);
                        }
                    }
                    break;
                case ES_EXIT:
                    ES_Timer_StopTimer(FORWARD_TIMER);
                    StopMotors();
                    break;

                case BEACON:
                    if (ThisEvent.EventParam == 1) {
                        nextState = GoToTarget;
                        makeTransition = TRUE;
                        // printf("**********************FOUND BEACON **************************************** \n");
                    } else {
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case TAPE_EVENT:
                    if (ThisEvent.EventParam & 7) {
                        lastState = DrivingForward;
                        nextState = AvoidTape;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case BUMPED:
                    //                    nextState = SteppingBack;
                    //                   makeTransition = FALSE;
                    //                    ThisEvent.EventType = ES_NO_EVENT; //
                    bumpers = ThisEvent.EventParam;

                    ES_Timer_InitTimer(BUMP_TURN_TIMER, TURNTIME / 8);
                    DriveBackward(900);

                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
        case LookingForBeacon:
            switch (ThisEvent.EventType) {

                case BEACON:
                    if ((ThisEvent.EventParam == 1)&&(newBeacon)) { //if new Beacon has been detected, go to it
                        nextState = GoToTarget;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        // printf("**********************FOUND BEACON **************************************** \n");
                    } else if ((ThisEvent.EventParam == 0)&&(!newBeacon)) { //if prior beacon is lost, set newBeacon high
                        ThisEvent.EventType = ES_NO_EVENT;
                        newBeacon = 1;
                    } else if ((ThisEvent.EventParam == 1)&&(!newBeacon)) { // do nothing if still looking at old beacon
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                    //                    if (ThisEvent.EventParam == 1) {
                    //                        nextState = GoToTarget;
                    //                        makeTransition = TRUE;
                    //                        //  printf("**********************FOUND BEACON **************************************** \n");
                    //                    } else {
                    //                        ThisEvent.EventType = ES_NO_EVENT;
                    //                    }
                    //                    break;
                case ES_ENTRY:
                    if (turnDir == 0) {
                        CWTank(700);
                        turnDir = 1;
                    } else {
                        CCWTank(700);
                        turnDir = 0;
                    }
                    ES_Timer_InitTimer(BEACON_SPIN_TIMER, BEACON_SPIN_TICS);
                    //ES_Timer_InitTimer(FORWARD_TIMER, FORWARD_TICS);
                    break;
                case ES_EXIT:
                    ES_Timer_StopTimer(BEACON_SPIN_TIMER);
                    //ES_Timer_StopTimer(FORWARD_TIMER);
                    StopMotors();
                    break;
                case ES_TIMEOUT:
                    nextState = DrivingForward;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default:
                    break;
            }
            break;
        case GoToTarget:
            switch (ThisEvent.EventType) {
                case TAPE_EVENT:
                    if (ThisEvent.EventParam & 8) {
                        //nextState = Dispense;
                        //makeTransition = TRUE;
                        ThisEvent.EventType = DISPENSE;
                        break;
                    } else if (ThisEvent.EventParam & 7) {
                        lastState = GoToTarget;
                        nextState = AvoidTape;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case BEACON:
                    if (ThisEvent.EventParam == 0) {
                        nextState = Relocate;
                        makeTransition = TRUE;
                        // printf("**********************LOST BEACON **************************************** \n");
                    } else {
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ES_ENTRY:
                    DriveBackward(900);
                    break;
                case BUMPED:
                    bumpers = ThisEvent.EventParam;
                    ES_Timer_InitTimer(BUMP_TURN_TIMER, TURNTIME / 8);
                    DriveForward(900);
                    break;
                case ES_TIMEOUT:
                    if (bumpers == 0) {
                        DriveBackward(900);
                        nextState = LookingForBeacon;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    } else if (ThisEvent.EventParam == BUMP_TURN_TIMER && (timerCount == 1)) {
                        DriveForward(800);
                        ES_Timer_InitTimer(FORWARD_TIMER, FORWARD_TICS / 2);
                        timerCount = 0;
                        bumpers = 0;
                    } else if (ThisEvent.EventParam == BUMP_TURN_TIMER && (timerCount == 0)) {
                        ES_Timer_InitTimer(BUMP_TURN_TIMER, TURNTIME / 8);
                        timerCount = 1;
                        if ((bumpers) & (BLBUMP)) {
                            CCWTank(800);
                        } else if ((bumpers) & (BRBUMP)) {
                            CWTank(800);
                        } else if ((bumpers) & (BBUMP)) {
                            CCWTank(800);
                        }
                    }
                    break;
                case ES_EXIT:
                    StopMotors();
                    break;
                default:
                    break;
            }
            break;
        case Relocate:
            switch (ThisEvent.EventType) {
                case TAPE_EVENT:
                    if (ThisEvent.EventParam & 8) {
                        //nextState = Dispense;
                        //makeTransition = TRUE;
                        ThisEvent.EventType = DISPENSE;
                    }
                    break;
                case BEACON:
                    if (ThisEvent.EventParam == 1) {
                        nextState = GoToTarget;
                        makeTransition = TRUE;
                        //printf("**********************FOUND BEACON **************************************** \n");
                    } else {
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ES_ENTRY:
                    CWTank(600);
                    toggleBit = 1;
                    ES_Timer_InitTimer(BEACON_SPIN_TIMER, BEACON_SPIN_TICS); // for timing out and back to looking for beacon
                    ES_Timer_InitTimer(FORWARD_TIMER, TURN_TICS); // for switching between looking left and right
                    break;
                case ES_EXIT:
                    ES_Timer_StopTimer(BEACON_SPIN_TIMER);
                    ES_Timer_StopTimer(FORWARD_TIMER);
                    StopMotors();
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == FORWARD_TIMER) {
                        if (toggleBit) {
                            toggleBit = 0;
                            CCWTank(600);
                        } else {
                            CWTank(600);
                            toggleBit = 1;
                        }
                        ES_Timer_InitTimer(FORWARD_TIMER, TURN_TICS + OFFTIME);
                    } else {
                        nextState = DrivingForward;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                default:
                    break;
            }
            break;
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunFindingTarget1SubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        //printf("\n nextstate=%d \n", CurrentState);
        RunFindingTarget1SubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

