/*
 * ftp_command_executor.c
 *
 *  Created on: Oct 26, 2015
 *      Author: Clayton
 */

#include "ftp_command_executor.h"

#include "ftp_commands.h"

void executeCommand(FTPCommandID ftpCommandID, const char *arguments, char *replyBuffer, size_t replyBufferLength)
{
	switch (ftpCommandID)
	{
		#define FTP_COMMAND_XMACRO(commandID) \
		case FTPCOMMANDID_##commandID: executeCommand_##commandID(arguments, replyBuffer, replyBufferLength); break;
		#include "ftp_commands.def"
		default:
			break;
			// Error: unknown command!
	}
}

void executeCommand_USER(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_PASS(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_ACCT(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_CWD(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_CDUP(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_SMNT(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_REIN(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_QUIT(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_PORT(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_PASV(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_TYPE(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_STRU(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_MODE(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_RETR(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_STOR(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_STOU(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_APPE(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_ALLO(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_REST(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_RNFR(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_RNTO(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_ABOR(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_DELE(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_RMD(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_MKD(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_PWD(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_LIST(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_NLST(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_SITE(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_SYST(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_STAT(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_HELP(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}

void executeCommand_NOOP(const char *arguments, char *replyBuffer, size_t replyBufferLength)
{

}
