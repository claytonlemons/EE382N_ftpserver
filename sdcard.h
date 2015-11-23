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

#ifndef SDCARD_H
#define SDCARD_H

#define MAX_PATH_LENGTH 255

typedef char Path[MAX_PATH_LENGTH + 1];

const char *fresultToString(FRESULT fresult);
void FatFSTickHandler(void);
FRESULT mountSDCard();
bool isDirectory(const Path path);
FRESULT getFileInfo(const Path cwd, const char *filepath, FILINFO *fileInfo);
FRESULT openDirectory(const Path cwd, const char *directoryPath, DIR *directory);
FRESULT openFile(const Path cwd, const char *filepath, FIL *file, BYTE mode);
FRESULT closeFile(FIL *file);
FRESULT readFromFile(FIL *file, uint8_t *buffer, WORD bytesToRead, WORD *bytesRead);
FRESULT writeToFile(FIL *file, const uint8_t *buffer, WORD bytesToWrite, WORD *bytesWritten);
FRESULT readDirectoryContents(const char *directoryPath, DynamicString *directoryContents, size_t *bytesWritten);

bool resolveRelativeAbsolutePath(const Path basePath, const char *pathString, Path joinedPath);

#endif
