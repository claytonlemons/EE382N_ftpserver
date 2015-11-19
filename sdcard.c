//*****************************************************************************
//
// sd_card.c - Example program for reading files from an SD card.
//
// Copyright (c) 2007-2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 10007 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************

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
#include "stdio.h"
#include "stdint.h"

#define CHECK_FRESULT(snippet) fresult = (snippet); if (fresult != FR_OK) goto ERROR;



static FATFS fatFS;

//*****************************************************************************
//
// A structure that holds a mapping between an FRESULT numerical code,
// and a string represenation.  FRESULT codes are returned from the FatFs
// FAT file system driver.
//
//*****************************************************************************
typedef struct
{
    FRESULT fresult;
    char *pcResultStr;
}
tFresultString;

//*****************************************************************************
//
// A macro to make it easy to add result codes to the table.
//
//*****************************************************************************
#define FRESULT_ENTRY(f)        { (f), (#f) }

//*****************************************************************************
//
// A table that holds a mapping between the numerical FRESULT code and
// it's name as a string.  This is used for looking up error codes for
// printing to the console.
//
//*****************************************************************************
tFresultString g_sFresultStrings[] =
{
    FRESULT_ENTRY(FR_OK),
    FRESULT_ENTRY(FR_NOT_READY),
    FRESULT_ENTRY(FR_NO_FILE),
    FRESULT_ENTRY(FR_NO_PATH),
    FRESULT_ENTRY(FR_INVALID_NAME),
    FRESULT_ENTRY(FR_INVALID_DRIVE),
    FRESULT_ENTRY(FR_DENIED),
    FRESULT_ENTRY(FR_EXIST),
    FRESULT_ENTRY(FR_RW_ERROR),
    FRESULT_ENTRY(FR_WRITE_PROTECTED),
    FRESULT_ENTRY(FR_NOT_ENABLED),
    FRESULT_ENTRY(FR_NO_FILESYSTEM),
    FRESULT_ENTRY(FR_INVALID_OBJECT),
    FRESULT_ENTRY(FR_MKFS_ABORTED)
};

//*****************************************************************************
//
// A macro that holds the number of result codes.
//
//*****************************************************************************
#define NUM_FRESULT_CODES (sizeof(g_sFresultStrings) / sizeof(tFresultString))

// This function is used to resolve relative and absolute paths
const char* resolveRelativeAbsolutePath(const char *cwd,
    const char *PathToResolve){
    char * finalPath =  malloc(strlen(cwd) + strlen(PathToResolve) + 1);

    // When the path to resolve starts with '/' we are dealing with an absolute
    // path
    if (*PathToResolve == '/'){
        // Increment the PathToResolve pointer to remove the first '/' because
        // the cwd already has one.
        PathToResolve += 1;
        // Copy the cwd and concatenate path to resolve
        strcpy(finalPath, cwd);
        strcat(finalPath, PathToResolve);
        return (const char*)finalPath;
    } else if (strlen(cwd) == 1){
        // This means we are at the root
        if(PathToResolve[0] == '.' && PathToResolve[1] == '.')
            // When we are already at the root and we receive a path trying to
            // go beyond the root, we return null
            //TODO: verify this is an appropriate response.
            *finalPath = '\0';
    } else {
        strcpy(finalPath, cwd);
        // This variable points to the end of the string from the "finalPath"
        // variable.
        char * EndOfPath = (finalPath + (strlen(cwd) - 2));
        if ((PathToResolve[0] == '.') &&
            (PathToResolve[1] == '.') &&
            (strlen(PathToResolve) == 2)){
            // This means we need to move up one directory
            while (*EndOfPath != '/'){
                EndOfPath--;
            }
            EndOfPath[1] = '\0';
            return finalPath;
        }
        unsigned int i = 0;
        unsigned int cwdCharsLeft = strlen(cwd);
        while((i < strlen(PathToResolve)) && (cwdCharsLeft > 0)){
            if((PathToResolve[0] == '.') &&
               (PathToResolve[1] == '.') &&
               (PathToResolve[2] == '/')){
                    // Move up one directory on the current working dir
                    while (*EndOfPath != '/'){
                        EndOfPath--;
                        cwdCharsLeft--;
                    }
                    EndOfPath[1] = '\0';
                    EndOfPath--;
                    // Add 3 to advance beyond the '/' we encountered.
                    PathToResolve += 3;
                    i += 3;
            } else {
                strcat(finalPath, PathToResolve);
                return finalPath;
            }
        }
    }
    // If we break out of the while loop it means the path is not valid so
    // return null
    *finalPath = '\0';
    return (const char *)finalPath;
}

//*****************************************************************************
//
// This function returns a string representation of an error code
// that was returned from a function call to FatFs.  It can be used
// for printing human readable error messages.
//
//*****************************************************************************
const char *fresultToString(FRESULT fresult)
{
    unsigned int uIdx;

    //
    // Enter a loop to search the error code table for a matching
    // error code.
    //
    for(uIdx = 0; uIdx < NUM_FRESULT_CODES; uIdx++)
    {
        //
        // If a match is found, then return the string name of the
        // error code.
        //
        if(g_sFresultStrings[uIdx].fresult == fresult)
        {
            return(g_sFresultStrings[uIdx].pcResultStr);
        }
    }

    //
    // At this point no matching code was found, so return a
    // string indicating unknown error.
    //
    return("UNKNOWN ERROR CODE");
}

