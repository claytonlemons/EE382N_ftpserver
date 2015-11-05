#ifndef FTP_COMMANDS_H_
#define FTP_COMMANDS_H_

#include <stdlib.h>
#include <string.h>

typedef const char *const FTPCommandString;

typedef enum
{
	#define FTP_COMMAND_XMACRO(commandID) FTPCOMMANDID_##commandID,
	#include "ftp_commands.def"
	UNKNOWN_COMMAND
} FTPCommandID;

FTPCommandString ftpCommandIDToString(FTPCommandID commandID);

FTPCommandID ftpCommandStringToID(FTPCommandString commandString);

#endif
