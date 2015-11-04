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
#include "ftp_control_block.h"
#include "ftp_command_executor.h"
#include "FtpProtocolInterpreter.h"
#include "utils/lwiplib.h"
#include "drivers/rit128x96x4.h"



// This method is used to transmit messages to the FTP client.
// The messages are sent through the TCP pcb module using the
// tcp_write.
static void ftp_SendMsg(struct tcp_pcb *pcb, char *msg)
{
	char Buffer[128];
    sprintf(Buffer, (const char *)msg);
    int MsgLength;
    MsgLength = strlen(Buffer);
    err_t err;

    // Check how much space is available on the TCP TX buffer
    if (tcp_sndbuf(pcb) < MsgLength) {
        RIT128x96x4Enable(1000000);
        RIT128x96x4StringDraw("ftp_SendMsg PCB<Msg!", 0, 80, 15);
        RIT128x96x4Disable();
        return;
    }
    // Send the message to the TCP module
	err = tcp_write(pcb, (const void *)Buffer, MsgLength, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        RIT128x96x4Enable(1000000);
        RIT128x96x4StringDraw("ftp_SendMsg writeError!", 0, 80, 15);
        RIT128x96x4Disable();
        return;
    }
}

// This method is used to open a TCP data connection.
// TODO: Need to define ftp_DataConnected. The function that will
// be called when the data connection is opened.
static err_t ftp_OpenDataConnection(struct tcp_pcb *pcb,
    FtpPiStruct_t *PI_Struct){

    err_t errStatus;
    // Open a data connection on the received FtpPiStruct_t structure
    PI_Struct->DataConnection = tcp_new();
    // Bind the data connection to port 20 of the server's IP
    tcp_bind(PI_Struct->DataConnection, &pcb->local_ip, 20);
    //TODO: Need:
    // 1. A way to convert our hostNumber to a struct ip_addr.
    // 2. A way to convert our portNumber type to a u16_t.
    // 3. A function to process data requests.
    /*errStatus = tcp_connect(
   	    PI_Struct->DataConnection,
        &PI_Struct->hostPort->hostNumber,
		(u16_t)PI_Struct->hostPort->portNumber,
	    ftp_DataConnected);*/
    if (errStatus == ERR_OK)
        PI_Struct->PresState = DATA_CONN_OPEN;
    return errStatus;
}

// This method is used to close a data connection
static err_t ftp_CloseDataConnection (FtpPiStruct_t *PI_Struct){
    return tcp_close(PI_Struct->DataConnection);
}


// This method is used to take action when the TCP module receives data for us.
static err_t ftp_RxCmd(void *arg, struct tcp_pcb *pcb, struct pbuf *p,
    err_t err)
{
    RIT128x96x4Enable(1000000);
    RIT128x96x4StringDraw("ftp_RxCmd Called!", 0, 80, 15);
    RIT128x96x4Disable();

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

        // RIT128x96x4Enable(1000000);
        // RIT128x96x4StringDraw((const char *)RxData, 0, 80, 15);
        // RIT128x96x4Disable();

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
        char * MessageToSend = NULL;
        // Execute the command
        // TODO: The executeCommand function needs to receive a FtpPiStruct_t *
        //executeCommand(ReceivedCommand, Payload, MessageToSend, PI_Struct);
        executeCommand(ReceivedCommand, Payload, MessageToSend);
        // Send the response after executing the command
        ftp_SendMsg(pcb, MessageToSend);

        RIT128x96x4Enable(1000000);
        RIT128x96x4StringDraw((const char *)CommandStr, 0, 60, 15);
        RIT128x96x4StringDraw((const char *)Payload, 0, 80, 15);
        RIT128x96x4Disable();

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
        RIT128x96x4Enable(1000000);
        RIT128x96x4StringDraw("ftp_CmdSent Called!", 0, 80, 15);
        RIT128x96x4Disable();
		return ERR_OK;
    }

	return ERR_OK;
}

// TODO: Need to implement ftp_Poll. We should check the state of the
// data transfer process and execute the appropriate command (file list,
// send file, etc. We may or may not need this...
static err_t ftp_Poll(void *arg, struct tcp_pcb *pcb)
{

    // RIT128x96x4Enable(1000000);
    // RIT128x96x4StringDraw("ftp_Poll Called!", 0, 80, 15);
    // RIT128x96x4Disable();

	return ERR_OK;
}

// This method gets called by the TCP module when an error occurs.
// TODO:We need an ftp__err function to handle errors.
static void ftp_PiError(void *arg, err_t err)
{
	//  FtpPiStruct_t *PI_Struct = arg;
    RIT128x96x4Enable(1000000);
    RIT128x96x4StringDraw("ftp_PiError Called!", 0, 80, 15);
    RIT128x96x4Disable();
}

// This method is used when the TCP module receives a new connection.
// Here we initialize the PI state machine to handle the requests from
// the user. Additionally we present all the required methods to the TCP
// module.
static err_t ftp_Accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    RIT128x96x4Enable(1000000);
    RIT128x96x4StringDraw("FTP_Accept Called!", 0, 80, 15);
    RIT128x96x4Disable();

    // Tell TCP that this is the structure we wish to be passed for our
    // callbacks.
    // Used to specify the argument that should be passed callback
    // functions. This is a TcpPi struct that contains the state of the
    // FTP protocol interpreter state machine.
    FtpPiStruct_t *PI_State = NULL;
    PI_State->PresState = WAIT_FOR_USERNAME;
    tcp_arg(pcb, PI_State);

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
    ftp_SendMsg(pcb, msg220);
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
