#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

// Events and file mapping descriptors
HANDLE hEventSend;
HANDLE hEventRecv;
HANDLE hFileMapping;
LPVOID lpFileMap;

// Event send name
CHAR lpEventSendName[] = "Session\\2\\$MylEventSendName$"; 
// Event receive name
CHAR lpEventRecvName[] = "Session\\2\\$MylEventRecvName$";
// Event file mapping name
CHAR lpFileShareName[] = "Session\\2\\$MyFileShareName$";

int main(int argc, char *argv[])
{
    CHAR str[80];
    DWORD dwRetCode;
    
    printf("Mapped and shared file, event sync, client process.\nEnter <Exit> to terminate...\n\n");
    
    // Try to open events
    hEventSend = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpEventSendName);
    hEventRecv = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpEventRecvName);
    if (hEventSend == NULL || hEventRecv == NULL)
    {
        fprintf(stdout, "OpenEvent: Error %ld\n",
            GetLastError());
        getch();
        CloseHandle(hEventSend);
        CloseHandle(hEventRecv);
        return-1;
    }
    
    // Try to open the file mapping
    hFileMapping = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, lpFileShareName);
    if (hFileMapping == NULL)
    {
        fprintf(stdout, "OpenFileMapping: Error %ld\n",
            GetLastError());
        getch();
        CloseHandle(hFileMapping);
        return -2;
    }

    // Try to open the map of file view
    lpFileMap = MapViewOfFile(hFileMapping,	FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
    if (lpFileMap == 0)
    {
        fprintf(stdout, "MapViewOfFile: Error %ld\n",
            GetLastError());
        getch();
        return -3;
    }
    
    while (TRUE)
    {
        printf("====> ");
        gets(str);
        strcpy((LPSTR)lpFileMap, str);
        
        if (!strcmp(str, "exit") || !strcmp(str, "Exit") || !strcmp(str, "EXIT"))
            break;
        SetEvent(hEventSend);
 
        dwRetCode = WaitForSingleObject(hEventRecv, INFINITE);
        
        if (dwRetCode == WAIT_OBJECT_0) puts((LPSTR)lpFileMap);
        if (dwRetCode == WAIT_ABANDONED_0 || dwRetCode == WAIT_FAILED)
        {
            printf("\nError waiting responce!\n)");
        }
        printf("\n");
    }

    CloseHandle(hEventSend);
    CloseHandle(hEventRecv);
    CloseHandle(hFileMapping);

    UnmapViewOfFile(lpFileMap);
    return 0;
}