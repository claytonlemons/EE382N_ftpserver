#include <stdlib.h>
#include <stdint.h>
#include "ftp_control_block.h"

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
