
/*
 * File:   Beacon.c
 * Author: vnandi
 *
 * Created on November 21, 2016, 2:01 PM
 */


#include "xc.h"
#include "IO_Ports.h"
#include "Beacon1.h"


int CheckBeacon(void) {

    if (IO_PortsReadPort(BEACON_PORT) & BEACON_PIN){
        return 0;
    } else {
        return 1;
    }
    

}