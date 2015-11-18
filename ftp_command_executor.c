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
#include "FtpProtocolInterpreter.h"
#include "UartDebug.h"
#include "sdcard.h"
#include "stdlib.h"

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
    UARTPrintLn("USER CMD Executed");

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
    UARTPrintLn("PASS CMD Executed");
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
    PI_Struct->PresState = QUIT;
    formatFTPReply(FTPREPLYID_221, reply);
}

void executeCommand_PORT
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
    burnWhitespace(arguments);
    const char *DebugPortContents = arguments;
    FTP_PARSE(HostPort(arguments, &(PI_Struct->hostPort)));

    // Debug UART Print
    UARTPrintLn("PORT CMD Executed");
    char debugPortBuff[64];
    DynamicString debugPort;
    initializeDynamicString(&debugPort, debugPortBuff, sizeof(debugPortBuff));
    FTP_PARSE(PrintableString(DebugPortContents, &debugPort));
    UARTPrintLn(debugPort.buffer);
    finalizeDynamicString(&debugPort);

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

    UARTPrint("executeCommand_TYPE Called!\r\n");
    TypeCode PrevTypeCode = PI_Struct->typeCode;
    FTP_PARSE(TypeCode(arguments, &(PI_Struct->typeCode)));
    if((PI_Struct->typeCode) == TYPECODE_UNKNOWN){
        PI_Struct->typeCode = PrevTypeCode;
    	formatFTPReply(FTPREPLYID_504, reply);
    } else {
        formatFTPReply(FTPREPLYID_200, reply);
    }
}

void executeCommand_STRU
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
    UARTPrint("executeCommand_STRU Called!\r\n");
    FTP_PARSE(StructureCode(arguments, &(PI_Struct->structCode)));
    if((PI_Struct->structCode) != STRUCTURECODE_F){
        // The only supported structure code is File
        PI_Struct->structCode = STRUCTURECODE_F;
    	formatFTPReply(FTPREPLYID_504, reply);
    } else {
        formatFTPReply(FTPREPLYID_200, reply);
    }
}

void executeCommand_MODE
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
    UARTPrint("executeCommand_MODE Called!\r\n");
    FTP_PARSE(ModeCode(arguments, &(PI_Struct->modeCode)));
    if((PI_Struct->modeCode) != MODECODE_S){
        // The only supported mode code is Stream
        PI_Struct->modeCode = MODECODE_S;
    	formatFTPReply(FTPREPLYID_504, reply);
    } else {
        formatFTPReply(FTPREPLYID_200, reply);
    }
}

void executeCommand_RETR
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
    // Send msg 150 to let the client the file is valid
	char fileNameBuff[64];
    DynamicString fileName;
    initializeDynamicString(&fileName, fileNameBuff, sizeof(fileNameBuff));
    // Get the file name from the arguments
    FTP_PARSE(PrintableString(arguments, &fileName));

    FRESULT fresult = openFile("/", fileName.buffer, &PI_Struct->DataStructure.file, FA_READ);
    if (fresult != FR_OK)
    {
    	formatFTPReply(FTPREPLYID_550, reply);
    	return;
    }

    PI_Struct->DataStructure.DtpState = TX_FILE;
    PI_Struct->DataStructure.bytesRemaining = PI_Struct->DataStructure.file.fsize;
    PI_Struct->DataStructure.sType = FromFile;

    formatFTPReply(FTPREPLYID_150, reply, fileName.buffer);
    if (ftp_OpenDataConnection(PI_Struct) != 0) {
        // @TODO: An error occured. Reply with a message
        return;
    }
    finalizeDynamicString(&fileName);
}

void executeCommand_STOR
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
    // Send msg 150 to let the client the file is valid
	char fileNameBuff[64];
    DynamicString fileName;
    initializeDynamicString(&fileName, fileNameBuff, sizeof(fileNameBuff));
    // Get the file name from the arguments
    FTP_PARSE(PrintableString(arguments, &fileName));

    FRESULT fresult = openFile("/", fileName.buffer, &PI_Struct->DataStructure.file, FA_WRITE | FA_CREATE_ALWAYS); // @TODO: use the cwd from the PI_Struct
    if (fresult != FR_OK)
    {
    	formatFTPReply(FTPREPLYID_550, reply);
    	return;
    }

    PI_Struct->DataStructure.DtpState = RX_FILE;
    PI_Struct->DataStructure.bytesRemaining = 0;
    PI_Struct->DataStructure.sType = FromFile;

    formatFTPReply(FTPREPLYID_150, reply, fileName.buffer);
    if (ftp_OpenDataConnection(PI_Struct) != 0) {
        // @TODO: An error occured. Reply with a message
        return;
    }
    finalizeDynamicString(&fileName);
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
    UARTPrintLn("Reading root dir...");
    DynamicString directoryContents;
    initializeDynamicString(&directoryContents, NULL, 0);

    size_t bytesWritten = 0;
    readDirectoryContents("/", &directoryContents, &bytesWritten);

    resizeDynamicString(&directoryContents, bytesWritten);
    readDirectoryContents("/", &directoryContents, &bytesWritten);

    UARTPrintLn(directoryContents.buffer);

    formatFTPReply(FTPREPLYID_213, reply, directoryContents.buffer);

    finalizeDynamicString(&directoryContents);

    UARTPrintLn(reply->buffer);
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
