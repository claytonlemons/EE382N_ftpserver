#include <lwip/src/include/ipv4/lwip/ip_addr.h>
#include <stdlib.h>
#include <stdint.h>

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
const char * parseHostPort(const char *arguments, struct ip_addr *hostNumber, uint16_t *portNumber);
const char * parseDecimalInteger(const char *arguments, uint32_t *decimalInteger);
const char * parseFormCode(const char *arguments, FormCode *formCode);
const char * parseTypeCode(const char *arguments, TypeCode *typeCode);
const char * parseStructureCode(const char *arguments, StructureCode *structureCode);
const char * parseModeCode(const char *arguments, ModeCode *modeCode);
