#include "ftp_control_block.h"
#include "dynamic_string.h"
#include <lwip/src/include/ipv4/lwip/ip_addr.h>
#include <stdlib.h>
#include <stdint.h>

#define FTP_PARSE(function) 					\
	arguments = parse##function; 				\
	if (arguments == NULL) 						\
	{ 											\
		formatFTPReply(FTPREPLYID_501, reply);	\
		return;									\
	}											\


const char * burnWhitespace(const char *arguments);
const char * parseEOL(const char *arguments);
const char * parseString(const char *arguments, DynamicString *stringBuffer);
const char * parsePrintableString(const char *arguments, DynamicString *stringBuffer);
const char * parseByteSize(const char *arguments, uint8_t *byteSize);
const char * parseHostPort(const char *arguments, HostPort *hostPort);
const char * parseDecimalInteger(const char *arguments, uint32_t *decimalInteger);
const char * parseFormCode(const char *arguments, FormCode *formCode);
const char * parseTypeCode(const char *arguments, TypeCode *typeCode);
const char * parseStructureCode(const char *arguments, StructureCode *structureCode);
const char * parseModeCode(const char *arguments, ModeCode *modeCode);
