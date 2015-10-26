/*
 * commands.c
 *
 *  Created on: Oct 25, 2015
 *      Author: Clayton
 */

#include "ftp_commands.h"

FTPCommand ftpCommands[] =
{
	#define FTP_COMMAND_XMACRO(commandID, isImplemented) { commandID, "#commandId", isImplemented },
	#include "ftp_commands.def"
	{ UNKNOWN_COMMAND, NULL, false }
};

