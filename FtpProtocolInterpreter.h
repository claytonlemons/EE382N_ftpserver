/*
 * FtpProtocolInterpreter.h
 *
 *  Created on: Oct 31, 2015
 *      Author: Jose Luis Loyola
 */
#include "utils/lwiplib.h"

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


void ftp_Init(void);
static void ftp_SendMsg(struct tcp_pcb *pcb, char *msg);

#endif /* FTPPROTOCOLINTERPRETER_H_ */
