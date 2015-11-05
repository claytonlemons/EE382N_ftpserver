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
typedef struct
{
	uint8_t octet1;
	uint8_t octet2;
	uint8_t octet3;
	uint8_t octet4;
} HostNumber;

// @TODO: Is it highByte/lowByte really what we should be calling these fields?
typedef struct
{
	uint8_t highByte;
	uint8_t lowByte;
} PortNumber;

// @TODO: No command actually takes a HostNumber or PortNumber by itself.
// Do we really need separate structs and parsing functions for them?
typedef struct
{
	HostNumber hostNumber;
	PortNumber portNumber;
} HostPort;

typedef enum
{
	FORMCODE_UNKNOWN,
	FORMCODE_N,
	FORMCODE_T,
	FORMCODE_C
} FormCode;

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
    DATA_CONN_OPEN
} FtpPiState_t;

// This enum defines the structure used to keep track of the state of the
// protocol interpreter.
typedef struct FtpPiStruct_t{
	FtpPiState_t PresState;
    struct tcp_pcb *DataConnection;
    HostPort hostPort;
    FormCode formCode;
    TypeCode typeCode;
    StructureCode structCode;
} FtpPiStruct_t;


#endif /* FTP_CONTROL_BLOCK_H_ */
