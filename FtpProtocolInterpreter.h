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


void ftp_Init(void);
static void ftp_SendMsg(struct tcp_pcb *pcb, const char *msg, size_t length);
err_t ftp_OpenDataConnection(FtpPiStruct_t *PI_Struct);
err_t ftp_CloseDataConnection(FtpPiStruct_t *PI_Struct);

#endif /* FTPPROTOCOLINTERPRETER_H_ */
