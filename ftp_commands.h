#ifndef FTP_COMMANDS_H_
#define FTP_COMMANDS_H_

#include <inc/hw_types.h>

#include <stdlib.h>
#include <string.h>

typedef const char *const FTPCommandString;

typedef enum
{
	#define FTP_COMMAND_XMACRO(commandID, isImplemented) commandID,
	#include "ftp_commands.def"
	UNKNOWN_COMMAND
} FTPCommandID;

typedef struct
{
	FTPCommandID commandID;
	FTPCommandString commandString;
	tBoolean isImplemented;
} FTPCommand;

FTPCommand ftpCommands[];

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
		}
	}

	return UNKNOWN_COMMAND;
}

#endif
