#include <windows.h>
#include <stdio.h>

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
	// The name of pipe
	LPSTR  pipeName = "\\\\.\\pipe\\$MyPipe$";
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
    int     maxChanges = 0;
    // Current number of changes in file
    int     changes = 0;
    // Library name and function in the library name, just for fun
    char    libName[] = LIB_NAME,
            funcName[] = FUNC_NAME;

    // Trying to connect to the pipe
    hPipe = CreateFile(pipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	
    // If cannot connect to the pipe
	if (hPipe == INVALID_HANDLE_VALUE) {
		printf("[CHILD PID:%Lu] Cannot connect to pipe <<%s>> (ERROR #%Lu).\n", GetCurrentProcessId(), pipeName, GetLastError());
        return -1;
	}

    // If connected to the pipe
    printf("[CHILD PID:%Lu] Connected to pipe <<%s>> successfully.\n", GetCurrentProcessId(), pipeName);
    
    // Check the library
    hLib = LoadLibrary(libName);
    if (hLib == NULL) {
		printf("[CHILD PID:%Lu] Cannot load library <<%s>>.\n", GetCurrentProcessId(), libName);
		CloseHandle(hPipe);
        return(-2);
	}
    printf("[CHILD PID:%Lu] Library <<%s>> is loaded successfully.\n", GetCurrentProcessId(), libName);

    // Check the function in library
	bufferProcessing = (int(*)(CHAR*, int))GetProcAddress(hLib, funcName);
	if (bufferProcessing == NULL) {
		printf("[CHILD PID:%Lu] Cannot find function <<%s>> in library <<%s>>.\n", GetCurrentProcessId(), funcName, libName);
        CloseHandle(hPipe);
        return(-3);
	}
    printf("[CHILD PID:%Lu] Function <<%s>> in the library found.\n", GetCurrentProcessId(), funcName, libName);

    // Read the pipe
    ZeroMemory(input, BUF_SIZE);
    ReadFile(hPipe, input, BUF_SIZE, &cbRead, NULL);
    
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
        printf("[CHILD PID:%Lu] Incorrect data from pipe.\n", GetCurrentProcessId());
        CloseHandle(hPipe);
        return(-4);
    } else printf("[CHILD PID:%Lu] Read data from pipe <<%s>>.\n", GetCurrentProcessId(), input);

    // Try to open input file (READ MODE)
    hIn = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); 
    if (hIn == INVALID_HANDLE_VALUE) {
        printf("[CHILD PID:%Lu] Cannot open input file <<%s>> (ERROR #%Lu).\n", GetCurrentProcessId(),  fileName, GetLastError());
        CloseHandle(hPipe);
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
        CloseHandle(hPipe);
        return(-5);
    }

    // File reading-writing 
    ZeroMemory(Buffer, BUF_SIZE);
    while(ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) { 
        changes = (*bufferProcessing)(Buffer, maxChanges);
        WriteFile(hOut, Buffer, nIn, &nOut, NULL); 
        if (nIn != nOut) {
            printf("[CHILD PID:%Lu] Writing file error %x.\n", GetCurrentProcessId(), GetLastError());
            CloseHandle(hPipe);
            return(-6);
        }
        ZeroMemory(Buffer, BUF_SIZE);
    }
    printf("[CHILD PID:%Lu] File proccess done. All changes written in file <<%s>>.\n", GetCurrentProcessId(),fileOutName);

    // Close all handles and return the number of changes in file
    FreeLibrary(hLib);
    CloseHandle(hIn); 
    CloseHandle(hOut);
    CloseHandle(hPipe);
    return changes;
}    