/*
 * ftp_command_executor.h
 *
 *  Created on: Oct 26, 2015
 *      Author: Clayton
 */

#ifndef FTP_COMMAND_EXECUTOR_H_
#define FTP_COMMAND_EXECUTOR_H_

#include "ftp_commands.h"
#include "dynamic_string.h"
#include "ftp_control_block.h"

void executeCommand
(
    FTPCommandID commandID,
	const char *arguments,
    DynamicString *reply,
	FtpPiStruct_t *PI_Struct
);

void executeCommand_USER
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_PASS
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_ACCT
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_CWD
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_CDUP
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_SMNT
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_REIN
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_QUIT
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_PORT
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_PASV
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_TYPE
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_STRU
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_MODE
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_RETR
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_STOR
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_STOU
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_APPE
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_ALLO
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_REST
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_RNFR
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_RNTO
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_ABOR
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_DELE
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_RMD
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_MKD
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_PWD
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_LIST
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_NLST
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_SITE
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_SYST
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_STAT
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_HELP
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

void executeCommand_NOOP
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
);

#endif /* FTP_COMMAND_EXECUTOR_H_ */
