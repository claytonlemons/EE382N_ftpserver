#include "ftp_parsing_utils.h"

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
		arguments[i] != '\13' /* carriage return */ &&
		arguments[i] != '\10' /* line feed */ &&
		arguments[i] != '\32' /* space */ &&
		i < stringBufferLength
	)
	{
		stringBuffer[i] = arguments[i];
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

const char * parseHostNumber(const char *arguments, HostNumber *hostNumber)
{
	size_t bytesScanned = 0;
	if 
	(
		sscanf
		(
			arguments, 
			"%hhu,%hhu,%hhu,%hhu%n", 
			&hostNumber->octet1,
			&hostNumber->octet2,
			&hostNumber->octet3,
			&hostNumber->octet4,
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

const char * parsePortNumber(const char *arguments, PortNumber *portNumber)
{
	size_t bytesScanned = 0;
	if 
	(
		sscanf
		(
			arguments, 
			"%hhu,%hhu%n", 
			&portNumber->highByte,
			&portNumber->lowByte,
			&bytesScanned
		) == 2
	)
	{
		return arguments + bytesScanned;
	}
	else
	{
		return NULL;
	}
}

const char * parseHostPort(const char *arguments, HostPort *hostPort)
{
	arguments = parseHostNumber(arguments, &hostPort->hostNumber);
	if (arguments == NULL)
	{
		return NULL;
	}
	
	if (arguments[0] == ',')
	{
		arguments++;
	}
	else
	{
		return NULL;
	}
	
	arguments = parsePortNumber(arguments, &hostPort->portNumber);
	if (arguments == NULL)
	{
		return NULL;
	}
	
	return arguments;
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
