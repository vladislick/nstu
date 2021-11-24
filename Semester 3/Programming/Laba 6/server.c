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

// Many variables of semaphores and mapping file. 
// It's not a deal to describe each variable.
// If you know, so you know :)
HANDLE  hSemSend,
        hSemRecv,
        hSemTermination,
        hSems[2],
        hFileMapping;
CHAR    lpSemSendName[] = "$MySemSendName$",
        lpSemRecvName[] = "$MySemRecvName$",
        lpSemTerminationName[] = "$MySemTerminationName$", 
        lpFileShareName[] = "$MyFileShareName$";

// Mapping file pointer
LPVOID lpFileMap;

int main()
{
    // Library name and function in the library name, just for fun
    char    libName[] = LIB_NAME,
            funcName[] = FUNC_NAME;
    DWORD dwRetCode;
    char message[BUF_SIZE] = { 0 };
    char out[BUF_SIZE * 4] = { 0 };
    
    DWORD  cbWritten;
    // Buffer for read/write the files
    CHAR Buffer[BUF_SIZE];
    // File descriptors (hIn - input file, hOut - output file)
    HANDLE hIn, hOut; 
    // Numbers of read/written bytes
    DWORD nIn, nOut;
    // Message from server app received by the pipe
    char    input[BUF_SIZE] = { 0 };
    // Name of input file
    char    fileName[BUF_SIZE] = { 0 };
    // Name for output file
    char    fileOutName[BUF_SIZE] = { 0 };
    int     maxChanges = 0;
    // Current number of changes in file
    int     changes = 0;
    int     errorCode = 0;


    printf("Hi!\nStarting all systems...\n");
    // Create this semaphore as sending "blink"
    hSemSend = CreateSemaphore(NULL, 0, 1, lpSemSendName);
    // Open this semaphore as reciving "blink"
    hSemRecv = CreateSemaphore(NULL, 0, 1, lpSemRecvName); 
    if (hSemSend == NULL || hSemRecv == NULL)
        printf("[SYSTEM ERROR #%d] Cannot create recv/send semaphore.\n", GetLastError());
    
    if (GetLastError() == ERROR_ALREADY_EXISTS)
        printf("It seem like other server application is running.\n");
    // Try to create this semaphore as signal that we are running down
    hSemTermination = CreateSemaphore(NULL,	0, 1, lpSemTerminationName);
    if (hSemTermination == NULL)
        printf("[SYSTEM ERROR #%d] Cannot create termination semaphore.\n", GetLastError());
    // Try to create mapping file
    hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 100, lpFileShareName);
    if (hFileMapping == NULL)
        printf("[SYSTEM ERROR #%d] Cannot create mapping file.\n", GetLastError());
    // Try to view mapping file 
    lpFileMap = MapViewOfFile(hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
    if (lpFileMap == 0)
        printf("[SYSTEM ERROR #%d] Cannot view mapping file.\n", GetLastError());
    // Put down the semaphores into an array for object waiting function
    hSems[0] = hSemTermination;
    hSems[1] = hSemSend;

    if (!GetLastError()) 
        printf("All systems started correctly.\n");
    else
        printf("Some systems did not started correctly.\n");
    
    while (TRUE)
    {
        fprintf(stdout, "\n> Waiting for a client... \n", GetLastError());
        dwRetCode = WaitForMultipleObjects(2, hSems, FALSE, INFINITE);
        if (dwRetCode == WAIT_ABANDONED_0 ||
            dwRetCode == WAIT_ABANDONED_0 + 1 || dwRetCode == WAIT_OBJECT_0 || dwRetCode == WAIT_FAILED)
            break;
        else
        {
            // Clear errors
            errorCode = 0;
            // Check received data
            ZeroMemory(out, BUF_SIZE * 4);
            ZeroMemory(message, BUF_SIZE);
            strcpy(input, (LPSTR)lpFileMap);
            printf("===> Received message: <<%s>>.\n", input); 
            
            // Check the library
            hLib = LoadLibrary(libName);
            if (hLib == NULL && errorCode == 0) {
                sprintf(message, "[SYSTEM ERROR #%d] Cannot load library <<%s>>.\n", GetLastError(), libName);
                printf(message);
                strcat(out, message);
                errorCode = -1;
            } 
            else printf("===> Library <<%s>> is loaded successfully.\n", libName);
            
            // Check the function in library
            bufferProcessing = (int(*)(CHAR*, int))GetProcAddress(hLib, funcName);
            if (bufferProcessing == NULL && errorCode == 0) {
                sprintf(message, "[SYSTEM ERROR #%d] Cannot find function <<%s>> in library <<%s>>.\n", GetLastError(), funcName, libName);
                printf(message);
                strcat(out, message);
                errorCode = -2;
            }
            else printf(message, "===> Function <<%s>> in the library found.\n", funcName, libName);
            
            // Get filename and maximum of changes
            strcpy(fileName, input);
            char isFile = 1;
            maxChanges = 0;
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
            printf("===> Maximum of changes is %d.\n", maxChanges);

            // Get the name of output file 
            ZeroMemory(fileOutName, BUF_SIZE);
            if (strchr(fileName, '.') == NULL) // If input file hasn't file extension
                strcpy(fileOutName, fileName);
            else 
                strncpy(fileOutName, fileName, strchr(fileName, '.') - fileName);
            strcat(fileOutName, ".out");

            // Сheck the recognized data
            if (!strcmp(fileName, "") && errorCode == 0) {
                sprintf(message, "[SYSTEM ERROR #%d] Incorrect data from client.\n", GetLastError());
                printf(message);
                strcat(out, message);
                errorCode = -3;
            } else printf("===> Input file: <<%s>>\n===> Output file: <<%s>>\n", fileName, fileOutName);

            // Try to open input file (READ MODE)
            hIn = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); 
            if (hIn == INVALID_HANDLE_VALUE && errorCode == 0) {
                sprintf(message, "[SYSTEM ERROR #%d] Cannot open input file <<%s>>.\n",  GetLastError(),  fileName);
                printf(message);
                strcat(out, message);
                errorCode = -3;
            } 

            // Try to open output file (WRITE MODE)
            if (errorCode == 0) hOut = CreateFile (fileOutName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
            if(hOut == INVALID_HANDLE_VALUE && errorCode == 0) {
                sprintf(message, "[SYSTEM ERROR #%d] Cannot open output file <<%s>>.\n",  GetLastError(),  fileName);
                printf(message);
                strcat(out, message);
                errorCode = -4;
            }

            // File reading-writing 
            ZeroMemory(Buffer, BUF_SIZE);
            if (errorCode == 0) while(ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) { 
                changes = (*bufferProcessing)(Buffer, maxChanges);
                WriteFile(hOut, Buffer, nIn, &nOut, NULL); 
                if (nIn != nOut) {
                    sprintf(message, "[SYSTEM ERROR #%d] Writing file error.\n", GetLastError());
                    strcat(out, message);
                }
                ZeroMemory(Buffer, BUF_SIZE);
            }
            
            // Answer to the client what was happened
            if (errorCode != 0)
                sprintf(message, "[SYSTEM ERROR #%d] Cannot proccess file <<%s>>.\n", errorCode, fileName);
            else 
                sprintf(message, "File proccess done. %d changes written in file <<%s>>.\n", changes, fileOutName);
            strcat(out, message);
            printf(message);
            strcpy((char*)lpFileMap, out);

            // Close files and library
            FreeLibrary(hLib);
            CloseHandle(hIn); 
            CloseHandle(hOut);

            // Let the client to read
            ReleaseSemaphore(hSemRecv, 1, NULL);
        }
    }
    // Massive leaving actions
    CloseHandle(hSemSend);
    CloseHandle(hSemRecv);
    CloseHandle(hSemTermination);
    UnmapViewOfFile(lpFileMap);
    CloseHandle(hFileMapping);
    return 0;
}

