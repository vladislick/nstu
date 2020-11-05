#include <windows.h>
#include <stdio.h>
#include <conio.h>

// Maximum size of strings in the app
#define BUF_SIZE    256
// Name of library (DLL)
#define LIB_NAME    "maindll.dll"
// The name of function in the library
#define FUNC_NAME   "bufferProcessing"

// Library descriptor
HINSTANCE hLib;
// Function prototype from the library
int (*bufferProcessing)(CHAR*, int);

int main() {
	// Pipe descriptor
	HANDLE hPipe;
    // Идентификаторы каналов Mailslot
	HANDLE hMailslot1, hMailslot2;
    // Размер сообщения в байтах
	DWORD  cbMessages;
		// Количество сообщений в канале Mailslot2
	DWORD  cbMsgNumber;
	// Mailshot name
	LPSTR  mailslotName1 = "\\\\.\\mailslot\\$Channel1$";
	// Mailshot name
	LPSTR  mailslotName2 = "\\\\.\\mailslot\\$Channel2$";
	// Number of read/written bytes from the pipe
	DWORD  cbRead, cbWritten;
	// File descriptors (hIn - input file, hOut - output file)
    HANDLE hIn, hOut; 
    // Numbers of read/written bytes
    DWORD nIn, nOut;
    // Buffer for read/write the files
    CHAR Buffer[BUF_SIZE];
    // Message from server app received by the pipe
	char    input[BUF_SIZE] = { 0 };
	// Name of input file
    char    fileName[BUF_SIZE] = { 0 };
    // Name for output file
    char    fileOutName[BUF_SIZE] = { 0 };
	// Maximum of possible changes in file
    // Message for child to say filename and maximum of changes
	char message[BUF_SIZE] = { 0 };

    int     maxChanges = 0;
    // Current number of changes in file
    int     changes = 0;
    // Library name and function in the library name, just for fun
    char    libName[] = LIB_NAME,
            funcName[] = FUNC_NAME;

    hMailslot1 = CreateMailslot(mailslotName1, 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hMailslot1 == INVALID_HANDLE_VALUE)
    {
        fprintf(stdout, "\n[CHILD] Create mailslot 1 failed (ERROR #%ld)\n", GetLastError());
        CloseHandle(hMailslot1);
        _getch();
        return -10;
    }

    hMailslot2 = CreateFile(mailslotName2, GENERIC_WRITE,FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hMailslot2 == INVALID_HANDLE_VALUE)
	{
		fprintf(stdout, "\n[CHILD] Create mailslot 2 failed (ERROR #%ld)\n", GetLastError());
		_getch();
		return -11;
	}

    ZeroMemory(input, BUF_SIZE);
    ReadFile(hMailslot1, input, BUF_SIZE, &cbRead, NULL);

    // Check the library
    hLib = LoadLibrary(libName);
    if (hLib == NULL) {
		printf("[CHILD PID:%Lu] Cannot load library <<%s>>.\n", GetCurrentProcessId(), libName);
        return(-2);
	}
    printf("[CHILD PID:%Lu] Library <<%s>> is loaded successfully.\n", GetCurrentProcessId(), libName);

    // Check the function in library
	bufferProcessing = (int(*)(CHAR*, int))GetProcAddress(hLib, funcName);
	if (bufferProcessing == NULL) {
		printf("[CHILD PID:%Lu] Cannot find function <<%s>> in library <<%s>>.\n", GetCurrentProcessId(), funcName, libName);
        CloseHandle(hMailslot1);
        return(-3);
	}
    printf("[CHILD PID:%Lu] Function <<%s>> in the library found.\n", GetCurrentProcessId(), funcName, libName);
    
    // Get filename and maximum of changes
    strcpy(fileName, input);
    char isFile = 1;
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] == ' ') {
            ZeroMemory(fileName, BUF_SIZE);
            strncpy(fileName, input, i);
            isFile = 0;
        } else if (!isFile && input[i] > 47 && input[i] < 58) {
            if (maxChanges > 0) maxChanges *= 10;
            maxChanges += input[i] - 48;
        }
    }
    if (maxChanges == 0) maxChanges = 99999;

    // Сheck the recognized data
    if (!strcmp(fileName, "")) {
        printf("[CHILD PID:%Lu] Incorrect data from mailshot.\n", GetCurrentProcessId());
        CloseHandle(hMailslot1);
        return(-4);
    } else printf("[CHILD PID:%Lu] Read data from mailshot <<%s>>.\n", GetCurrentProcessId(), input);

    // Try to open input file (READ MODE)
    hIn = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); 
    if (hIn == INVALID_HANDLE_VALUE) {
        printf("[CHILD PID:%Lu] Cannot open input file <<%s>> (ERROR #%Lu).\n", GetCurrentProcessId(),  fileName, GetLastError());
        CloseHandle(hMailslot1);
        return(-4);
    } 
 
    // Get the name of output file 
    ZeroMemory(fileOutName, BUF_SIZE);
    if (strchr(fileName, '.') == NULL) // If input file hasn't file extension
        strcpy(fileOutName, fileName);
    else 
        strncpy(fileOutName, fileName, strchr(fileName, '.') - fileName);
    strcat(fileOutName, ".out");

    // Try to open output file (WRITE MODE)
    hOut = CreateFile (fileOutName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
    if(hOut == INVALID_HANDLE_VALUE) {
        printf("[CHILD PID:%Lu] Cannot open output file <<%s>> (ERROR #%Lu).\n", GetCurrentProcessId(), fileOutName, GetLastError());
        return(-5);
    }

    // File reading-writing 
    ZeroMemory(Buffer, BUF_SIZE);
    while(ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) { 
        changes = (*bufferProcessing)(Buffer, maxChanges);
        WriteFile(hOut, Buffer, nIn, &nOut, NULL); 
        if (nIn != nOut) {
            printf("[CHILD PID:%Lu] Writing file error %x.\n", GetCurrentProcessId(), GetLastError());
            return(-6);
        }
        ZeroMemory(Buffer, BUF_SIZE);
    }
    printf("[CHILD PID:%Lu] File proccess done. All changes written in file <<%s>>.\n", GetCurrentProcessId(),fileOutName);

    ZeroMemory(message, BUF_SIZE);
    sprintf(message, "Hello");
    if (!WriteFile(hMailslot2, message, strlen(message) + 1, &cbWritten, NULL)) printf("[CHILD PID:%Lu] Writing message error %x.\n", GetCurrentProcessId(), GetLastError());

    // Close all handles and return the number of changes in file
    FreeLibrary(hLib);
    CloseHandle(hIn); 
    CloseHandle(hOut);
    return changes;
}    