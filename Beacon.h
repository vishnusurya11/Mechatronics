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

#ifndef BEACON_H    /* Guard against multiple inclusion */
#define BEACON_H



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


#define BEACON_PIN AD_PORTW7



int CheckBeacon(void);

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
