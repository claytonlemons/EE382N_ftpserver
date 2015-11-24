/*
 * debug.c
 *
 *  Created on: Nov 24, 2015
 *      Author: Clayton
 */
#include "driverlib/debug.h"

#include "includes/utils/uartstdio.h"

void __error__(char *pcFilename, unsigned long ulLine)
{
	UARTprintf("FILE: %s; LINE: %d", pcFilename, ulLine);
}
