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
            formatFTPReply(FTPREPLYID_502, reply);
            break;
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
	UARTPrintLn("CWD CMD Executed");
    const char * NewDir;
    char Buffer[64];
    DynamicString ReceivedDir;
    initializeDynamicString(&ReceivedDir, Buffer, sizeof(Buffer));

    // Get the path from the arguments
    FTP_PARSE(PrintableString(arguments, &ReceivedDir));
    NewDir = resolveRelativeAbsolutePath(PI_Struct->CWD, ReceivedDir.buffer);
    const char * Debug = NewDir;
    UARTPrintLn(Debug);
    if(openDirectory(PI_Struct->CWD, NewDir, &PI_Struct->DataStructure.directory) == FR_OK){
        strncpy(PI_Struct->CWD, NewDir, sizeof(PI_Struct->CWD));
        formatFTPReply(FTPREPLYID_250, reply);
    } else {
        formatFTPReply(FTPREPLYID_501, reply);
    }
}

void executeCommand_CDUP
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	UARTPrintLn("CDUP CMD Executed");
	strcpy(PI_Struct->CWD, resolveRelativeAbsolutePath(PI_Struct->CWD, ".."));
	UARTPrintLn(PI_Struct->CWD);
    formatFTPReply(FTPREPLYID_200, reply);
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
	static uint16_t portNumber = 5000;

	PI_Struct->hostPort.hostNumber = PI_Struct->MessageConnection->local_ip;
	PI_Struct->hostPort.portNumber = portNumber++; // @TODO: We'll need to make this more robust for multiple connections

	uint8_t *hostNumberAsByteArray = (uint8_t *) &(PI_Struct->hostPort.hostNumber.addr);
	uint8_t *portNumberAsByteArray = (uint8_t *) &(PI_Struct->hostPort.portNumber);

    formatFTPReply
	(
		FTPREPLYID_227,
    	reply,
		hostNumberAsByteArray[0],
		hostNumberAsByteArray[1],
		hostNumberAsByteArray[2],
		hostNumberAsByteArray[3],
		portNumberAsByteArray[1],
		portNumberAsByteArray[0]
	);
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

    FRESULT fresult = openFile(PI_Struct->CWD, fileName.buffer, &PI_Struct->DataStructure.file, FA_READ);
    if (fresult != FR_OK)
    {
    	formatFTPReply(FTPREPLYID_550, reply);
    	return;
    }

    PI_Struct->DataStructure.dtpState = STATE_SEND_FILE;
    PI_Struct->DataStructure.bytesRemaining = PI_Struct->DataStructure.file.fsize;

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

    FRESULT fresult = openFile(PI_Struct->CWD, fileName.buffer, &PI_Struct->DataStructure.file, FA_WRITE | FA_CREATE_ALWAYS); // @TODO: use the cwd from the PI_Struct
    if (fresult != FR_OK)
    {
    	formatFTPReply(FTPREPLYID_550, reply);
    	return;
    }

    PI_Struct->DataStructure.dtpState = STATE_RECEIVE_FILE;
    PI_Struct->DataStructure.bytesRemaining = 0;

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
    formatFTPReply(FTPREPLYID_257, reply, PI_Struct->CWD);
}

void _executeListingCommand
(
	const char *arguments,
	bool detailedListing,
	DynamicString *reply,
	FtpPiStruct_t *PI_Struct
)
{
	char fileNameBuff[64];
    DynamicString fileName;
    initializeDynamicString(&fileName, fileNameBuff, sizeof(fileNameBuff));
    // Get the file name from the arguments
    if (parsePrintableString(arguments, &fileName) == NULL)
    {
    	fileNameBuff[0] = '\0';
    }


    if (getFileInfo(PI_Struct->CWD, fileName.buffer, &PI_Struct->DataStructure.fileInfo) == FR_OK)
    {
    	PI_Struct->DataStructure.dtpState = STATE_SEND_LISTING;
    	PI_Struct->DataStructure.detailedListing = detailedListing;
		if (ftp_OpenDataConnection(PI_Struct) != 0)
		{
			// @TODO: An error occured. Reply with a message
		}
		else
		{
		    // Send msg 150 to let the client the listing is on its way
			formatFTPReply(FTPREPLYID_150, reply, fileName.buffer);
		}
    }
    else
    {
    	formatFTPReply(FTPREPLYID_550, reply);
    }

    finalizeDynamicString(&fileName);
}

