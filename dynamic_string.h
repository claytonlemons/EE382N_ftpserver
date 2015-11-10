/*
 * dynamic_string.h
 *
 *  Created on: Nov 10, 2015
 *      Author: Clayton
 */

#ifndef DYNAMIC_STRING_H_
#define DYNAMIC_STRING_H_

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
	char *buffer;
	size_t length;
	bool resized;

} DynamicString;

void initializeDynamicString(DynamicString *dynamicString, char *buffer, size_t length);

bool resizeDynamicString(DynamicString *dynamicString, size_t newLength);

void finalizeDynamicString(DynamicString *dynamicString);


#endif /* DYNAMIC_STRING_H_ */
