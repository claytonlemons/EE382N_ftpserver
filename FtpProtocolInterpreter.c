/*
 * ProtocolInterpreter.c
 *
 *  Created on: Oct 31, 2015
 *      Author: Jose Luis Loyola
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ftp_commands.h"
#include "ftp_replies.h"
#include "ftp_control_block.h"
#include "ftp_command_executor.h"
#include "ftp_parsing_utils.h"
#include "dynamic_string.h"
#include "UartDebug.h"
#include "utils/lwiplib.h"
#include "drivers/rit128x96x4.h"
#include "UartDebug.h"
#include "sdcard.h"

#define kReplyBufferLength 128

// This method is used to close a message connection
err_t ftp_CloseMessageConnection (FtpPiStruct_t *PI_Struct){
    UARTPrintLn("ftp_CloseMessageConnection called!");
    tcp_arg(PI_Struct->MessageConnection, NULL);
    tcp_sent(PI_Struct->MessageConnection, NULL);
    tcp_recv(PI_Struct->MessageConnection, NULL);
    tcp_close(PI_Struct->MessageConnection);
    free(PI_Struct);
    return ERR_OK;
}

// This method is used to transmit messages to the FTP client.
// The messages are sent through the TCP pcb module using the
// tcp_write.
static void ftp_SendMsg(struct tcp_pcb *pcb, const char *msg, size_t length)
{
    err_t err;

    // Check how much space is available on the TCP TX buffer
    if (tcp_sndbuf(pcb) < length) {
    	UARTPrintLn("ftp_SendMsg PCB < Msg!");
        return;
    }
    // Send the message to the TCP module
	err = tcp_write(pcb, msg, length, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
    	UARTPrintLn("ftp_SendMsg writeError!");
        return;
    }
}

// This method is used to close a data connection
err_t ftp_CloseDataConnection (FtpPiStruct_t *PI_Struct){
    UARTPrintLn("ftp_CloseDataConnection called!");
    tcp_arg(PI_Struct->DataConnection, NULL);
    tcp_sent(PI_Struct->DataConnection, NULL);
    tcp_recv(PI_Struct->DataConnection, NULL);
    return tcp_close(PI_Struct->DataConnection);
}

// This method is used to take action when the TCP Data connection receives
// data for us.
static err_t ftp_RxData(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	//UARTPrint("ftp_RxData Called!\r\n");

    FtpPiStruct_t *PI_Struct = arg;

    // We process the RX data only if no errors occurred and the input buffer is not empty.
    if (err == ERR_OK && p != NULL)
    {
        struct pbuf *q;
        uint8_t *receivedData;
		WORD totalBytesWritten = 0;

        // Loop through all the pbufs that have data.
		for (q = p; q != NULL; q = q->next)
		{
			receivedData = q->payload;

            WORD bytesWritten = 0;
            while (bytesWritten != q->len)
            {
            	FRESULT fresult = writeToFile(&PI_Struct->DataStructure.file, receivedData + bytesWritten, q->len, &bytesWritten);
            	if (fresult != FR_OK)
            	{
            		UARTPrintLn(fresultToString(fresult));
            		return ERR_ABRT; // @TODO: What is the right error to return?
            	}
            }

            totalBytesWritten += bytesWritten;
		}

        // We need to tell the TCP module that the data has been accepted.
        tcp_recved(pcb, totalBytesWritten);

        // Deallocate the input buffer.
        pbuf_free(p);
    }

    // When the input data buffer is empty we have received all the data from
    // the client. We proceed to close the connection
    if (err == ERR_OK && p == NULL) {
        PI_Struct->PresState = READY;
        PI_Struct->DataStructure.DtpState = DATA_CLOSED;
        closeFile(&PI_Struct->DataStructure.file);
        ftp_CloseDataConnection(PI_Struct);

        // Send msg226 when the operation completes.
        char StringBuffer[kReplyBufferLength];
        DynamicString reply;
        initializeDynamicString(&reply, StringBuffer, sizeof(StringBuffer));

        formatFTPReply(FTPREPLYID_226, &reply);
        ftp_SendMsg((PI_Struct->MessageConnection), reply.buffer, strlen(reply.buffer));

        finalizeDynamicString(&reply);
    }

    return ERR_OK;
}


// This method is used to take action when the TCP Data connection receives
// data for us.
static err_t ftp_SendData(struct tcp_pcb *pcb, void *data, size_t length)
{
	err_t err;

	do
	{
		err = tcp_write(pcb, data, length, TCP_WRITE_FLAG_COPY);

		if (err == ERR_MEM)
		{
			length /= 2;
		}
	} while (err == ERR_MEM && length > 1);

	if (ERR_IS_FATAL(err))
	{
		UARTPrintLn("Encountered fatal error during ftp_SendData!");
	}

	return err;
}

// 8.3 filename format + 3 bytes for EOL and NULL :)
#define FILE_INFO_BUFFER_SIZE (8 + 1 + 3 + 3)

static err_t ftp_SendDirectoryListing(struct tcp_pcb *pcb, FtpPiStruct_t *PI_Struct)
{
	FILINFO fileInfo;
	f_readdir(&PI_Struct->DataStructure.directory, &fileInfo);

	if (fileInfo.fname[0] == NULL)
	{
		// @TODO: Refactor this code with the code in ftp_RxData
        PI_Struct->PresState = READY;
        PI_Struct->DataStructure.DtpState = DATA_CLOSED;
        ftp_CloseDataConnection(PI_Struct);

        // Send msg226 when the operation completes.
        char StringBuffer[kReplyBufferLength];
        DynamicString reply;
        initializeDynamicString(&reply, StringBuffer, sizeof(StringBuffer));

        formatFTPReply(FTPREPLYID_226, &reply);
        ftp_SendMsg((PI_Struct->MessageConnection), reply.buffer, strlen(reply.buffer));

        finalizeDynamicString(&reply);

        return ERR_OK;
	}

	char fileInfoBuffer[FILE_INFO_BUFFER_SIZE];

	snprintf
	(
		fileInfoBuffer,
		FILE_INFO_BUFFER_SIZE,
		"%s\r\n",
		fileInfo.fname
	);

	ftp_SendData(pcb, fileInfoBuffer, FILE_INFO_BUFFER_SIZE);

	return ERR_OK;
}

static err_t ftp_SendListing(struct tcp_pcb *pcb, FtpPiStruct_t *PI_Struct)
{
	if (PI_Struct->DataStructure.fileInfo.fattrib & AM_DIR)
	{
		if (openDirectory("/", PI_Struct->DataStructure.fileInfo.fname, &PI_Struct->DataStructure.directory) != FR_OK)
		{
			UARTPrintLn("Error: unable to open directory for sending listing!");
		}

		return ftp_SendDirectoryListing(pcb, PI_Struct);
	}
	else
	{
		// @TODO: Handle sending a listing for a single file here
		return ERR_OK;
	}
}

static err_t ftp_SendFile(struct tcp_pcb *pcb, FtpPiStruct_t *PI_Struct)
{
	err_t err;

	uint16_t bufferLength =
		(tcp_sndbuf(pcb) < PI_Struct->DataStructure.bytesRemaining) ? tcp_sndbuf(pcb) : PI_Struct->DataStructure.bytesRemaining;

	uint16_t numBytesRead = 0;

	uint8_t intermediateDataBuffer[TCP_SND_BUF];
	readFromFile(&PI_Struct->DataStructure.file, intermediateDataBuffer, bufferLength, &numBytesRead);

	if (numBytesRead != bufferLength)
	{
		UARTPrintLn("Unable to read the right number of bytes from the file.");
		return ERR_ABRT;
	}

	err = ftp_SendData(pcb, intermediateDataBuffer, bufferLength);

	PI_Struct->DataStructure.bytesRemaining -= bufferLength;

	return err;
}


// This method gets called when the data connection is done sending a frame
static err_t ftp_DataSent(void *arg, struct tcp_pcb *pcb, u16_t len){
	err_t err;

    FtpPiStruct_t *PI_Struct = arg;
    UARTPrint("ftp_DataSent Called!\r\n");

    switch (PI_Struct->DataStructure.DtpState) {
        case STATE_SEND_LISTING:
        	if (PI_Struct->DataStructure.fileInfo.fattrib & AM_DIR)
        	{
        		ftp_SendDirectoryListing(pcb, PI_Struct);
        	}
        	else
        	{
        		// @TODO: Handle file stuff
        	}
            break;
        case STATE_SEND_FILE:
        	if (PI_Struct->DataStructure.bytesRemaining > 0)
        	{
        		err = ftp_SendFile(pcb, PI_Struct);
        	}
        	else
        	{
        		// @TODO: Refactor this code with the code in ftp_RxData
                PI_Struct->PresState = READY;
                PI_Struct->DataStructure.DtpState = DATA_CLOSED;
                closeFile(&PI_Struct->DataStructure.file);
                ftp_CloseDataConnection(PI_Struct);

                // Send msg226 when the operation completes.
                char StringBuffer[kReplyBufferLength];
                DynamicString reply;
                initializeDynamicString(&reply, StringBuffer, sizeof(StringBuffer));

                formatFTPReply(FTPREPLYID_226, &reply);
                ftp_SendMsg((PI_Struct->MessageConnection), reply.buffer, strlen(reply.buffer));

                finalizeDynamicString(&reply);
        	}

            break;
        default:
            break;
    }

	if (ERR_IS_FATAL(err))
	{
		UARTPrintLn("Encountered fatal error during ftp_DataSent!");
	}

    return err;
}

// This method is called when a TCP data connection is opened.
static err_t ftp_DataConnected(void *arg, struct tcp_pcb *pcb, err_t err){
    FtpPiStruct_t *PI_Struct = arg;
    PI_Struct->PresState = DATA_CONN_OPEN;
    PI_Struct->DataConnection = pcb;
    UARTPrintLn("ftp_DataConnected Called!");

    // TCP will call ftp_RxData when it receives data through this connection
    tcp_recv(pcb, ftp_RxData);

    // TCP will call ftp_DataSent it completes a frame transfer
    tcp_sent(pcb, ftp_DataSent);

    switch (PI_Struct->DataStructure.DtpState) {
        case STATE_SEND_LISTING:
        	return ftp_SendListing(pcb, PI_Struct);

        case STATE_SEND_FILE:
        	return ftp_SendFile(pcb, PI_Struct);

        default:
            return ERR_ARG; // Invalid STATE!
    }
}

// This method is used to open a TCP data connection.
err_t ftp_OpenDataConnection(FtpPiStruct_t *PI_Struct){

    err_t errStatus;
    // Open a data connection on the received FtpPiStruct_t structure
    PI_Struct->DataConnection = tcp_new();
    // Bind the data connection to port 20 of the server's IP
    errStatus = tcp_bind(PI_Struct->DataConnection,
        &PI_Struct->MessageConnection->local_ip, 20);
    // Pass the PI_Struct to TCP
    tcp_arg(PI_Struct->DataConnection, PI_Struct);
    // When the client has not programmed any Address/Port we will use
    // the address of the client and the default port 20
    if (PI_Struct->hostPort.portNumber == 0){
        PI_Struct->hostPort.hostNumber = PI_Struct->MessageConnection->remote_ip;
        PI_Struct->hostPort.portNumber = 20;
    }
    // Call ftp_DataConnected when the connection is established
    errStatus = tcp_connect(PI_Struct->DataConnection,
        &PI_Struct->hostPort.hostNumber,
        PI_Struct->hostPort.portNumber,
        ftp_DataConnected);
    if (errStatus == ERR_OK)
        PI_Struct->PresState = DATA_CONN_OPEN;
    return errStatus;
}


// This method is used to take action when the TCP module receives data for us.
static err_t ftp_RxCmd(void *arg, struct tcp_pcb *pcb, struct pbuf *p,
    err_t err)
{
	UARTPrintLn("ftp_RxCmd Called!");

    FtpPiStruct_t *PI_Struct = arg;

    // We need to tell the TCP module that the data has been accepted.
    tcp_recved(pcb, p->tot_len);

    char *RxData;
    char *CommandStr;
    char *Payload = NULL;

    // We process the RX data only if no errors occurred and the input buffer
    // is not empty.
    if (err == ERR_OK && p != NULL) {
        // Grab the data from the input buffer.
        RxData = p->payload;

        // The first entry is the first character of the command. Set the
        // CommandStr pointer to that location.
        CommandStr = RxData;

        int i;
        // In this loop we split the command from the payload by replacing the
        // ' ' between them with a \0
        for (i=0; i< p->tot_len; i++){
            if(*RxData == ' '){
                // The string of the incoming command ends when we encounter the
                // first space character. Replace the ' ' with a \0 to mark the
                // end of the command.
                *RxData = '\0';
                // The payload of the command starts after the first space
                // character.
                Payload = RxData + 1;
                break;
            }
            // If we receive a command without arguments we will see a \r after
            // the command. We replace the \r with a NULL to process the command
            // properly.
            if(*RxData == '\r'){
                parseEOL(RxData);
                *RxData = '\0';
                break;
            } else{
                RxData++;
            }
        }

        // Convert the command string to an FTPCommandString
        FTPCommandID ReceivedCommand = ftpCommandStringToID(CommandStr);

        // Initialize reply buffer
        char StringBuffer[kReplyBufferLength];
        DynamicString reply;
        initializeDynamicString(&reply, StringBuffer, sizeof(StringBuffer));

        UARTPrintLn(CommandStr);

        // Execute the command
        executeCommand(ReceivedCommand, Payload, &reply, PI_Struct);

        // Send the response after executing the command
        ftp_SendMsg(pcb, reply.buffer, strlen(reply.buffer)); // @TODO: fix the reply buffer length problem

        // Clean up reply buffer
        finalizeDynamicString(&reply);

        // Deallocate the input buffer.
        pbuf_free(p);
    }

    return ERR_OK;
}

// This is the method that gets called when the TCP module successfully
// completes a transmission.
static err_t ftp_CmdSent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    FtpPiStruct_t *PI_Struct = arg;
    UARTPrintLn("ftp_CmdSent Called!");

    // Close the message connection when we receive the quit command
    if((PI_Struct->PresState == QUIT ) &&
       (PI_Struct->DataStructure.DtpState == DATA_CLOSED)){
        ftp_CloseMessageConnection(PI_Struct);
    }

    return ERR_OK;
}

// TODO: Need to implement ftp_Poll. We should check the state of the
// data transfer process and execute the appropriate command (file list,
// send file, etc. We may or may not need this...
static err_t ftp_Poll(void *arg, struct tcp_pcb *pcb)
{
	//UARTPrintLn("ftp_Poll Called!");

    return ERR_OK;
}

// This method gets called by the TCP module when an error occurs.
// TODO:We need an ftp__err function to handle errors.
static void ftp_PiError(void *arg, err_t err)
{
	//  FtpPiStruct_t *PI_Struct = arg;
	UARTPrintLn("ftp_PiError Called!");
}



// This method is used when the TCP module receives a new connection.
// Here we initialize the PI state machine to handle the requests from
// the user. Additionally we present all the required methods to the TCP
// module.
static err_t ftp_Accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);

    UARTPrintLn("FTP_Accept Called!");

    // Tell TCP that this is the structure we wish to be passed for our
    // callbacks.
    // Used to specify the argument that should be passed callback
    // functions. This is a TcpPi struct that contains the state of the
    // FTP protocol interpreter state machine.
    FtpPiStruct_t *PI_Structure;
    PI_Structure = malloc(sizeof(FtpPiStruct_t));
    PI_Structure->PresState = WAIT_FOR_USERNAME;
    PI_Structure->MessageConnection = pcb;
    // Apply default values for new connections
    PI_Structure->hostPort.portNumber = 0;
    PI_Structure->typeCode = TYPECODE_A;
    PI_Structure->structCode = STRUCTURECODE_F;
    PI_Structure->modeCode = MODECODE_S;
    tcp_arg(pcb, PI_Structure);

    // Tell TCP that we wish to be informed of incoming data by a call
    // to the ftp_RxCmd() function.
    // Used to specify the function that should be called when a TCP
    // connection receives data.
    tcp_recv(pcb, ftp_RxCmd);

    // Used to specify the function that should be called when a fatal error
    // has occurred on the connection.
    tcp_err(pcb, ftp_PiError);


    // Tell TCP that we wish to be informed when a command has been successfully
    // sent. The TCP module will call ftp_CmdSent when this happens.
    tcp_sent(pcb, ftp_CmdSent);


    // Used to specify the function that should be called periodically
    // from TCP. The interval is specified in terms of the TCP coarse
    // timer interval, which is called twice a second.
    tcp_poll(pcb, ftp_Poll, 1);

    // Send msg220 to let the user know the server received the request.
    char StringBuffer[kReplyBufferLength];
    DynamicString reply;
    initializeDynamicString(&reply, StringBuffer, sizeof(StringBuffer));
	formatFTPReply(FTPREPLYID_220, &reply);
	ftp_SendMsg(pcb, reply.buffer, strlen(reply.buffer));
    finalizeDynamicString(&reply);
    return ERR_OK;
}


// This method is used to configure the TCP module to listen to an FTP session
void
ftp_Init(void)
{
  struct tcp_pcb *pcb;

  // Create new TCP control block
  pcb = tcp_new();
  // Bind the control block to any IP address and port 21
  tcp_bind(pcb, IP_ADDR_ANY, 21);
  // Tell TCP to listen to this port/control block
  pcb = tcp_listen(pcb);
  // Used to specify the function that should be called when
  // a listening connection is established
  tcp_accept(pcb, ftp_Accept);
}
