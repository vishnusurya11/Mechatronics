
#ifndef _EXAMPLE_FILE_NAME_H    /* Guard against multiple inclusion */
#define _EXAMPLE_FILE_NAME_H

#include "AD.h"
#include "IO_Ports.h"
#include "BOARD.h"

#define PRESSED_THRESHOLD 200
#define DEPRESSED_THRESHOLD 10

#define BUMPERPORT PORTX
#define FRONTLEFT PIN4
#define FRONTRIGHT PIN3
#define BACKLEFT PIN6
#define BACKRIGHT PIN5

#define FLBUMP 1
#define FRBUMP 2
#define BLBUMP 4
#define BRBUMP 8
#define FBUMP 3
#define BBUMP 12

/*returns a 4 bit character corresponding to the bumpers which are on (1 is on 0 is off) in the following configuration:
 * [BackRight, BackLeft, FrontRight,FrontLeft]
 */
unsigned char CheckBumpers(void);

unsigned char CheckSavedBumpers(void);

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */