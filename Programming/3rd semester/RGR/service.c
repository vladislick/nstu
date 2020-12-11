#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <AclAPI.h> // for SetSecurityInfo

// Maximum size of strings in the app
#define BUF_SIZE    256
// Name of library (DLL)
#define LIB_NAME    "maindll.dll"
// The name of function in the library
#define FUNC_NAME   "bufferProcessing"

// Function prototype from other .c file
extern int addLogMessage(const char* text);
// Server function prototype
int ServiceStart();
// Server function prototype
void ServiceStop();

// Events and file mapping descriptors
HANDLE hEventSend;
HANDLE hEventRecv;
HANDLE hEventTermination;
HANDLE hEvents[2];
HANDLE hFileMapping;
LPVOID lpFileMap;

// Event send name
CHAR lpEventSendName[] = "Session\\2\\$MylEventSendName$"; 
// Event receive name
CHAR lpEventRecvName[] = "Session\\2\\$MylEventRecvName$";
// Event file mapping name
CHAR lpFileShareName[] = "Session\\2\\$MyFileShareName$";

// Library descriptor
HINSTANCE hLib;
// Function prototype from the library
int (*bufferProcessing)(CHAR*, int);

int Server()
{
    // Waiting handles code
    DWORD 	dwRetCode;
    // Library name and function in the library name, just for fun
    char    libName[] = LIB_NAME,
            funcName[] = FUNC_NAME;
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
    int     changes = 0, clientNum = 0;
    // Buffer for the messages to the client and log file
    char message[BUF_SIZE] = { 0 };

    addLogMessage("Server Ready!");
    while (TRUE)
    {
        clientNum++;
        sprintf(message, "Wait a message #%d...", clientNum);
        addLogMessage(message);
        dwRetCode = WaitForSingleObject(hEventSend, INFINITE);
        // Wait for the client message
        if (dwRetCode == WAIT_ABANDONED_0 || dwRetCode == WAIT_FAILED)
            break;
        else
        {
            // Get the message
            strcpy(input, (LPSTR)lpFileMap);
            sprintf(message, "Get message: %s.", input);
            addLogMessage(message);
            
            // Check the library
            hLib = LoadLibrary(libName); 
            if (hLib == NULL) {
                sprintf(message, "Cannot load library \"%s\" (ERROR #%ld)", libName, GetLastError());
                addLogMessage(message);
                strcpy((LPSTR)lpFileMap, message);
                SetEvent(hEventRecv);
                continue;
            } 

            // Check the function in library
            bufferProcessing = (int(*)(CHAR*, int))GetProcAddress(hLib, funcName);
            if (bufferProcessing == NULL) {
                sprintf(message, "Cannot find function \"%s\" in the library (ERROR #%ld)", funcName, GetLastError());
                addLogMessage(message);
                FreeLibrary(hLib);
                strcpy((LPSTR)lpFileMap, message);
                SetEvent(hEventRecv);
                continue;
            }

            // Get filename and maximum of changes
            strcpy(fileName, input);
            char isFile = 1, quotes = 0;
            maxChanges = 0;
            for (int i = 0; i < strlen(input); i++) {
                if (input[i] == '"') {
                    quotes++;
                } else if (input[i] == ' ' && quotes % 2 == 0) {
                    ZeroMemory(fileName, BUF_SIZE);
                    strncpy(fileName, input, i);
                    isFile = 0;
                } else if (!isFile && input[i] > 47 && input[i] < 58) {
                    if (maxChanges > 0) maxChanges *= 10;
                    maxChanges += input[i] - 48;
                }
            }

            // Remove the quotes from the file name
            if (quotes % 2 == 0)
                for (int i = 0; i < strlen(fileName); i++)
                    if (fileName[i] == '"') 
                        for (int j = i; j < strlen(fileName); j++)
                            fileName[j] = fileName[j + 1];
            
            if (maxChanges == 0) maxChanges = 99999;

            // Get the name of output file 
            ZeroMemory(fileOutName, BUF_SIZE);
            if (strchr(fileName, '.') == NULL) // If input file hasn't file extension
                strcpy(fileOutName, fileName);
            else 
                strncpy(fileOutName, fileName, strchr(fileName, '.') - fileName);
            strcat(fileOutName, ".out");

            // Сheck the recognized data
            if (!strcmp(fileName, "")) {
                sprintf(message, "Incorrect data from a client");
                addLogMessage(message);
                FreeLibrary(hLib);
                strcpy((LPSTR)lpFileMap, message);
                SetEvent(hEventRecv);
                continue;
            }

            // Try to open input file (READ MODE)
            hIn = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); 
            if (hIn == INVALID_HANDLE_VALUE) {
                sprintf(message, "Cannot open input file \"%s\" (ERROR #%ld)", fileName, GetLastError());
                addLogMessage(message);
                FreeLibrary(hLib);
                strcpy((LPSTR)lpFileMap, message);
                SetEvent(hEventRecv);
                continue;
            } 

            // Try to open output file (WRITE MODE)
            hOut = CreateFile (fileOutName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
            if(hOut == INVALID_HANDLE_VALUE) {
                sprintf(message, "Cannot open output file \"%s\" (ERROR #%ld)", fileOutName, GetLastError());
                addLogMessage(message);
                FreeLibrary(hLib);
                CloseHandle(hIn);
                strcpy((LPSTR)lpFileMap, message);
                SetEvent(hEventRecv);
                continue;
            }

            // File reading-writing 
            ZeroMemory(Buffer, BUF_SIZE);
            while(ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) { 
                changes = (*bufferProcessing)(Buffer, maxChanges);
                WriteFile(hOut, Buffer, nIn, &nOut, NULL); 
                if (nIn != nOut) {
                    sprintf(message, "Catch an error with file reading/writing");
                    addLogMessage(message);
                    FreeLibrary(hLib);
                    CloseHandle(hIn);
                    CloseHandle(hOut);
                    strcpy((LPSTR)lpFileMap, message);
                    SetEvent(hEventRecv);
                    continue;
                }
                ZeroMemory(Buffer, BUF_SIZE);
            }

            // An answer to the client what was happened
            sprintf(message, "File proccess done. Maximum changes is set to %d.\n%d changes written in file \"%s\"", maxChanges, changes, fileOutName);
            addLogMessage(message);
            strcpy((LPSTR)lpFileMap, message);

            // Close files and library
            FreeLibrary(hLib);
            CloseHandle(hIn); 
            CloseHandle(hOut);
            SetEvent(hEventRecv);
            addLogMessage("The answer sent to a client!");
        }
    }
    return 0;
}

