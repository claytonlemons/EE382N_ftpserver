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
#include "sdcard.h"

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

bool _findNextPathSegment(const char *path, const char **segment, size_t *segmentLength)
{
	if (path == NULL || segment == NULL || segmentLength == NULL)
	{
		UARTprintf("Error: NULL argument passed into _findNextPathSegment\r\n");
		return false;
	}

	if (*path == '\0')
	{
		return false;
	}

	*segment = path;

	while (*path != '\0' && *path != '/')
	{
		path++;
	}

	*segmentLength = path - *segment;

	if (*path == '/')
	{
		*segmentLength += 1; // The +1 accounts for the trailing slash
	}

	return true;
}

// This function is used to resolve relative and absolute paths
bool resolveRelativeAbsolutePath(const Path basePath, const char *pathToJoinString, Path joinedPath)
{
	Path tempPath;

    // When the path to join starts with '/', we are dealing with an absolute path
    if (*pathToJoinString == '/')
    {
    	if (strlen(pathToJoinString) > MAX_PATH_LENGTH)
    	{
    		return false;
    	}
    	else
    	{
            // Copy the path to join
    		strncpy(tempPath, pathToJoinString, MAX_PATH_LENGTH);
    	}
    }
    else
    {
        strcpy(tempPath, basePath);

        // INVARIANT: endOfTempPath must always point to the NULL byte at the end of tempPath
        char * endOfTempPath = tempPath + strlen(tempPath);

        const char *segment = NULL;
        size_t segmentLength = 0;

        while (_findNextPathSegment(pathToJoinString, &segment, &segmentLength))
        {
        	if ((segmentLength == 2 && strncmp(segment, "..", segmentLength) == 0) || (segmentLength == 3 && strncmp(segment, "../", segmentLength) == 0))
        	{
        		while (*--endOfTempPath != '/');
        		*++endOfTempPath = '\0';
        	}
        	else if ((segmentLength == 1 && strncmp(segment, ".", segmentLength) == 0) || (segmentLength == 2 && strncmp(segment, "./", segmentLength) == 0))
        	{
        		// Nothing to do here
        	}
        	else
        	{
                // Add a trailing slash to tempPath if needed
                if (*(endOfTempPath - 1) != '/')
                {
                	if ((endOfTempPath + 1) - tempPath > MAX_PATH_LENGTH)
                	{
                		return false;
                	}
                	else
                	{
                		strcat(endOfTempPath, "/");
                	}
                }

        		if ((endOfTempPath + segmentLength) - tempPath > MAX_PATH_LENGTH)
        		{
        			return false;
        		}
        		else
        		{
        			strncat(endOfTempPath, pathToJoinString, segmentLength);
        			endOfTempPath += segmentLength;
        		}
        	}

        	pathToJoinString += segmentLength;
        }

    }

    strncpy(joinedPath, tempPath, MAX_PATH_LENGTH);

    size_t indexOfLastCharacterInJoinedPath = strlen(joinedPath) - 1;

    if (strcmp(joinedPath, "/") != 0 && joinedPath[indexOfLastCharacterInJoinedPath] == '/')
    {
    	joinedPath[indexOfLastCharacterInJoinedPath] = '\0';
    }

    return true;
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

bool isDirectory(const Path path)
{
	FRESULT fresult;
	FILINFO info;

	CHECK_FRESULT(f_stat(path, &info));

ERROR:
	return (fresult == FR_OK) && (info.fattrib & AM_DIR);
}

FRESULT getFileInfo(const Path cwd, const char *filePathString, FILINFO *fileInfo)
{
	FRESULT fresult;

	Path filePath;

	if (resolveRelativeAbsolutePath(cwd, filePathString, filePath) == false)
	{
		fresult = FR_PATH_TOO_LONG;
		CHECK_FRESULT(fresult);
	}

	CHECK_FRESULT(f_stat(filePath, fileInfo));

ERROR:

	return fresult;
}

FRESULT openFile(const Path cwd, const char *filePathString, FIL *file, BYTE mode)
{
	FRESULT fresult;

	Path filePath;

	if (resolveRelativeAbsolutePath(cwd, filePathString, filePath) == false)
	{
		fresult = FR_PATH_TOO_LONG;
		CHECK_FRESULT(fresult);
	}

	CHECK_FRESULT(f_open(file, filePath, mode));

ERROR:

	return fresult;
}

FRESULT openDirectory(const char *cwd, const char *directoryPathString, DIR *directory)
{
	FRESULT fresult;

	Path directoryPath;

	if (resolveRelativeAbsolutePath(cwd, directoryPathString, directoryPath) == false)
	{
		fresult = FR_PATH_TOO_LONG;
		CHECK_FRESULT(fresult);
	}


	CHECK_FRESULT(f_opendir(directory, directoryPath));

ERROR:
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
FRESULT readDirectoryContents(const char *directoryPath, DynamicString *directoryContents, size_t *totalBytesWritten)
{
    FRESULT fresult = FR_OK;

    DIR directory;
    FILINFO fileInfo;

    char *directoryContentsBuffer = directoryContents->buffer;
    size_t directoryContentsBufferLength = directoryContents->length;

    size_t bytesWritten;

    *totalBytesWritten = 0;

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
