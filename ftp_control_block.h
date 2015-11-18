/*
 * ftp_control_block.h
 *
 *  Created on: Nov 3, 2015
 *      Author: Jose Luis Loyola
 */

#ifndef FTP_CONTROL_BLOCK_H_
#define FTP_CONTROL_BLOCK_H_
#include <stdlib.h>
#include <stdint.h>
#include "fatfs/src/FF.h"
#include <lwip/src/include/ipv4/lwip/ip_addr.h>

typedef struct
{
	struct ip_addr hostNumber;
	uint16_t portNumber;
} HostPort;


typedef enum
{
	TYPECODE_UNKNOWN,
	TYPECODE_A,
	TYPECODE_E,
	TYPECODE_I,
	TYPECODE_L
} TypeCode;

typedef enum
{
	STRUCTURECODE_UNKNOWN,
	STRUCTURECODE_F,
	STRUCTURECODE_R,
	STRUCTURECODE_P,
} StructureCode;

typedef enum
{
	MODECODE_UNKNOWN,
	MODECODE_S,
	MODECODE_B,
	MODECODE_C
} ModeCode;

// This enum defines the possible states of the FTP state machine
typedef enum {
    WAIT_FOR_USERNAME,
    WAIT_FOR_PASSWORD,
    READY,
    DATA_CONN_OPEN,
	QUIT
} FtpPiState_t;

// This enum defines the possible states of the DataConnection
typedef enum {
    DATA_CLOSED,
    TX_FILE,
	TX_DIR,
    RX_FILE,
    ABORTED
} DTP_State_t;

// This enum defines the possible data sources for data TX/RX transactions
typedef enum {
    FromFile,
    FromInternalBuff
} Source_Type;

// This is the structure used to keep track of the state of the
// data connection.
typedef struct FTP_DTP_CB{
    FIL file;
    char *buffer;
    int bytesRemaining;
    Source_Type sType;
    DTP_State_t DtpState;
} FTP_DTP_CB;

// This is the structure used to keep track of the state of the
// protocol interpreter.
typedef struct FtpPiStruct_t{
	FtpPiState_t PresState;
	struct tcp_pcb *MessageConnection;
    struct tcp_pcb *DataConnection;
    FTP_DTP_CB DataStructure;
    HostPort hostPort;
    TypeCode typeCode;
    StructureCode structCode;
    ModeCode modeCode;
} FtpPiStruct_t;


#endif /* FTP_CONTROL_BLOCK_H_ */
