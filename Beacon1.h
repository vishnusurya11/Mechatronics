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

#ifndef BEACON1_H    /* Guard against multiple inclusion */
#define BEACON1_H



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


#define BEACON_PORT PORTZ
#define BEACON_PIN PIN3



int CheckBeacon(void);

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
