/*
 * UartDebug.c
 *
 *  Created on: Nov 10, 2015
 *      Author: Jose Luis Loyola
 */
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

// Use this method to send a string to the UART Port A. The main purpose of
// this function is to transmit data for debugging purposes.
void UARTSendString(const unsigned char *Buff, unsigned long CharsToSend){
    // Loop while there are more characters to send.
    while(CharsToSend--){
        // Send the next character to the UART.
        UARTCharPutNonBlocking(UART0_BASE, *Buff++);
    }
}
