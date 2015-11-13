/*
 * UartDebug.h
 *
 *  Created on: Nov 10, 2015
 *      Author: Jose Luis Loyola
 */

#ifndef UARTDEBUG_H_
#define UARTDEBUG_H_

void UARTPrintUsingLength(const char *string, size_t length);

void UARTPrint(const char *string);

void UARTPrintLn(const char *string);

#endif /* UARTDEBUG_H_ */