int ServiceStart()
{
    char message[BUF_SIZE] = { 0 };
    DWORD res;
    
    // Try to create events
    hEventSend = CreateEvent(NULL, FALSE, FALSE, lpEventSendName);
    hEventRecv = CreateEvent(NULL, FALSE, FALSE, lpEventRecvName);
    SetSecurityInfo(hEventSend,	SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL,NULL,NULL,	NULL);
    SetSecurityInfo(hEventRecv, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL);
    addLogMessage("Creating events");
    if (hEventSend == NULL || hEventRecv == NULL)
    {
        sprintf(message, "Cannot execute CreateEvent (ERROR #%ld)", GetLastError());
        addLogMessage(message);
        return (-1);
    }
    
    // Try to create the file mapping
    hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE, lpFileShareName);
    SetSecurityInfo(hFileMapping, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL);
    addLogMessage("Creating Mapped file");
    if (hFileMapping == NULL)
    {
        sprintf(message, "Cannot execute CreateFileMapping (ERROR #%ld)", GetLastError());
        addLogMessage(message);
        return -2;
    }
    
    // Try to open the map of file view
    lpFileMap = MapViewOfFile(hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
    if (lpFileMap == 0)
    {
        sprintf(message, "MapViewOfFile: Error %ld\n", GetLastError());
        addLogMessage(message);
        return -3;
    }
    return 0;
}

void ServiceStop()
{
    CloseHandle(hEventSend);
    CloseHandle(hEventRecv);
    CloseHandle(hFileMapping);
    UnmapViewOfFile(lpFileMap);
    addLogMessage("All Kernel objects closed!");
}