
/*
 * File: SubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel Elkaim and Soja-Marie Morgens
 *
 *  file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is another template file for the SubHSM's that is slightly differet, and
 * should be used for all of the subordinate state machines (flat or heirarchical)
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
#include "FindingTapeSubHSM.h"
#include "FollowingTapeSubHSM.h"
#include "FindingTarget1SubHSM.h"
#include "FindingTarget2SubHSM.h"
#include "LoadingSubHSM.h"
#include "Motors.h" //#include all sub state machines called
#include "Bumpers.h"
#include "TrackWireSensors.h"
#include "ES_Events.h"
#include "ES_Timers.h"
#include "Servo.h"
#include "Beacon1.h"
#include <stdio.h>
/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/
//Include any defines you need to do
#define BEACON_PORT PORTZ
#define BEACON_PIN PIN3
/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define TURN_OFF_TAPE_TICKS 1000
#define BACK_UP_TICKS 900
#define LOADTIME 10000
#define BALLTIME 2000
#define TWITCH 300
#define FIELD_CHECK_TICS 2350

#define UNKNOWN 0
#define LEFT 1
#define RIGHT 2


#define OOO 0
#define OOI 1
#define OIO 2
#define OII 3
#define IOO 4
#define IOI 5
#define IIO 6
#define III 7

typedef enum {
    InitPState,
    FindingTape,
    FollowingTape,
    CheckingLoader,
    Loading,
    FindingTarget1,
    FindingTarget2,
    Dispense,
    TurningOffTape,
    Reinitialize,
    CheckingFieldDirection,
    DeadBot,
} HSMState_t;

static const char *StateNames[] = {
    "InitPState",
    "FindingTape",
    "FollowingTape",
    "CheckingLoader",
    "Loading",
    "FindingTarget1",
    "FindingTarget2",
    "Dispense",
    "TurningOffTape",
    "Reinitialize",
    "CheckingFieldDirection",
    "DeadBot",
};


/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine
   Example: char RunAway(uint_8 seconds);*/
