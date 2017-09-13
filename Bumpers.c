/*
 * File:   Bumpers.c
 * Author: bbaltaxe
 *
 * Created on November 12, 2016, 5:26 PM
 */

#include "Bumpers.h"

static unsigned char savedBumpers;

/*returns a 4 bit character corresponding to the bumpers which are on (1 is on 0 is off) in the following configuration:
 * [FRONTLEFT,FRONTRIGHT,BACKLEFT,BACKRIGHT]
 */
unsigned char CheckBumpers(void) {
    unsigned char fl = FLeft();
    unsigned char fr = FRight();
    unsigned char bl = BLeft();
    unsigned char br = BRight();
    unsigned char returnVal = (!fl + (!fr << 1)+(!bl << 2)+(!br << 3));

    if (returnVal) {
        savedBumpers = returnVal;
    }
    return returnVal;
}

unsigned char CheckSavedBumpers(void) {
    return savedBumpers;
}
//-------------------------------Private Functions -----------------------------------------------------//

//return 1 if bumped, 0 if not

int FLeft(void) {
    if ((IO_PortsReadPort(BUMPERPORT) & FRONTLEFT) != 0) {
        return 1;
    } else {
        return 0;
    }
}

int FRight(void) {
    if ((IO_PortsReadPort(BUMPERPORT) & FRONTRIGHT) != 0) {
        return 1;
    } else {
        return 0;
    }
}

int BLeft(void) {
    if ((IO_PortsReadPort(BUMPERPORT) & BACKLEFT) != 0) {
        return 1;
    } else {
        return 0;
    }
}

int BRight(void) {
    if ((IO_PortsReadPort(BUMPERPORT) & BACKRIGHT) != 0) {
        return 1;
    } else {
        return 0;
    }
}