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

const char * burnWhitespace(const char *arguments);
const char * parseEOL(const char *arguments);
const char * parseString(const char *arguments, char stringBuffer[], size_t stringBufferLength);
const char * parsePrintableString(const char *arguments, char printableStringBuffer[], size_t printableStringBufferLength);
const char * parseByteSize(const char *arguments, uint8_t *byteSize);
const char * parseHostNumber(const char *arguments, HostNumber *hostNumber);
const char * parsePortNumber(const char *arguments, PortNumber *portNumber);
const char * parseHostPort(const char *arguments, HostPort *hostPort);
const char * parseDecimalInteger(const char *arguments, uint32_t *decimalInteger);
const char * parseFormCode(const char *arguments, FormCode *formCode);
const char * parseTypeCode(const char *arguments, TypeCode *typeCode);
const char * parseStructureCode(const char *arguments, StructureCode *structureCode);
const char * parseModeCode(const char *arguments, ModeCode *modeCode);
