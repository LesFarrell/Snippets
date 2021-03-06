// MIT License
//
// 	Copyright(c) 2019 Les Farrell
//
// 	Permission is hereby granted,
// 	free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
// 	The above copyright notice and this permission notice shall be included in all copies
// 	or
// 	substantial portions of the Software.
//
// 	THE SOFTWARE IS PROVIDED "AS IS",
// 	WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// 	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// 	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// 	DAMAGES OR OTHER
// 	LIABILITY,
// 	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// 	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// 	SOFTWARE.

#ifndef SERIAL_HEADER
#define SERIAL_HEADER

#include "windows.h"
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctype.h"

#define HANDSHAKE_NONE 0
#define HANDSHAKE_HARDWARE 1
#define HANDSHAKE_SOFTWARE 2

/* #define ONESTOPBIT		0	// 1 stop bit. */
/* #define ONE5STOPBITS		1	// 1.5 stop bits. */
/* #define TWOSTOPBITS		2	// 2 stop bits */

/* Function Prototypes */
HANDLE Serial_OpenPort(unsigned char nComPort, DWORD nBaudRate, unsigned char nDatabits, unsigned char nParity, unsigned char nStopbits, unsigned char nHandshake);
void Serial_ClosePort(HANDLE Serial_Handle);
void Serial_PutByte(HANDLE Serial_Handle, unsigned char c);
void Serial_PutString(HANDLE Serial_Handle, char *c);
int Serial_GetByte(HANDLE Serial_Handle, unsigned char *c);
int Serial_GetString(HANDLE Serial_Handle, char *Buffer, int BufSize);
void Serial_Flush(HANDLE Serial_Handle);
void Serial_FlushRX(HANDLE Serial_Handle);
void Serial_FlushTX(HANDLE Serial_Handle);
void Serial_Break(HANDLE Serial_Handle);
void Serial_ClearBreak(HANDLE Serial_Handle);
void Serial_ClearDTR(HANDLE Serial_Handle);
void Serial_SetDTR(HANDLE Serial_Handle);
void Serial_SetRTS(HANDLE Serial_Handle);
void Serial_ClearRTS(HANDLE Serial_Handle);
int Serial_PortExists(unsigned char ComPort);

// Serial I/O Buffer Sizes
#define SERIAL_IN_BUFFER_SIZE 2048
#define SERIAL_OUT_BUFFER_SIZE 2048

#endif
