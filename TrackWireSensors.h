/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _TRACK_WIRE_SENSOR_H    /* Guard against multiple inclusion */
#define _TRACK_WIRE_SENSOR_H

#include "xc.h"
#include "AD.h"
#include "BOARD.h"
#include "IO_Ports.h"
#include "LED.h"
#include "stdio.h"
#include "stdlib.h"
#include "pwm.h"
#include "serial.h"
#include "math.h"

//#define TRACK_PORT PORTX
#define MID_TRACK_PIN AD_PORTW3
#define FRONT_TRACK_PIN AD_PORTW4

#define FRONT_TRACK 1
#define MID_TRACK 2

unsigned char CheckTracks(void);

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
