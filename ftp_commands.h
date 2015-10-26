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

extern FTPCommand ftpCommands[];

FTPCommandString ftpCommandIDToString(FTPCommandID commandID);

FTPCommandID ftpCommandStringToID(FTPCommandString commandString);

#endif
