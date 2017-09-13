/*
 * File: LoadingSubHSM.c
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
#include "LoadingSubHSM.h"
#include "Motors.h"
#include "TapeSensors.h"
#include "ES_Events.h"
#include "TrackWireSensors.h"
#include "Bumpers.h"
#include "ES_Timers.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    Backup,
    TankLeft,
    TankRight,
} LoadingSubHSMState_t;

static const char *StateNames[] = {
    "InitPSubState",
    "Backup",
    "TankLeft",
    "TankRight",
};



/*returns a 4 bit character. corresponding to the values of each sensor. 1 bit for each of the following 
 * [Back,front left, front mid, front right]
 */

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

static LoadingSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;
static unsigned char bumpers;
static int seenMid = FALSE;
//static unsigned char alreadyBacking;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitLoadingSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitLoadingSubHSM(unsigned char curBumpers) {
    ES_Event returnEvent;
    //    bumpers = curBumpers;
    //    if (alreadyBacking) {
    //        CurrentState = Backup;
    //    } else if (bumpers &= 1) {
    //        CurrentState = TankRight;
    //    } else {
    //        CurrentState = TankLeft;
    //    }
    CurrentState = InitPSubState;
    returnEvent = RunLoadingSubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunLoadingSubHSM(ES_Event ThisEvent)
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
ES_Event RunLoadingSubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    LoadingSubHSMState_t nextState; // <- change type to correct enum
    //static unsigned char bumpers = 0x0;
    ES_Tattle(); // trace call stack


    switch (CurrentState) {
        case InitPSubState:
            DriveForward(900);
            switch (ThisEvent.EventType) {
                case TRACK:
                    bumpers = CheckSavedBumpers();

                    if (bumpers & 1) {
                        nextState = TankRight;
                        makeTransition = TRUE;
                        //ThisEvent.EventType = ES_NO_EVENT;
                    } else if (bumpers & 2) {
                        nextState = TankLeft;
                        makeTransition = TRUE;
                        //ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;
                case ES_ENTRY:
                    break;

                default:
                    break;
            }
            break;

        case TankLeft:

            switch (ThisEvent.EventType) {
                case TRACK:
                    switch (ThisEvent.EventParam) {
                        case MID_TRACK:
                            CCWTank(800);
                            //seenMid = TRUE;
                            ThisEvent.EventType = ES_NO_EVENT;
                            break;
                        case 3:
                            nextState = Backup;
                            makeTransition = TRUE;
                            //ThisEvent.EventType = ES_NO_EVENT;
                            break;
                        case FRONT_TRACK:
                            if (seenMid) {
                                nextState = Backup;
                                makeTransition = TRUE;
                                //ThisEvent.EventType = ES_NO_EVENT;
                            }
                            break;

                        default:
                            ThisEvent.EventType = ES_NO_EVENT;
                            break;
                    }
                    break;
                case ES_ENTRY:
                    seenMid = FALSE;
                    break;
                default:
                    break;
            }

            break;
        case TankRight:

            switch (ThisEvent.EventType) {
                case TRACK:
                    switch (ThisEvent.EventParam) {
                        case MID_TRACK:
                            CWTank(800);
                            //seenMid = TRUE;
                            ThisEvent.EventType = ES_NO_EVENT;
                            break;
                        case 3:
                            nextState = Backup;
                            makeTransition = TRUE;
                            //ThisEvent.EventType = ES_NO_EVENT;
                            break;
                        case FRONT_TRACK:
                            if (seenMid) {
                                nextState = Backup;
                                makeTransition = TRUE;
                            }
                            break;
                        default:
                            ThisEvent.EventType = ES_NO_EVENT;
                            break;
                    }
                    break;
                case ES_ENTRY:
                    seenMid = FALSE;
                    break;
                default:
                    break;
            }
        case Backup:
            switch (ThisEvent.EventType) {
                case TRACK:
                    if ((ThisEvent.EventParam == 3) || seenMid) {
                        DriveBackward(1000);
                        ThisEvent.EventType = ES_NO_EVENT;

                    }
                    break;
                case ES_ENTRY:
                    break;
                case ES_EXIT:
                    nextState = InitPSubState;
                    CurrentState = InitPSubState;
                    break;
                default: break;
            }

            break;
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        //printf("Exit Event\n");
        RunLoadingSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        //printf("\n nextstate=%d \n", CurrentState);
        RunLoadingSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