//*****************************************************************************
//
// This is the handler for this interrupt.  FatFs requires a
// timer tick every 10 ms for internal timing purposes.
//
//*****************************************************************************
void FatFSTickHandler(void)
{
    //
    // Call the FatFs tick timer.
    //
    disk_timerproc();
}

FRESULT mountSDCard()
{
    FRESULT fresult = FR_OK;

    CHECK_FRESULT(f_mount(0, &fatFS));

ERROR:
	return fresult;
}

FRESULT getFileInfo(const char *cwd, const char *filepath, FILINFO *fileInfo)
{
	FRESULT fresult;

	// @TODO: Refactor this code into a function that properly handles relative and absolute paths
	char *finalPath = malloc(strlen(cwd) + strlen(filepath) + 1);

	// @TODO: add this back in once we can handle absolute paths
	//strcpy(finalPath, cwd);
	//strcat(finalPath, filepath);
	strcpy(finalPath, filepath);

	CHECK_FRESULT(f_stat(finalPath, fileInfo));

ERROR:
	free(finalPath);
	return fresult;
}

FRESULT openFile(const char *cwd, const char *filepath, FIL *file, BYTE mode)
{
	FRESULT fresult;

	// @TODO: Refactor this code into a function that properly handles relative and absolute paths
	char *finalPath = malloc(strlen(cwd) + strlen(filepath) + 1);

	// @TODO: add this back in once we can handle absolute paths
	//strcpy(finalPath, cwd);
	//strcat(finalPath, filepath);
	strcpy(finalPath, filepath);

	CHECK_FRESULT(f_open(file, finalPath, mode));

ERROR:
	free(finalPath);
	return fresult;
}

FRESULT openDirectory(const char *cwd, const char *directoryPath, DIR *directory)
{
	FRESULT fresult;

	// @TODO: Refactor this code into a function that properly handles relative and absolute paths
	char *finalPath = malloc(strlen(cwd) + strlen(directoryPath) + 1);

	// @TODO: add this back in once we can handle absolute paths
	//strcpy(finalPath, cwd);
	//strcat(finalPath, directoryPath);
	strcpy(finalPath, directoryPath);

	CHECK_FRESULT(f_opendir(directory, finalPath));

ERROR:
	free(finalPath);
	return fresult;
}

FRESULT closeFile(FIL *file)
{
	FRESULT fresult;

	CHECK_FRESULT(f_close(file));

ERROR:
	return fresult;
}

FRESULT readFromFile(FIL *file, uint8_t *buffer, WORD bytesToRead, WORD *bytesRead)
{
	FRESULT fresult;

	CHECK_FRESULT(f_read(file, buffer, bytesToRead, bytesRead));

ERROR:
	return fresult;
}

FRESULT writeToFile(FIL *file, const uint8_t *buffer, WORD bytesToWrite, WORD *bytesWritten)
{
	FRESULT fresult;

	CHECK_FRESULT(f_write(file, buffer, bytesToWrite, bytesWritten));

ERROR:
	return fresult;
}


