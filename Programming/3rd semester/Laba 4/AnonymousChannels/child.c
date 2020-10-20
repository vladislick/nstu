#include <windows.h> 
#include <stdio.h> 

#define BUF_SIZE    256
#define LIB_NAME    "maindll.dll"
#define FUNC_NAME   "bufferProcessing"

HINSTANCE hLib;
int (*bufferProcessing)(CHAR*, int);

int main(void) {
    HANDLE hIn, hOut; 
    DWORD nIn, nOut; 
    DWORD cbWritten;
    CHAR Buffer[BUF_SIZE];
    CHAR FileOut[BUF_SIZE];
    char    libName[] = LIB_NAME,
            funcName[] = FUNC_NAME,
            message[BUF_SIZE],
            filename[BUF_SIZE],
            inputData[BUF_SIZE];
    int Changes = 0, MAX_CHANGES = 0;
    
    // Check the library
    hLib = LoadLibrary(libName);
    if (hLib == NULL) {
		sprintf(message, "[CLIENT PID:%Lu] Cannot load library <<%s>>\n", GetCurrentProcessId(), libName);
        WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message) + 1, &cbWritten, NULL);
		exit(-1);
	}

    // Check the function in library
	bufferProcessing = (int(*)(CHAR*, int))GetProcAddress(hLib, funcName);
	if (bufferProcessing == NULL) {
		sprintf(message, "[CLIENT PID:%Lu] Cannot find function <<%s>> in library <<%s>>\n", GetCurrentProcessId(), funcName, libName);
		WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message) + 1, &cbWritten, NULL);
        exit(-2);
	}

    // Check the input data from channel
    ReadFile(GetStdHandle(STD_INPUT_HANDLE), inputData, BUF_SIZE, &cbWritten, NULL);
    if (cbWritten < 1) {
        sprintf(message, "[CLIENT PID:%Lu] Input data error\n", GetCurrentProcessId());
        WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message) + 1, &cbWritten, NULL);
        exit(-3);
    } 

    // Get filename and maximum of changes
    strcpy(filename, inputData);
    char isFile = 1;
    for (int i = 0; i < strlen(inputData); i++) {
        if (inputData[i] == ' ') {
            ZeroMemory(filename, BUF_SIZE);
            strncpy(filename, inputData, i);
            isFile = 0;
        }
        else if (!isFile && inputData[i] > 47 && inputData[i] < 58) {
            if (MAX_CHANGES > 0) MAX_CHANGES *= 10;
            MAX_CHANGES += inputData[i] - 48;
        }
    }
    if (MAX_CHANGES == 0) MAX_CHANGES = 99999;
    
    // Try to open input file (READ MODE)
    hIn = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); 
    if (hIn == INVALID_HANDLE_VALUE) {
        sprintf(message, "[CLIENT PID:%Lu] Cannot open input file <<%s>> by error %x\n", GetCurrentProcessId(),  filename, GetLastError());
        WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message) + 1, &cbWritten, NULL);
        exit(-4);
    } 
 
    // Get the name of output file 
    ZeroMemory(FileOut, BUF_SIZE);
    if (strchr(filename, '.') == NULL) // If input file hasn't file extension
        strcpy(FileOut, filename);
    else 
        strncpy(FileOut, filename, strchr(filename, '.') - filename);
    strcat(FileOut, ".out");
 
    // Try to open output file (WRITE MODE)
    hOut = CreateFile (FileOut, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
    if(hOut == INVALID_HANDLE_VALUE) {
        sprintf(message, "[CLIENT PID:%Lu] Cannot open output file <<%s>> by error %x\n", GetCurrentProcessId(), FileOut, GetLastError());
        WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message) + 1, &cbWritten, NULL);
        exit(-5);
    }
 
    // File reading-writing 
    ZeroMemory(Buffer, BUF_SIZE);
    while(ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) { 
        Changes = (*bufferProcessing)(Buffer, MAX_CHANGES);
        WriteFile(hOut, Buffer, nIn, &nOut, NULL); 
        if (nIn != nOut) {
            sprintf(message, "[CLIENT PID:%Lu] Writing file error %x\n", GetCurrentProcessId(), GetLastError());
            WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message) + 1, &cbWritten, NULL);
            exit(-6);
        }
        ZeroMemory(Buffer, BUF_SIZE);
    }
 
    FreeLibrary(hLib);
    sprintf(message, "%d", Changes);
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), message, strlen(message) + 1, &cbWritten, NULL);
    CloseHandle (hIn); 
    CloseHandle (hOut); 
    return Changes;
}