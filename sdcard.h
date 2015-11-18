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
FRESULT openFile(const char *cwd, const char *filepath, FIL *file, BYTE mode);
FRESULT closeFile(FIL *file);
FRESULT readFromFile(FIL *file, uint8_t *buffer, WORD bytesToRead, WORD *bytesRead);
FRESULT writeToFile(FIL *file, const uint8_t *buffer, WORD bytesToWrite, WORD *bytesWritten);
FRESULT readDirectoryContents(char *directoryPath, DynamicString *directoryContents, size_t *bytesWritten);
