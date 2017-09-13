/*
 * File:   TrackWireSensors.c
 * Author: bbaltaxe
 *
 * Created on November 15, 2016, 7:44 PM
 */


#include "xc.h"
#include "IO_Ports.h"
#include "TrackWireSensors.h"

//return [null,null,front,mid] trackwire values

unsigned char CheckTracks(void) {
    unsigned char mid = 0, front = 0;
    mid = CheckMid();
    front = CheckFront();
    return (mid + (front << 1));
}

int CheckMid(void) {
    int trackVal = AD_ReadADPin(MID_TRACK_PIN);
    if (trackVal > 600) {
        return 0;
    } else {
       // printf("Mid = %d\n", trackVal);
        return 1;
    }
}

int CheckFront(void) {

    int trackVal = AD_ReadADPin(FRONT_TRACK_PIN);
    //printf("Front = %d\n", trackVal);
    if (trackVal > 600) {
        return 0;
    } else {
        return 1;
    }
    //printf("Front = %d\n", trackVal);

}