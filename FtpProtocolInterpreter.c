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
#include "dynamic_string.h"
#include "UartDebug.h"
#include "utils/lwiplib.h"
#include "drivers/rit128x96x4.h"
#include "UartDebug.h"

#define kReplyBufferLength 128

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
static err_t ftp_RxData(void *arg, struct tcp_pcb *pcb, struct pbuf *p,
    err_t err)
{
	UARTPrint("ftp_RxData Called!\r\n");

    FtpPiStruct_t *PI_Struct = arg;



    // We process the RX data only if no errors occurred and the input buffer
    // is not empty.
    if (err == ERR_OK && p != NULL) {
        struct pbuf *q;
		//int totalRxDataLength = 0;
        // Grab the data from the input buffer.
        char *RxData;
        char *NullInserter;

        // Loop through all the pbufs that have data.
		for (q = p; q != NULL; q = q->next) {
            // TODO: here is where we will write to the file system
            // WriteToFileSystem(q->payload, q->len, FileSystemStruct?)
			//totalRxDataLength += q->len;

            // !HACK!This is just for debugging purposes and should be removed
            // after adding the function to write the file system.
            RxData = q->payload;
            NullInserter = RxData + q->len;
            *NullInserter = '\0';
            UARTPrint(RxData);
		}

        // We need to tell the TCP module that the data has been accepted.
        tcp_recved(pcb, p->tot_len);

        // Deallocate the input buffer.
        pbuf_free(p);
    }

    // When the input data buffer is empty we have received all the data from
    // the client. We proceed to close the connection
    if (err == ERR_OK && p == NULL) {
        PI_Struct->PresState = READY;
        ftp_CloseDataConnection(PI_Struct);
        PI_Struct->DataStructure.DtpState = IDLE;
        // Send msg226 when the operation completes.
        char StringBuffer[kReplyBufferLength];
        DynamicString reply;
        initializeDynamicString(&reply, StringBuffer, sizeof(StringBuffer));
        formatFTPReply(FTPREPLYID_226, &reply);
        ftp_SendMsg((PI_Struct->MessageConnection), reply.buffer,
            strlen(reply.buffer));
        finalizeDynamicString(&reply);
    }

    return ERR_OK;
}

// This method gets called when the data connection is done sending a frame
static err_t ftp_DataSent(void *arg, struct tcp_pcb *pcb, u16_t len){
    FtpPiStruct_t *PI_Struct = arg;
    UARTPrint("ftp_DataSent Called!\r\n");
    switch (PI_Struct->DataStructure.DtpState) {
        case TX_DIR:
            //TODO: need tcp_SendData and tcp_SendDir
            break;
        case TX_FILE:
            //TODO: need tcp_SendData and tcp_SendFile
            // tcp_SendData should be used to send small chunk of data.
            // tcp_SendDir should call tcp_SendData. It handles the partiion
            // of the file at a higher level
            break;
        default:
            break;
    }
    return ERR_OK;
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
        case TX_DIR:
            //TODO: need tcp_SendData and tcp_SendDir
            break;
        case TX_FILE:
            //TODO: need tcp_SendData and tcp_SendFile
            break;
        default:
            break;
    }
    return ERR_OK;
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
            RxData++;
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
    //  FtpPiStruct_t *PI_Struct = arg;

    // Check the present state of the TCP state machine. If it is greater than
    // "ESTABLISHED", we simply return. The only states that are < ESTABLISHED
    // are:
    // CLOSED      = 0,
    // LISTEN      = 1,
    // SYN_SENT    = 2,
    // SYN_RCVD    = 3,
	if (pcb->state > ESTABLISHED){
		UARTPrintLn("ftp_CmdSent Called!");
		return ERR_OK;
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
    PI_Structure->hostPort.portNumber = 0;
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
