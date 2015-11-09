/*
 * ftp_command_executor.c
 *
 *  Created on: Oct 26, 2015
 *      Author: Clayton
 */

#include "ftp_command_executor.h"

#include "ftp_commands.h"
#include "ftp_replies.h"
#include "ftp_parsing_utils.h"
#include "ftp_control_block.h"

void executeCommand(FTPCommandID ftpCommandID, const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{
	switch (ftpCommandID)
	{
		#define FTP_COMMAND_XMACRO(commandID) \
		case FTPCOMMANDID_##commandID: executeCommand_##commandID(arguments, replyBuffer, \
		    replyBufferLength, PI_Struct); break;
		#include "ftp_commands.def"
		default:
			break;
			// Error: unknown command!
	}
}

void executeCommand_USER(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{
    burnWhitespace(arguments);
    char UsernameBuff[64];

    // Get the Username from the arguments
    parseString(arguments, UsernameBuff, 64);
    //TODO: compare username against database. For now every user is welcome

    PI_Struct->PresState = WAIT_FOR_PASSWORD;
    // Return reply 331 to let the user know everything is correct
    formatFTPReply(FTPREPLYID_331, replyBuffer, replyBufferLength);

}

void executeCommand_PASS(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{
    burnWhitespace(arguments);
    char PasswordBuff[64];

    // Get the Username from the arguments
    parseString(arguments, PasswordBuff, 128);
    //TODO: verify user's password against database. For now every user is welcome

    PI_Struct->PresState = READY;
    // Return reply 230 to let the user know everything is correct
    formatFTPReply(FTPREPLYID_230, replyBuffer, replyBufferLength);
}

void executeCommand_ACCT(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_CWD(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_CDUP(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_SMNT(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_REIN(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_QUIT(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_PORT(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_PASV(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_TYPE(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_STRU(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_MODE(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_RETR(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_STOR(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_STOU(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_APPE(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_ALLO(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_REST(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_RNFR(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_RNTO(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_ABOR(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_DELE(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_RMD(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_MKD(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_PWD(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_LIST(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_NLST(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_SITE(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_SYST(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_STAT(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_HELP(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}

void executeCommand_NOOP(const char *arguments,
    char *replyBuffer, size_t replyBufferLength, FtpPiStruct_t *PI_Struct)
{

}
