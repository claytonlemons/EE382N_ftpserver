#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/cmdline.h"
#include "utils/uartstdio.h"
#include "fatfs/src/ff.h"
#include "fatfs/src/diskio.h"
#include "dynamic_string.h"

const char *fresultToString(FRESULT fresult);
void FatFSTickHandler(void);
FRESULT mountSDCard();
FRESULT getDirectoryContents(char *directoryPath, DynamicString *directoryContents, size_t *bytesWritten);
