/*
 * File: FindingTapeSubHSM.c
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
#include "FindingTapeSubHSM.h"
#include "Motors.h"
#include "TapeSensors.h"
#include "Bumpers.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    Turning,
    DrivingForward,
} FindingTapeSubHSMState_t;

static const char *StateNames[] = {
    "InitPSubState",
    "Turning",
    "DrivingForward",
};

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

static FindingTapeSubHSMState_t CurrentState = Turning; // <- change name to match ENUM
static uint8_t MyPriority;
static unsigned char bumpers = 0x0;
static unsigned int timerCount = 0;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitFindingTapeSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitFindingTapeSubHSM(void) {
    bumpers = 0x0;
    timerCount = 0;
    ES_Event returnEvent;
    CurrentState = Turning;
    returnEvent = RunFindingTapeSubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunFindingTapeSubHSM(ES_Event ThisEvent)
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
ES_Event RunFindingTapeSubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    FindingTapeSubHSMState_t nextState; // <- change type to correct enum

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case Turning: // in the first state, replace this with correct names
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_InitTimer(STEP_TIMER, TURNTIME);
                    CWTank(800);
                    break;
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == STEP_TIMER) {
                        StopMotors();
                        nextState = DrivingForward;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ES_EXIT:
                    StopMotors();
                    break;
                case BUMPED:
                    //                    nextState = SteppingBack;
                    //                   makeTransition = FALSE;
                    //                    ThisEvent.EventType = ES_NO_EVENT; //
                    bumpers = ThisEvent.EventParam;
                    break;
                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break; //???
        case DrivingForward: // in the first state, replace this with correct names
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_InitTimer(STEP_TIMER, TURNTIME);
                    DriveForward(1000);
                    break;
                case ES_EXIT:
                    StopMotors();
                    break;
                case BUMPED:
                    //                    nextState = SteppingBack;
                    //                   makeTransition = FALSE;
                    //                    ThisEvent.EventType = ES_NO_EVENT; //
                    bumpers = ThisEvent.EventParam;
                    ES_Timer_InitTimer(BUMP_TURN_TIMER, TURNTIME / 8);
                    //                    if ((bumpers) & (FBUMP)) {
                    //                    
                    //                    
                    //                    }
                    //                    else if ((bumpers) & (FLBUMP)) {
                    //                        CWTank(800);
                    //                    } else if ((bumpers) & (FRBUMP)) {
                    //                        CCWTank(800);
                    //                    }
                    DriveBackward(900);
                    break;
                case ES_NO_EVENT:
                    break;
                case ES_TIMEOUT:

                    if ((ThisEvent.EventParam == BUMP_TURN_TIMER)&&(timerCount == 0)) {
                        //printf("bumpers %d", bumpers);
                        ES_Timer_InitTimer(BUMP_TURN_TIMER, TURNTIME / 8);
                        timerCount = 1;
                        if ((bumpers) & (FBUMP)) {
                            CWTank(800);
                        } else if ((bumpers) & (FLBUMP)) {
                            CWTank(800);
                        } else if ((bumpers) & (FRBUMP)) {
                            CCWTank(800);
                        }
                    } else if ((ThisEvent.EventParam == BUMP_TURN_TIMER)&&(timerCount == 1)) {
                        DriveForward(1000);

                        //printf("timerCount is %d",timerCount);
                        timerCount = 0;
                        bumpers = 0;

                    } else if ((ThisEvent.EventParam == STEP_TIMER)&& (bumpers == 0)) {
                        DriveForward(1000);
                        // printf("not going bump turn");
                    }

            }
            break;
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunFindingTapeSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        //printf("\n nextstate=%d \n", CurrentState);
        RunFindingTapeSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

