/*
 * ftp_command_executor.c
 *
 *  Created on: Oct 26, 2015
 *      Author: Clayton
 */

#include "ftp_command_executor.h"

#include "ftp_commands.h"
#include "dynamic_string.h"
#include "ftp_control_block.h"
#include "ftp_replies.h"
#include "ftp_parsing_utils.h"

void executeCommand
(
	FTPCommandID ftpCommandID,
	const char *arguments,
    DynamicString *reply,
	FtpPiStruct_t *PI_Struct
)
{
	switch (ftpCommandID)
	{
		#define FTP_COMMAND_XMACRO(commandID) \
		case FTPCOMMANDID_##commandID: executeCommand_##commandID(arguments, reply, PI_Struct); break;
		#include "ftp_commands.def"
		default:
			break;
			// Error: unknown command!
	}
}

void executeCommand_USER
(
	const char *arguments,
    DynamicString *reply,
	FtpPiStruct_t *PI_Struct
)
{
    burnWhitespace(arguments);

    char usernameBuffer[64];
    DynamicString username;
    initializeDynamicString(&username, usernameBuffer, sizeof(usernameBuffer));

    // Get the username from the arguments
    FTP_PARSE(String(arguments, &username));
    //TODO: compare username against database. For now every user is welcome

    finalizeDynamicString(&username);

    PI_Struct->PresState = WAIT_FOR_PASSWORD;
    // Return reply 331 to let the user know everything is correct
    formatFTPReply(FTPREPLYID_331, reply);

}

void executeCommand_PASS
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
    burnWhitespace(arguments);

    char passwordBuffer[64];
	DynamicString password;
	initializeDynamicString(&password, passwordBuffer, sizeof(passwordBuffer));

    // Get the password from the arguments
	FTP_PARSE(String(arguments, &password));
    //TODO: verify user's password against database. For now any password works

    finalizeDynamicString(&password);

    PI_Struct->PresState = READY;
    // Return reply 230 to let the user know everything is correct
    formatFTPReply(FTPREPLYID_230, reply);
}

void executeCommand_ACCT
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_CWD
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_CDUP
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_SMNT
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_REIN
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_QUIT
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_PORT
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
    burnWhitespace(arguments);

    FTP_PARSE(HostPort(arguments, &(PI_Struct->hostPort)));

    formatFTPReply(FTPREPLYID_200, reply);
}

void executeCommand_PASV
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_TYPE
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_STRU
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_MODE
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_RETR
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_STOR
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_STOU
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_APPE
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_ALLO
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_REST
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_RNFR
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_RNTO
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_ABOR
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_DELE
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_RMD
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_MKD
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_PWD
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_LIST
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_NLST
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_SITE
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_SYST
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_STAT
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_HELP
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_502, reply);
}

void executeCommand_NOOP
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	formatFTPReply(FTPREPLYID_200, reply);
}
