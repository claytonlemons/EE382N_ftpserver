/*
 * FtpProtocolInterpreter.h
 *
 *  Created on: Oct 31, 2015
 *      Author: Jose Luis Loyola
 */
#include "utils/lwiplib.h"
#include "ftp_control_block.h"

#ifndef FTPPROTOCOLINTERPRETER_H_
#define FTPPROTOCOLINTERPRETER_H_

////////////////////////////////////////////////////////////////////////////////
// FTP Messages
////////////////////////////////////////////////////////////////////////////////
#define msg200 "200 Command OK\r\n"
#define msg202 "202 Command not implemented\r\n"
#define msg220 "220 Welcome to LM3S6965 FTP Server\r\n"
#define msg230 "230 User Logged in\r\n"
#define msg331 "331 User name OK, need password\r\n"
#define msg502 "502 Command not implemented\r\n"

#define kReplyBufferLength 128

void ftp_Init(void);
static void ftp_SendMsg(struct tcp_pcb *pcb, const char *msg, size_t length);
static err_t ftp_OpenDataConnection(struct tcp_pcb *pcb,
    FtpPiStruct_t *PI_Struct);
static err_t ftp_CloseDataConnection (FtpPiStruct_t *PI_Struct);

#endif /* FTPPROTOCOLINTERPRETER_H_ */
