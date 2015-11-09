/*
 * commands.c
 *
 *  Created on: Oct 25, 2015
 *      Author: Clayton
 */

#include "ftp_commands.h"

typedef struct
{
	FTPCommandID commandID;
	FTPCommandString commandString;
} FTPCommand;

static FTPCommand ftpCommands[] =
{
	#define FTP_COMMAND_XMACRO(commandID) { FTPCOMMANDID_##commandID, #commandID},
	#include "ftp_commands.def"
	{ UNKNOWN_COMMAND, NULL }
};


FTPCommandString ftpCommandIDToString(FTPCommandID commandID)
{
	return ftpCommands[commandID].commandString;
}

FTPCommandID ftpCommandStringToID(FTPCommandString commandString)
{
	FTPCommand *ftpCommandPtr = ftpCommands;
	while (ftpCommandPtr != NULL)
	{
		if (strcmp(commandString, ftpCommandPtr->commandString) == 0)
		{
			return ftpCommandPtr->commandID;
		} else{
            ftpCommandPtr++;
        }
	}

	return UNKNOWN_COMMAND;
}
