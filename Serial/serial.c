#include <time.h>
#include "serial.h"

/*
 * Function: Serial_ShowError
 * Shows an error message both on stderr and on an debugger which reads the OutputDebugString function.
 *
 * Parameters:
 * lpszFunction - Pointer to the error message.
 *
 * Returns:
 * void.
 */
void Serial_ShowError(LPSTR lpszFunction)
{
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	if (dw != 0)
	{
		char szBuf[80];
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
		sprintf(szBuf, "%s failed\nGetLastError: %s", lpszFunction, (LPCTSTR)lpMsgBuf);
		fprintf(stderr, "Error: %s\n", szBuf);
		OutputDebugString(szBuf);
		ExitProcess(dw);
	}
}

/*
 * Function: Serial_OpenPort
 * Opens serial port.
 *
 * Parameters:
 * nComPort - Serial port to open 1 - 255.
 * nBaudRate - Baud rate for the serial port.
 * nDatabits - Number of data bits.
 * nParity - Parity to use.
 * nStopbits - Number of stop bits.
 * nHandshake - Type of handshaking to use.
 *
 * Returns:
 * Returns HANDLE or NULL on error.
 *
 */
HANDLE Serial_OpenPort(unsigned char nComPort, DWORD nBaudRate, unsigned char nDatabits, unsigned char nParity, unsigned char nStopbits, unsigned char nHandshake)
{
	char szComPort[20];
	COMMTIMEOUTS ct;
	DCB dcbCommPort;
	HANDLE Serial_Handle;

	// Check com-port data
	if (nComPort != 0)
	{
		sprintf(szComPort, "\\\\.\\COM%d", nComPort);
	}
	else
	{
		return INVALID_HANDLE_VALUE;
	}

	Serial_Handle = CreateFile(szComPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (Serial_Handle == INVALID_HANDLE_VALUE)
	{
		Serial_ShowError("CreateFile");
		return INVALID_HANDLE_VALUE;
	}

	memset(&ct, 0, sizeof(ct));

	// Set timeouts
	ct.ReadIntervalTimeout = MAXDWORD;
	ct.ReadTotalTimeoutMultiplier = MAXDWORD;
	ct.ReadTotalTimeoutConstant = 200;
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.WriteTotalTimeoutConstant = 200;
	SetCommTimeouts(Serial_Handle, &ct);

	// Comm state
	if (!GetCommState(Serial_Handle, &dcbCommPort))
	{
		CloseHandle(Serial_Handle);
		return INVALID_HANDLE_VALUE;
	}

	dcbCommPort.DCBlength = sizeof(DCB);
	dcbCommPort.BaudRate = nBaudRate;
	if (nParity != NOPARITY)
	{
		dcbCommPort.fParity = TRUE;
	}
	else
	{
		dcbCommPort.fParity = FALSE;
	}

	dcbCommPort.Parity = nParity;
	dcbCommPort.fDtrControl = DTR_CONTROL_ENABLE;
	dcbCommPort.ByteSize = nDatabits;
	dcbCommPort.StopBits = nStopbits;

	if (HANDSHAKE_HARDWARE == nHandshake)
	{
		// Software flow control
		dcbCommPort.fOutX = FALSE;
		dcbCommPort.fInX = FALSE;

		// Hardware flow control
		dcbCommPort.fOutxCtsFlow = TRUE;
		dcbCommPort.fOutxDsrFlow = TRUE;
	}
	else if (HANDSHAKE_SOFTWARE == nHandshake)
	{
		// Software flow control
		dcbCommPort.fOutX = TRUE;
		dcbCommPort.fInX = TRUE;

		// Hardware flow control
		dcbCommPort.fOutxCtsFlow = FALSE;
		dcbCommPort.fOutxDsrFlow = FALSE;
	}
	else
	{
		//  HANDSHAKE_NONE

		// Software flow control
		dcbCommPort.fOutX = FALSE;
		dcbCommPort.fInX = FALSE;

		// Hardware flow control
		dcbCommPort.fOutxCtsFlow = FALSE;
		dcbCommPort.fOutxDsrFlow = FALSE;
	}

	if (!SetCommState(Serial_Handle, &dcbCommPort))
	{
		CloseHandle(Serial_Handle);
		return INVALID_HANDLE_VALUE;
	}

	return Serial_Handle;
}

/*
 * Function: Serial_ClosePort
 * Closes a serial port opened with the OpenPort function.
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port
 *
 * Returns:
 * void.
 */
void Serial_ClosePort(HANDLE Serial_Handle)
{
	PurgeComm(Serial_Handle, PURGE_TXCLEAR | PURGE_RXCLEAR);
	CloseHandle(Serial_Handle);

	Serial_Handle = INVALID_HANDLE_VALUE;
}

/*
 * Function: Serial_PutByte
 * Places a string in the serial output buffer
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port.
 * c - Character to place in the serial output buffer.
 *
 * Returns:
 * void.
 */
void Serial_PutByte(HANDLE Serial_Handle, unsigned char c)
{
	DWORD cnt;
	WriteFile(Serial_Handle, &c, 1, &cnt, NULL);
}

/*
 * Function: Serial_PutString
 * Places a string in the serial output buffer.
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port.
 * c - Pointer to string to send to the serial port.
 *
 * Returns:
 * void.
 */
void Serial_PutString(HANDLE Serial_Handle, char *cstring)
{
	DWORD cnt;
	WriteFile(Serial_Handle, cstring, strlen(cstring), &cnt, NULL);
}

/*
 * Function: Serial_GetByte
 * Reads a character from the serial port
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port.
 * c - Pointer to unsigned char to receive the read character
 *
 * Returns:
 * (int ) - 0 = Nothing read
 *			1 = Byte read
 *
 */
int Serial_GetByte(HANDLE Serial_Handle, unsigned char *c)
{
	DWORD bytesread = 0;
	if (ReadFile(Serial_Handle, c, 1, &bytesread, NULL) == 0)
	{
		Serial_ShowError("Serial_GetByte");
	}
	return bytesread;
}

/*
 * Function: Serial_Flush
 * Clears the serial input and output buffers
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port
 *
 * Returns:
 * void.
 */
void Serial_Flush(HANDLE Serial_Handle)
{
	PurgeComm(Serial_Handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

/*
 * Function: Serial_FlushRX
 * Clears the serial input buffer
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port
 *
 * Returns:
 * void.
 */
void Serial_FlushRX(HANDLE Serial_Handle)
{
	PurgeComm(Serial_Handle, PURGE_RXCLEAR);
}

/*
 * Function: Serial_FlushTX
 * Clears the serial transmit buffer
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port.
 *
 * Returns:
 * void.
 */
void Serial_FlushTX(HANDLE Serial_Handle)
{
	PurgeComm(Serial_Handle, PURGE_TXCLEAR);
}

/*
 * Function: Serial_Break
 * Put the serial port into a break state
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port.
 *
 * Returns:
 * void.
 */
void Serial_Break(HANDLE Serial_Handle)
{
	SetCommBreak(Serial_Handle);
}

/*
 * Function: Serial_ClearBreak
 * Clear the serial break state
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port.
 *
 * Returns:
 * void.
 */
void Serial_ClearBreak(HANDLE Serial_Handle)
{
	ClearCommBreak(Serial_Handle);
}

/*
 * Function: Serial_ClearDTR
 * Clear the DTR Line
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port.
 *
 * Returns:
 * void.
 */
void Serial_ClearDTR(HANDLE Serial_Handle)
{
	EscapeCommFunction(Serial_Handle, CLRDTR);
}

/*
 * Function: Serial_SetDTR
 * Set the DTR Line
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port.
 *
 * Returns:
 * void.
 */
void Serial_SetDTR(HANDLE Serial_Handle)
{
	EscapeCommFunction(Serial_Handle, SETDTR);
}

/*
 * Function: Serial_SetRTS
 * Set the RTS Line
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port.
 *
 * Returns:
 * void.
 */
void Serial_SetRTS(HANDLE Serial_Handle)
{
	EscapeCommFunction(Serial_Handle, SETRTS);
}

/*
 * Function: Serial_ClearRTS
 * Clear the RTS Line
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port.
 *
 * Returns:
 * void.
 */
void Serial_ClearRTS(HANDLE Serial_Handle)
{
	EscapeCommFunction(Serial_Handle, CLRRTS);
}

/*
 * Function: Serial_GetString
 * Reads a string from the serial port
 *
 * Parameters:
 * Serial_Handle - Handle to the serial port.
 * Buffer - Pointer to buffer to receive the read string.
 * BufSize - Length of the buffer for the returned string.
 *
 * Returns:
 * (int) - The number of characters.
 *
 */
int Serial_GetString(HANDLE Serial_Handle, char *Buffer, int BufSize)
{
	char c = 0;
	int charsread = 0;
	DWORD bytesread = 0;
	double elapsed = 0.0;
	clock_t start;

	// Clear the buffer contents
	memset(Buffer, 0, BufSize);

	start = clock();
	for (;;)
	{
		elapsed = ((double)(clock() - start)) / CLOCKS_PER_SEC;

		if (!ReadFile(Serial_Handle, &c, 1, &bytesread, NULL))
		{
			Serial_ShowError("Serial_GetString");
			break;
		}

		if (bytesread != 0)
		{
			// Break out if a carriage return or line feed is found.
			if (c == 10 || c == 13 || elapsed > 5)
			{
				//printf("%s\n",Buffer);
				break;
			}

			// Break out if the buffer is filled.
			if (charsread >= (BufSize - 1))
				break;

			// Add the read char to the buffer
			*Buffer = c;
			Buffer++;

			// Increment the number of characters read
			charsread++;
		}

		if (elapsed > 5)
		{
			break;
		}
	}
	Buffer[charsread] = '\0';

	return charsread;
}

/*
 * Function: Serial_PortExists
 * Checks to see if a comport exists.
 *
 * Parameters:
 * ComPort - Port number to check.
 *
 * Returns:
 * (int) - non zero port exists.
 *
 */
int Serial_PortExists(unsigned char ComPort)
{
	char szComPort[20] = {'\0'};
	HANDLE result = 0;
	int value = 0;

	sprintf(szComPort, "\\\\.\\COM%d", ComPort);

	result = CreateFile(szComPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	CloseHandle(result);
	if (result != INVALID_HANDLE_VALUE)
		value = 1;
	else
		value = 0;
	return (value);
}