/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static HSMState_t CurrentState = InitPState; // <- change enum name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitHSM(uint8_t Priority) {
    MyPriority = Priority;
    // put us into the Initial PseudoState
    CurrentState = InitPState;
    // post the initial transition event
    if (ES_PostToService(MyPriority, INIT_EVENT) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostHSM(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunHSM(ES_Event ThisEvent)
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
ES_Event RunHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    HSMState_t nextState; // <- change type to correct enum
    static HSMState_t lastState; // <- change type to correct enum
    static uint16_t curTapeSensors = 0;
    ES_Tattle(); // trace call stack
    static unsigned char curBumpers = 0;
    static unsigned int ball1Retained = 0;
    static unsigned int ball2Released = 0;
    static unsigned int firstScan = 1;
    static unsigned int FieldSide = UNKNOWN;
    static unsigned int CheckedFieldSide = 0;


    switch (CurrentState) {
        case InitPState: // If current state is initial Pseudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state
                // Initialize all sub-state machines
                //            InitSubHSM();
                // now put the machine into the actual initial state
                nextState = FindingTape;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;

            }
            break;

        case FindingTape: // in the first state, replace this with correct names
            // run sub-state machine for this state
            //NOTE: the SubState Machine runs and responds to events before anything in the this
            //state machine does
            //        ThisEvent = RunSubHSM(ThisEvent);
            ThisEvent = RunFindingTapeSubHSM(ThisEvent);
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    break;
                case ES_EXIT:
                    StopMotors();
                    break;
                case TAPE_EVENT:
                    if (ThisEvent.EventParam & 7) {
                        nextState = FollowingTape;
                        curTapeSensors = ThisEvent.EventParam;
                        makeTransition = TRUE;
                    }
                    //printf("\n tape sensors: %d \n",curTapeSensors);
                    //ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default:
                    break;
            }
            break;

        case FollowingTape: // in the first state, replace this with correct names
            // run sub-state machine for this state
            //NOTE: the SubState Machine runs and responds to events before anything in the this
            //state machine does
            //        ThisEvent = RunSubHSM(ThisEvent);
            ThisEvent = RunFollowingTapeSubHSM(ThisEvent);

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    if (CheckedFieldSide == 0) {
                        ES_Timer_InitTimer(TURN_OFF_TAPE_TIMER, 2000);
                    }
                    break;
                case ES_EXIT:
                    StopMotors();
                    break;
                case BUMPED:

                    if ((ThisEvent.EventParam == FLBUMP) && (FieldSide == LEFT)) {
                        //go around bot to left
                        nextState = DeadBot;
                        printf("hit dead bot! FieldSide = %d -----------------------------", FieldSide);


                    } else
                        if ((ThisEvent.EventParam == FRBUMP) && (FieldSide == RIGHT)) {
                        //go around bot to right
                        nextState = DeadBot;
                        printf("hit dead bot! FieldSide = %d -----------------------------", FieldSide);

                    } else {
                        nextState = TurningOffTape;
                        //nextState = CheckingLoader;
                    }
                    makeTransition = TRUE;
                    curBumpers = ThisEvent.EventParam; //will use curbumpers in loading state
                    //printf("\n bumpers: %d \n",ThisEvent.EventParam);
                    ThisEvent.EventType = BUMPED;
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == TURN_OFF_TAPE_TIMER) {
                        //ES_Timer_StopTimer(TURN_OFF_TAPE_TIMER);
                        nextState = CheckingFieldDirection;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                default:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
            }


            break;

        case DeadBot:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    StopMotors();
                    break;
                case ES_EXIT:
                    StopMotors();
                    break;
            }
            break;


        case TurningOffTape: // in the first state, replace this with correct names
            // run sub-state machine for this state
            //NOTE: the SubState Machine runs and responds to events before anything in the this
            //state machine does
            //        ThisEvent = RunSubHSM(ThisEvent);

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    DriveBackward(900);
                    ES_Timer_InitTimer(BACK_UP_TIMER, BACK_UP_TICKS);
                    break;
                case ES_EXIT:
                    StopMotors();
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == TURN_OFF_TAPE_TIMER) {
                        nextState = Loading;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        DriveForward(1000);
                        ES_Timer_StopTimer(TURN_OFF_TAPE_TIMER);

                    } else if (ThisEvent.EventParam == BACK_UP_TIMER) {
                        ES_Timer_InitTimer(TURN_OFF_TAPE_TIMER, TURN_OFF_TAPE_TICKS);

                        ES_Timer_StopTimer(BACK_UP_TIMER);
                        if (curBumpers & FLBUMP) {
                            CWTank(800);
                        } else {
                            CCWTank(800);
                        }

                    }
                    break;
                    //                case BUMPED:
                    //                    nextState = TurningOffTape;
                    //                    makeTransition = TRUE;
                    //                    curBumpers = ThisEvent.EventParam;
                    //                    //printf("\n bumpers: %d \n",ThisEvent.EventParam);
                    //                    ThisEvent.EventType = ES_NO_EVENT;
                    //                    break;
                default: break;
            }
            //while(1);

            break;

        case Loading:

            //InitLoadingSubHSM(curBumpers);
            ThisEvent = RunLoadingSubHSM(ThisEvent);
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_InitTimer(LOAD_TIMER, LOADTIME);
                    //DriveForward(600);
                    break;
                case ES_EXIT:
                    StopMotors();
                    break;

                case TAPE_EVENT:
                    if (ThisEvent.EventParam & 7) {
                        nextState = FollowingTape;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == LOAD_TIMER) {
                        ES_Timer_StopTimer(LOAD_TIMER);
                        nextState = FindingTarget1;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                default: break;
            }
            break;

        case FindingTarget1:
            lastState = FindingTarget1;
            ThisEvent = RunFindingTarget1SubHSM(ThisEvent);
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    lastState = FindingTarget1;
                    DriveForward(800);
                    break;
                case ES_EXIT:
                    lastState = FindingTarget1;
                    StopMotors();
                    break;
                case DISPENSE:
                    RetainBall();
                    nextState = Dispense;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default: break;
            }
            break;

        case Dispense:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    StopMotors();
                    ReleaseBall();
                    ES_Timer_InitTimer(BALL_TIMER, BALLTIME);
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == BALL_TIMER) {
                        if (ball1Retained == 0 && ball2Released == 0) {
                            //ES_Timer_InitTimer(LOAD_TIMER, TWITCH); //these two lines to make it shimmy at loader
                            //CWTurnBack(800);
                            RetainBall();
                            ball1Retained = 1;
                            ThisEvent.EventType = ES_NO_EVENT;
                            ES_Timer_InitTimer(BALL_TIMER, BALLTIME);
                        } else if (ball1Retained == 1 && ball2Released == 0) {
                            ball1Retained = 0;
                            ReleaseBall();
                            ball2Released = 1;
                            //ES_Timer_InitTimer(TURN_OFF_TAPE_TIMER, TWITCH);  //these two lines to make it shimmy at loader
                            //CCWTurnBack(800);
                            ThisEvent.EventType = ES_NO_EVENT;
                            ES_Timer_InitTimer(BALL_TIMER, BALLTIME);
                        } else if (ball2Released == 1) {
                            ES_Timer_StopTimer(BALL_TIMER);
                            ball2Released = 0;
                            RetainBall();
                            if (lastState == FindingTarget1) {
                                nextState = FindingTarget2;
                            } else if (lastState == FindingTarget2) {
                                nextState = FindingTarget1;
                                ReInitFindingTarget2SubHSM();
                                ReInitFindingTarget1SubHSM();
                                ball1Retained = 0;
                                ball2Released = 0;
                                printf("startingOver");
                            }
                            makeTransition = TRUE;
                            ThisEvent.EventType = ES_NO_EVENT;
                        }
                    } else if ((ThisEvent.EventParam == LOAD_TIMER) || (ThisEvent.EventParam == TURN_OFF_TAPE_TIMER)) {
                        StopMotors();
                    }
                    break;
                case ES_EXIT:
                    StopMotors();
                    ES_Timer_StopTimer(BALL_TIMER);
                    break;
                case TAPE_EVENT:
                    break;

                default: break;
            }
            break;

        case FindingTarget2:
            ThisEvent = RunFindingTarget2SubHSM(ThisEvent);
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    DriveForward(800);
                    lastState = FindingTarget2;
                    break;
                case ES_EXIT:
                    StopMotors();
                    break;
                case DISPENSE:
                    nextState = Dispense;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                default: break;
            }
            break;

        case Reinitialize:
            printf("In reinitialize");
            //reinitialize local static variables then call all subHSM inits to do the same
            curTapeSensors = 0;
            curBumpers = 0;
            ball1Retained = 0;
            ball2Released = 0;
            //InitFindingTapeSubHSM();
            //InitFindingTarget1SubHSM();
            //InitFindingTarget2SubHSM();
            //InitFollowingTapeSubHSM();
            //InitLoadingSubHSM();
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            nextState = FindingTarget1;
            printf("at end \n");
            break;

        case CheckingFieldDirection:
            //printf("in checking field------------------------------------");
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    CheckedFieldSide = 0;
                    ES_Timer_InitTimer(TURN_OFF_TAPE_TIMER, FIELD_CHECK_TICS);
                    CWTank(900);
                    firstScan = 1;
                    break;
                case ES_EXIT:
                    ES_Timer_StopTimer(TURN_OFF_TAPE_TIMER);
                    StopMotors();
                    break;
                case BEACON:
                    if (firstScan == 1) {
                        FieldSide = LEFT;
                        printf("Field to the left-----------");
                    } else if (firstScan == 0) {
                        FieldSide = RIGHT;
                        printf("Field to the Right-----------");
                    }

                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == TURN_OFF_TAPE_TIMER) {
                        if (firstScan == 1) {
                            firstScan = 0;

                            ES_Timer_InitTimer(TURN_OFF_TAPE_TIMER, FIELD_CHECK_TICS);
                        } else if (firstScan == 0) {
                            CheckedFieldSide = 1;
                            nextState = FollowingTape;
                            makeTransition = TRUE;
                            ThisEvent.EventType = ES_NO_EVENT;
                        }
                    }
                    break;
            }
            break;





        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event

        RunHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/