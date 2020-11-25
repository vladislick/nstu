#include   <windows.h> 
#include   <stdio.h> 
#include   <string.h> 

// Maximum count of threads
#define threads 64
// Maximum size of strings in the app
#define BUF_SIZE    256
// Name of library (DLL)
#define LIB_NAME    "maindll.dll"
// The name of function in the library
#define FUNC_NAME   "bufferProcessing"

// Global mutex (for all threads)
HANDLE hMutex;
// Struct of information for threads
struct fileData
{
    int     index;
    char    file[80];
    int     max;
} data[threads];
// Function prototype from the library
int (*bufferProcessing)(CHAR*, int);

DWORD WINAPI func(int *num)
{
    // Library descriptor
    HINSTANCE hLib;
    // File descriptors (hIn - input file, hOut - output file)
    HANDLE hIn, hOut; 
    // Numbers of read/written bytes
    DWORD nIn, nOut;
    // Name for output file
    char    fileOutName[BUF_SIZE],
            libName[] = LIB_NAME,
            funcName[] = FUNC_NAME,
            Buffer[BUF_SIZE];
    
    int i = *num,
        changes = 0,
        maxChanges = data[i].max;

    printf("[Thread #%d] I'm waiting for the mutex...\n", i);
    WaitForSingleObject(hMutex, INFINITE);
    printf("[Thread #%d] I just catch the mutex! I start processing the file \"%s\".\n", i, data[i].file);
        
    // Check the library
    hLib = LoadLibrary(libName);
    if (hLib == NULL) {
        printf("[Thread #%d] Cannot load library \"%s\".\n", i, libName);
        ExitThread (-1);
    } 
    
    // Check the function in library
    bufferProcessing = (int(*)(CHAR*, int))GetProcAddress(hLib, funcName);
    if (bufferProcessing == NULL) {
        printf("[Thread #%d] Cannot find function \"%s\".\n", i, funcName);
        ExitThread (-2);
    }

    // Get the name of output file 
    ZeroMemory(fileOutName, BUF_SIZE);
    strcpy(fileOutName, data[i].file);
    strcat(fileOutName, ".out");

    // Try to open input file (READ MODE)
    hIn = CreateFile(data[i].file, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); 
    if (hIn == INVALID_HANDLE_VALUE) {
        printf("[Thread #%d] Cannot open input file \"%s\". (ERROR #%lu)\n", i, data[i].file, GetLastError());
        ExitThread (-3);
    } 

    // Try to open output file (WRITE MODE)
    hOut = CreateFile (fileOutName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
    if(hOut == INVALID_HANDLE_VALUE) {
        printf("[Thread #%d] Cannot open output file \"%s\". (ERROR #%lu)\n", i, fileOutName, GetLastError());
        ExitThread (-4);
    }

    // File reading-writing 
    ZeroMemory(Buffer, BUF_SIZE);
    while(ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) { 
        changes = (*bufferProcessing)(Buffer, maxChanges);
        WriteFile(hOut, Buffer, nIn, &nOut, NULL); 
        if (nIn != nOut) {
            printf("[Thread #%d] Cannot write output file \"%s\" correctly. (ERROR #%lu)\n", i, fileOutName, GetLastError());
            ExitThread (-5);
        }
        ZeroMemory(Buffer, BUF_SIZE);
    }
    printf("[Thread #%d] I proccessed the file \"%s\" correctly. All changes saved in file \"%s\".\n", i, data[i].file, fileOutName);
    FreeLibrary(hLib);
    CloseHandle(hIn); 
    CloseHandle(hOut);
    ReleaseMutex(hMutex);
    return changes;
}

int main(int argc, char* argv[])
{
    // Thread descriptors
    HANDLE  hThreads[threads];
    // Thread result codes
    DWORD   res[threads];
    // The maximum of changes
    int MAXCHANGES = 99999;

    // Check the arguments
    if (argc < 3)
    {
        printf("[MAIN] Usage: main.exe filename1 filename2 ... max_of_changes");
        exit(-1);
    }

    // Check if user say the maximum of changes
    if (isdigit(argv[argc - 1][0])) MAXCHANGES = atoi(argv[argc - 1]); 
    // Create a 'stop' mutex
    hMutex = CreateMutex(NULL, FALSE, NULL);
    // Capture the mutex (to hold the threads)
    WaitForSingleObject(hMutex, INFINITE);
    
    // Create threads and hold them by the mutex
    for (int i = 0; i < argc - 1; i++)
    {
        // Copy data
        strcpy(data[i].file, argv[i + 1]);
        data[i].max = MAXCHANGES;
        data[i].index = i;
        // If the argument is number, just skip 
        if (isdigit(data[i].file[0])) continue;
        // Create a thread
        hThreads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, &data[i].index, 0, NULL);
        if (hThreads[i] == NULL)
        {
            printf("[MAIN] Cannot create a thread for file \"%s\"! (ERROR #%lu)\n", data[i].file, GetLastError());
            exit(-i);
        }
        else printf("[MAIN] Thread #%d for file \"%s\" created successfuly!\n", i, data[i].file);
        Sleep(10);	
    }	
    
    Sleep(10);	
    printf("\n");
    
    // Free the mutex to start threads
    ReleaseMutex(hMutex);

    // Wait for each thread
    for (int i = 0; i < argc - 1; i++)
    {
        if (isdigit(data[i].file[0])) continue;
        WaitForSingleObject(hThreads[i], INFINITE);
        GetExitCodeThread(hThreads[i], &res[i]);	
    }
    // Now we can close the mutex
    CloseHandle(hMutex);

    // Show the results to the user
    printf("\n");
    for (int i = 0; i < argc - 1; i++) {
        if (isdigit(data[i].file[0])) continue;
        // So, res is DWORD type var, then negative number is very big in binary and used 0 - 1024 for example
        if (res[i] > ((DWORD)0 - (DWORD)1024)) 
            printf("[MAIN] Thread #%d was unable to process the file \"%s\".\n", i, data[i].file);
        else 
            printf("[MAIN] Thread #%d processed the file \"%s\". Total changes is %d.\n", i, data[i].file, res[i]);
        CloseHandle(hThreads[i]);

    }
}