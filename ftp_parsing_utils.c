#include "ftp_parsing_utils.h"

#include <lwip/src/include/ipv4/lwip/ip_addr.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>

const char * burnWhitespace(const char *arguments)
{
	while (isspace(*arguments++));
	return arguments;
}

const char * parseEOL(const char *arguments)
{
	if (arguments[0] == '\13' && arguments[1] == '\10')
	{
		return arguments + 2;
	}
	else
	{
		return NULL;
	}
}

const char * parseString(const char *arguments, char stringBuffer[], size_t stringBufferLength)
{
	size_t i = 0;

	while
	(
		arguments[i] != '\r' /* carriage return */ &&
		arguments[i] != '\n' /* line feed */ &&
		arguments[i] != ' ' /* space */ &&
		i < stringBufferLength
	)
	{
		stringBuffer[i] = arguments[i];
		i++;
	}

	if (i > 0 && i < stringBufferLength)
	{
		return arguments + i;
	}
	else
	{
		return NULL;
	}
}

const char * parsePrintableString(const char *arguments, char printableStringBuffer[], size_t printableStringBufferLength)
{
	size_t i = 0;

	while
	(
		arguments[i] <= (char) 126 /* ~ */ &&
		arguments[i] >= (char) 33 /* ! */ &&
		i < printableStringBufferLength
	)
	{
		printableStringBuffer[i] = arguments[i];
	}

	if (i > 0 && i < printableStringBufferLength)
	{
		return arguments + i;
	}
	else
	{
		return NULL;
	}
}

const char * parseByteSize(const char *arguments, uint8_t *byteSize)
{
	size_t bytesScanned = 0;
	if (sscanf(arguments, "%hhu%n", byteSize, &bytesScanned) == 1)
	{
		return arguments + bytesScanned;
	}
	else
	{
		return NULL;
	}
}

const char * parseHostPort(const char *arguments, struct ip_addr *hostNumber, uint16_t *portNumber)
{
	uint8_t *hostNumberAsByteArray = (uint8_t *) &hostNumber->addr;
	uint8_t *portNumberAsByteArray = (uint8_t *) portNumber;

	// @TODO: Depending on the endianness of the system, we may need to change the order
	// in which we index the byte arrays below.
	size_t bytesScanned = 0;
	if 
	(
		sscanf
		(
			arguments, 
			"%hhu,%hhu,%hhu,%hhu,%hhu,%hhu%n",
			&hostNumberAsByteArray[0],
			&hostNumberAsByteArray[1],
			&hostNumberAsByteArray[2],
			&hostNumberAsByteArray[3],
			&portNumberAsByteArray[0],
			&portNumberAsByteArray[1],
			&bytesScanned
		) == 4
	)
	{
		return arguments + bytesScanned;
	}
	else
	{
		return NULL;
	}
}

const char * parseDecimalInteger(const char *arguments, uint32_t *decimalInteger)
{
	size_t bytesScanned = 0;
	if (sscanf(arguments, "%lu%n", decimalInteger, &bytesScanned) == 1)
	{
		return arguments + bytesScanned;
	}
	else
	{
		return NULL;
	}
}

const char * parseFormCode(const char *arguments, FormCode *formCode)
{
	switch (arguments[0])
	{
		case 'N':
			*formCode = FORMCODE_N;
			break;
			
		case 'T':
			*formCode = FORMCODE_T;
			break;
			
		case 'C':
			*formCode = FORMCODE_C;
			break;
			
		default:
			return NULL;
	}
	
	return arguments + 1;	
}

const char * parseTypeCode(const char *arguments, TypeCode *typeCode)
{
	switch (arguments[0])
	{
		case 'A':
			*typeCode = TYPECODE_A;
			break;
			
		case 'E':
			*typeCode = TYPECODE_E;
			break;
			
		case 'I':
			*typeCode = TYPECODE_I;
			break;
			
		case 'L':
			*typeCode = TYPECODE_L;
			break;
			
		default:
			return NULL;
	}
	
	return arguments + 1;
}

const char * parseStructureCode(const char *arguments, StructureCode *structureCode)
{
	switch (arguments[0])
	{
		case 'F':
			*structureCode = STRUCTURECODE_F;
			break;
			
		case 'R':
			*structureCode = STRUCTURECODE_R;
			break;
			
		case 'P':
			*structureCode = STRUCTURECODE_P;
			break;
			
		default:
			return NULL;
	}
	
	return arguments + 1;
}

const char * parseModeCode(const char *arguments, ModeCode *modeCode)
{
	switch (arguments[0])
	{
		case 'S':
			*modeCode = MODECODE_S;
			break;
			
		case 'B':
			*modeCode = MODECODE_B;
			break;
			
		case 'C':
			*modeCode = MODECODE_C;
			break;
			
		default:
			return NULL;
	}
	
	return arguments + 1;
}
