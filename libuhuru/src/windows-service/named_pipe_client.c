#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <libuhuru-config.h>
#include <libuhuru/core.h>
#include <libuhuru/ipc.h>
#include "named_pipe_client.h"

#define BUFSIZE 512
#define MAX_NAMED_PIPE_SIZE 18

int connect_to_IHM(int scan_id, HANDLE * hPipe)
{
	// if win32
	char * server_path = (char*)malloc((MAX_NAMED_PIPE_SIZE+1)*sizeof(char));
	sprintf(server_path, "\\\\.\\pipe\\IHM_scan_%d", scan_id ); 

	// TODO : free(server_path) ??
	return start_named_pipe_client(server_path, hPipe);
}

int start_named_pipe_client(char* path, HANDLE * hPipe)
{
	BOOL   fSuccess = FALSE;
	DWORD  dwMode;
	// HANDLE _hPipe;
	LPTSTR lpszPipename = "\\\\.\\pipe\\IHM_scan_77";

	// Try to open a named pipe; wait for it, if necessary. 
	while (1)
	{
		*hPipe = CreateFile(
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

		// Break if the pipe handle is valid. 
		if (*hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(lpszPipename, 20000))
		{
			printf("Could not open pipe: 20 second wait timed out.");
			return -1;
		}
	}

	// The pipe connected; change to message-read mode. 
	printf("SetNamedPipeHandleState : -%s- ", lpszPipename);
	
	// This is needed to change from byte-mode to readmode client. 
	// Maybe it doesn't works because it's already in READMODE_MESSAGE

	//dwMode = PIPE_READMODE_MESSAGE;
	dwMode = NULL;
	fSuccess = SetNamedPipeHandleState(
		*hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 

	if (!fSuccess)
	{
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	return 0;
}

// Send a message to the pipe server. 
int send_message_to_IHM(HANDLE * hPipe, char * message, char** server_response)
{

	LPTSTR lpvMessage = "";
	TCHAR  chBuf[BUFSIZE];
	BOOL   fSuccess = FALSE;
	DWORD  cbRead, cbToWrite, cbWritten;

	if (hPipe == NULL){
		printf("Null handle pointer passed to send_message_to_IHM()");
		return -1;
	}

	if (*hPipe == INVALID_HANDLE_VALUE){
		printf("Invalid handle passed to send_message_to_IHM()");
		return -1;
	}

	// TODO: Checker le message avant envoi
	lpvMessage = message;
	cbToWrite = (lstrlen(lpvMessage) + 1)*sizeof(TCHAR);
	_tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, lpvMessage);

	fSuccess = WriteFile(
		*hPipe,                  // pipe handle 
		lpvMessage,             // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess)
	{
		_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	printf("\nMessage sent to server, receiving reply as follows:\n");

	do
	{
		// Read from the pipe. 

		fSuccess = ReadFile(
			*hPipe,    // pipe handle 
			chBuf,    // buffer to receive reply 
			BUFSIZE*sizeof(TCHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

		printf(" Server response : %s \n", chBuf);
		printf(" %d bytes read \n", cbRead);

		*server_response = chBuf;

		printf(" After assign response \n");

		//_tprintf(TEXT("\"%s\"\n"), chBuf);

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
	{
		_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	return 0;

}

int closeConnection_to_IHM( HANDLE * hPipe){

	CloseHandle(*hPipe);
	return 0;
}