void executeCommand_LIST
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	bool detailedListing = true;
	_executeListingCommand(arguments, detailedListing, reply, PI_Struct);
}

void executeCommand_NLST
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	bool detailedListing = false;
	UARTPrintLn("NLST CMD Executed");
	_executeListingCommand(arguments, detailedListing, reply, PI_Struct);
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
	// According to http://cr.yp.to/ftp/syst.html, we should always use
	// the following message for the SYST command.
    formatFTPReply(FTPREPLYID_215, reply, "UNIX Type: L8");
}

void executeCommand_STAT
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
    UARTPrintLn("Reading root dir...");

    DynamicString path;
    initializeDynamicString(&path, NULL, 0);
    if (parsePrintableString(arguments, &path) == NULL)
    {
    	formatFTPReply(FTPREPLYID_211, reply, "All is well! Thanks for asking!");
    	return;
    }

    const char *finalPath = resolveRelativeAbsolutePath(PI_Struct->CWD, path.buffer);
    finalizeDynamicString(&path);

    DynamicString directoryContents;
    initializeDynamicString(&directoryContents, NULL, 0);

    size_t bytesWritten = 0;
    readDirectoryContents(finalPath, &directoryContents, &bytesWritten);

    resizeDynamicString(&directoryContents, bytesWritten + 1);
    readDirectoryContents(finalPath, &directoryContents, &bytesWritten);

    UARTPrintLn(directoryContents.buffer);

    formatFTPReply(FTPREPLYID_213, reply, directoryContents.buffer);

    finalizeDynamicString(&directoryContents);
    free(finalPath);

    UARTPrintLn(reply->buffer);
}

void executeCommand_HELP
(
    const char *arguments,
    DynamicString *reply,
    FtpPiStruct_t *PI_Struct
)
{
	const char *HelpMessage = "\
LM3S8962 FTP Server Help\r\n\
\r\n\
This FTP server is running on a TI LM3S8962. It implements\r\n\
a subset of the FTP commands to execute basic FTP operations.\r\n\
The commands implemented are:\r\n\
\r\n\
   USER: used to set the username for the FTP session\r\n\
\r\n\
   PASS: used to send the password for the FTP session\r\n\
\r\n\
   QUIT: used to end the FTP session\r\n\
\r\n\
   PORT: used to send the IP address and port for the data connection.\r\n\
      The syntax is PORT a1,a2,a3,a4,p1,p2.\r\n\
      This is interpreted as IP address a1.a2.a3.a4, port p1*256+p2\r\n\
\r\n\
   TYPE: used to set the format of the data transmission.\r\n\
      The syntax is:\r\n\
      TYPE A for ASCII files\r\n\
      TYPE I for binary files\r\n\
\r\n\
   STRU: used to set the file structure. The only value supported is:\r\n\
      STRU F (file structure).\r\n\
   MODE: used to set the transfer mode. The only value supported is:\r\n\
      MODE S (stream).\r\n\
\r\n\
   RETR: used to request a file from the server.\r\n\
      RETR remote-filename\r\n\
\r\n\
   STOR: used to send a file to the server.\r\n\
      STOR local-filename\r\n\
\r\n\
   STAT: used to display the current state of the server.\r\n\
\r\n\
   HELP: used to display this message.\r\n\
\r\n\
   NOOP: does nothing except return a response.\r\n";

    formatFTPReply(FTPREPLYID_214, reply, HelpMessage);
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
