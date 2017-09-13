/*
 * File:   Beacon.c
 * Author: vnandi
 *
 * Created on November 21, 2016, 2:01 PM
 */


#include "xc.h"
#include "IO_Ports.h"
#include "Beacon.h"


int CheckBeacon(void) {

    int BeaconVal = AD_ReadADPin(BEACON_PIN);
    
    if (BeaconVal > 600) {
        return 0;
    } else {
        return 1;
    }
    

}