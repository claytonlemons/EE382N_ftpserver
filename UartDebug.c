/*
 * UartDebug.c
 *
 *  Created on: Nov 10, 2015
 *      Author: Jose Luis Loyola
 */
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include <string.h>

// Use this method to send a string to the UART Port A. The main purpose of
// this function is to transmit data for debugging purposes.

void UARTPrintUsingLength(const char *string, size_t length)
{
	while (length--)
	{
		UARTCharPut(UART0_BASE, *string++);
	}
}

void UARTPrint(const char *string)
{
	UARTPrintUsingLength(string, strlen(string));
}

void UARTPrintLn(const char *string)
{
	UARTPrint(string);
	UARTPrint("\r\n");
}
