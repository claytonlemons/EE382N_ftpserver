/*
 * dynamic_string.cpp
 *
 *  Created on: Nov 10, 2015
 *      Author: Clayton
 */

#include "dynamic_string.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void initializeDynamicString(DynamicString *dynamicString, char *buffer, size_t length)
{
	dynamicString->buffer = buffer;
	dynamicString->length = length;
	dynamicString->resized = false;
}

bool resizeDynamicString(DynamicString *dynamicString, size_t newLength)
{
	char *tempBuffer = (char *) malloc(newLength);
	if (tempBuffer != NULL)
	{
		finalizeDynamicString(dynamicString);
		initializeDynamicString(dynamicString, tempBuffer, newLength);
		dynamicString->resized = true;
		return true;
	}
	else // We ran of out memory!
	{
		return false;
	}

}

void finalizeDynamicString(DynamicString *dynamicString)
{
	if (dynamicString->resized == true)
	{
		free(dynamicString->buffer);
	}

	initializeDynamicString(dynamicString, NULL, 0);
}
