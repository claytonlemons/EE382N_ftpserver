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
	// err = tcp_write(pcb, (const void *)Buffer, (u16_t)(fsm->fifo.size - i), 1);
    if (err != ERR_OK) {
        RIT128x96x4Enable(1000000);
        RIT128x96x4StringDraw("ftp_SendMsg writeError!", 0, 80, 15);
        RIT128x96x4Disable();
        return;
    }
}

// This method is used to take action when the TCP module receives data for us.
// TODO: Convert the command using ftpCommandStringToID, if the command is
// found execute it. If not, send msg502
static err_t ftp_RxCmd(void *arg, struct tcp_pcb *pcb, struct pbuf *p,
    err_t err)
{
    RIT128x96x4Enable(1000000);
    RIT128x96x4StringDraw("ftp_RxCmd Called!", 0, 80, 15);
    RIT128x96x4Disable();

    // We need to tell the TCP module that the data has been accepted.
    tcp_recved(pcb, p->tot_len);

    char *RxData;
    char *Command;
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
        // Command pointer to that location.
        Command = RxData;

        int i;
        // In this loop we do two things:
        // + Split the command from the payload by inserting a \0.
        // + Replace the \r\n characters with \0
        for (i=0; i< p->tot_len; i++){
            if (*RxData == '\r'){
                *RxData = '\0';
                break;
            } else if(*RxData == ' ' && Payload == NULL){
                // The string of the incoming ends when we encounter the first
                // space character. Replace the ' ' with a \0 to mark the end
                // of the command.
                *RxData = '\0';
                // The payload of the command starts after the first space
                // character.
                Payload = RxData + 1;
            }
            RxData++;
        }

        // if(ftpCommandStringToID(Command) == UNKNOWN_COMMAND){
            // Send an error message when the command is not recognized
            // ftp_SendMsg(pcb, msg502);
        // }
        // else{
            // executeCommand(ftpCommandStringToID(Command), )
            // ftp_SendMsg(pcb, msg331);
        // }
            ftp_SendMsg(pcb, msg331);


        RIT128x96x4Enable(1000000);
        RIT128x96x4StringDraw((const char *)Command, 0, 60, 15);
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
	// struct ftpd_msgstate *fsm = arg;

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

	// if ((sfifo_used(&fsm->fifo) == 0) && (fsm->state == FTPD_QUIT))
		// ftpd_msgclose(pcb, fsm);

    // TODO:Define the state machine for the FTP server.
	// send_msgdata(pcb, fsm);

	return ERR_OK;
}

// TODO: Need to implement ftp_Poll. We should check the state of the
// data transfer process and execute the appropriate command (file list,
// send file, etc.
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
	// struct ftpd_msgstate *fsm = arg;
    RIT128x96x4Enable(1000000);
    RIT128x96x4StringDraw("ftp_PiError Called!", 0, 80, 15);
    RIT128x96x4Disable();

	// dbg_printf("ftpd_msgerr: %s (%i)\n", lwip_strerr(err), err);
	// if (fsm == NULL)
		// return;
	// if (fsm->datafs)
		// ftpd_dataclose(fsm->datapcb, fsm->datafs);
	// sfifo_close(&fsm->fifo);
	// vfs_close(fsm->vfs);
	// fsm->vfs = NULL;
	// if (fsm->renamefrom)
		// free(fsm->renamefrom);
	// fsm->renamefrom = NULL;
	// free(fsm);
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
    // tcp_arg(pcb, hs);

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

    // Send msg220 to let the user know the server is ready.
    ftp_SendMsg(pcb, msg220);
    // Call TCP write with the message to send, size, and API flag of 0.
    // The 0 means the TCP module will not copy the data to be sent.
    // tcp_write(pcb, (const void *)"220 Welcome to LM3S6965 FTP Server\r\n",36,0);
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