// Open directoryPath and show its contents
FRESULT readDirectoryContents(char *directoryPath, DynamicString *directoryContents, size_t *totalBytesWritten)
{
    FRESULT fresult = FR_OK;

    DIR directory;
    FILINFO fileInfo;

    char *directoryContentsBuffer = directoryContents->buffer;
    size_t directoryContentsBufferLength = directoryContents->length;

    size_t bytesWritten;

    // Open the current directory for access.
    CHECK_FRESULT(f_opendir(&directory, directoryPath));

    // Enter loop to enumerate through all directory entries.
    for(;;)
    {
        // Read an entry from the directory.
    	CHECK_FRESULT(f_readdir(&directory, &fileInfo));

        // If the file name is blank, then this is the end of the
        // listing.
        if (!fileInfo.fname[0])
        {
            break;
        }


        // Print the entry information on a single line with formatting
        // to show the attributes, date, time, size, and name.
        bytesWritten = snprintf
		(
			directoryContentsBuffer,
			directoryContentsBufferLength,
			"%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s\r\n",
			(fileInfo.fattrib & AM_DIR) ? 'D' : '-',
			(fileInfo.fattrib & AM_RDO) ? 'R' : '-',
			(fileInfo.fattrib & AM_HID) ? 'H' : '-',
			(fileInfo.fattrib & AM_SYS) ? 'S' : '-',
			(fileInfo.fattrib & AM_ARC) ? 'A' : '-',
			(fileInfo.fdate >> 9) + 1980,
			(fileInfo.fdate >> 5) & 15,
			 fileInfo.fdate & 31,
			(fileInfo.ftime >> 11),
			(fileInfo.ftime >> 5) & 63,
			 fileInfo.fsize,
			 fileInfo.fname
		);

        if (directoryContentsBuffer)
        {
			directoryContentsBuffer += bytesWritten;
			directoryContentsBufferLength -= bytesWritten;
        }

        *totalBytesWritten += bytesWritten;
    }

ERROR:
    return fresult;
}
//
////*****************************************************************************
////
//// This function implements the "cd" command.  It takes an argument
//// that specifes the directory to make the current working directory.
//// Path separators must use a forward slash "/".  The argument to cd
//// can be one of the following:
//// * root ("/")
//// * a fully specified path ("/my/path/to/mydir")
//// * a single directory name that is in the current directory ("mydir")
//// * parent directory ("..")
////
//// It does not understand relative paths, so dont try something like this:
//// ("../my/new/path")
////
//// Once the new directory is specified, it attempts to open the directory
//// to make sure it exists.  If the new path is opened successfully, then
//// the current working directory (cwd) is changed to the new path.
////
////*****************************************************************************
//int
//Cmd_cd(int argc, char *argv[])
//{
//    unsigned int uIdx;
//    FRESULT fresult;
//
//    //
//    // Copy the current working path into a temporary buffer so
//    // it can be manipulated.
//    //
//    strcpy(g_cTmpBuf, g_cCwdBuf);
//
//    //
//    // If the first character is /, then this is a fully specified
//    // path, and it should just be used as-is.
//    //
//    if(argv[1][0] == '/')
//    {
//        //
//        // Make sure the new path is not bigger than the cwd buffer.
//        //
//        if(strlen(argv[1]) + 1 > sizeof(g_cCwdBuf))
//        {
//            UARTprintf("Resulting path name is too long\n");
//            return(0);
//        }
//
//        //
//        // If the new path name (in argv[1])  is not too long, then
//        // copy it into the temporary buffer so it can be checked.
//        //
//        else
//        {
//            strncpy(g_cTmpBuf, argv[1], sizeof(g_cTmpBuf));
//        }
//    }
//
//    //
//    // If the argument is .. then attempt to remove the lowest level
//    // on the CWD.
//    //
//    else if(!strcmp(argv[1], ".."))
//    {
//        //
//        // Get the index to the last character in the current path.
//        //
//        uIdx = strlen(g_cTmpBuf) - 1;
//
//        //
//        // Back up from the end of the path name until a separator (/)
//        // is found, or until we bump up to the start of the path.
//        //
//        while((g_cTmpBuf[uIdx] != '/') && (uIdx > 1))
//        {
//            //
//            // Back up one character.
//            //
//            uIdx--;
//        }
//
//        //
//        // Now we are either at the lowest level separator in the
//        // current path, or at the beginning of the string (root).
//        // So set the new end of string here, effectively removing
//        // that last part of the path.
//        //
//        g_cTmpBuf[uIdx] = 0;
//    }
//
//    //
//    // Otherwise this is just a normal path name from the current
//    // directory, and it needs to be appended to the current path.
//    //
//    else
//    {
//        //
//        // Test to make sure that when the new additional path is
//        // added on to the current path, there is room in the buffer
//        // for the full new path.  It needs to include a new separator,
//        // and a trailing null character.
//        //
//        if(strlen(g_cTmpBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_cCwdBuf))
//        {
//            UARTprintf("Resulting path name is too long\n");
//            return(0);
//        }
//
//        //
//        // The new path is okay, so add the separator and then append
//        // the new directory to the path.
//        //
//        else
//        {
//            //
//            // If not already at the root level, then append a /
//            //
//            if(strcmp(g_cTmpBuf, "/"))
//            {
//                strcat(g_cTmpBuf, "/");
//            }
//
//            //
//            // Append the new directory to the path.
//            //
//            strcat(g_cTmpBuf, argv[1]);
//        }
//    }
//
//    //
//    // At this point, a candidate new directory path is in chTmpBuf.
//    // Try to open it to make sure it is valid.
//    //
//    fresult = f_opendir(&g_sDirObject, g_cTmpBuf);
//
//    //
//    // If it cant be opened, then it is a bad path.  Inform
//    // user and return.
//    //
//    if(fresult != FR_OK)
//    {
//        UARTprintf("cd: %s\n", g_cTmpBuf);
//        return(fresult);
//    }
//
//    //
//    // Otherwise, it is a valid new path, so copy it into the CWD.
//    //
//    else
//    {
//        strncpy(g_cCwdBuf, g_cTmpBuf, sizeof(g_cCwdBuf));
//    }
//
//    //
//    // Return success.
//    //
//    return(0);
//}

//*****************************************************************************
//
// This function implements the "pwd" command.  It simply prints the
// current working directory.
//
//*****************************************************************************
//int
//Cmd_pwd(int argc, char *argv[])
//{
//    //
//    // Print the CWD to the console.
//    //
//    UARTprintf("%s\n", g_cCwdBuf);
//
//    //
//    // Return success.
//    //
//    return(0);
//}

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif
