#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

// Network port
#define DEFAULT_PORT    65500
// Network address
#define DEFAULT_ADDR "192.168.1.102"
// Maximum size of strings in the app
#define BUF_SIZE    512
// Name of library (DLL)
#define LIB_NAME    "maindll.dll"
// The name of function in the library
#define FUNC_NAME   "bufferProcessing"

// Function prototype from the library
int (*bufferProcessing)(CHAR*, int);

DWORD WINAPI clientProcessing(LPVOID clientSocket) {
    
    SOCKET funcSocket = ((SOCKET*) clientSocket)[0];
    char buff[BUF_SIZE];
    char str1[] = "Enter the file name: ";
    char str2[] = "Enter the maximum of changes: ";
    int cbWritten, changes; // размер принятого сообщения
    char message[BUF_SIZE];

    // Library descriptor
    HINSTANCE hLib;
    // File descriptors (hIn - input file, hOut - output file)
    HANDLE hIn, hOut; 
    // Numbers of read/written bytes
    DWORD nIn, nOut;
    // Name for output file
    char    fileOutName[BUF_SIZE],
            fileName[BUF_SIZE],
            libName[] = LIB_NAME,
            funcName[] = FUNC_NAME,
            Buffer[BUF_SIZE];
    int maxChanges = 0;

    // Check the library
    hLib = LoadLibrary(libName);
    if (hLib == NULL) {
        sprintf(message, "Cannot load library \"%s\".\n", libName);
        printf("[SERVER] %s", message);
        send(funcSocket, message, sizeof(message), 0);
        closesocket(funcSocket);
        printf("[SERVER] One client just leave us.\n");
        ExitThread (-1);
    } 
    
    // Check the function in library
    bufferProcessing = (int(*)(CHAR*, int))GetProcAddress(hLib, funcName);
    if (bufferProcessing == NULL) {
        sprintf(message, "Cannot find function \"%s\".\n", funcName);
        printf("[SERVER] %s", message);
        send(funcSocket, message, sizeof(message), 0);
        FreeLibrary(hLib);
        closesocket(funcSocket);
        printf("[SERVER] One client just leave us.\n");
        ExitThread (-2);
    }

    ZeroMemory(message, BUF_SIZE);
    while(1) {
        // Send a require for a file name 
        ZeroMemory(buff, BUF_SIZE);
        strcat(message, str1);
        send(funcSocket, message, sizeof(message), 0);
        ZeroMemory(message, BUF_SIZE);
        cbWritten = recv(funcSocket, buff, sizeof(buff), 0);
        if(cbWritten == SOCKET_ERROR) // принятие сообщения от клиента
        {
            sprintf(message, "Error while receiving data from client [ERROR #%d].\n", GetLastError());
            printf("[SERVER] %s", message);
            send(funcSocket, message, sizeof(message), 0);
            FreeLibrary(hLib);
            closesocket(funcSocket);
            printf("[SERVER] One client just leave us.\n");
            ExitThread (-3);
        }
        if (!strcmp(buff, "exit")) break;
        strcpy(fileName, buff);
        
        ZeroMemory(buff, BUF_SIZE);
        send(funcSocket, str2, sizeof(str2), 0); // отправляем клиенту сообщение
        cbWritten = recv(funcSocket, buff, sizeof(buff), 0);
        if(cbWritten == SOCKET_ERROR) // принятие сообщения от клиента
        {
            sprintf(message, "Error while receiving data from clinet [ERROR #%d].\n", GetLastError());
            printf("[SERVER] %s", message);
            send(funcSocket, message, sizeof(message), 0);
            FreeLibrary(hLib);
            closesocket(funcSocket);
            printf("[SERVER] One client just leave us.\n");
            ExitThread (-4);
        }
        if (!strcmp(buff, "exit")) break;
        maxChanges = atoi(buff);

        // Get the name of output file 
        strcpy(fileOutName, fileName);
        strcat(fileOutName, ".out");

        // Try to open input file (READ MODE)
        hIn = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); 
        if (hIn == INVALID_HANDLE_VALUE) {
            sprintf(message, "Cannot open input file \"%s\". (ERROR #%lu)\n", fileName, GetLastError());
            printf("[SERVER] %s", message);
            continue;
        } 

        // Try to open output file (WRITE MODE)
        hOut = CreateFile (fileOutName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
        if(hOut == INVALID_HANDLE_VALUE) {
            sprintf(message, "Cannot open output file \"%s\". (ERROR #%lu)\n", fileOutName, GetLastError());
            printf("[SERVER] %s", message);
            send(funcSocket, message, sizeof(message), 0);
            FreeLibrary(hLib);
            closesocket(funcSocket);
            CloseHandle(hIn);
            printf("[SERVER] One client just leave us.\n"); 
            ExitThread (-5);
        }

        // File reading-writing 
        ZeroMemory(Buffer, BUF_SIZE);
        while(ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) { 
            changes = (*bufferProcessing)(Buffer, maxChanges);
            WriteFile(hOut, Buffer, nIn, &nOut, NULL); 
            if (nIn != nOut) {
                printf(message, "Cannot write output file \"%s\" correctly. (ERROR #%lu)\n", fileOutName, GetLastError());
                printf("[SERVER] %s", message);
                send(funcSocket, message, sizeof(message), 0);
                FreeLibrary(hLib);
                closesocket(funcSocket);
                CloseHandle(hIn); 
                CloseHandle(hOut);
                printf("[SERVER] One client just leave us.\n");
                ExitThread (-6);
            }
            ZeroMemory(Buffer, BUF_SIZE);
        }
        sprintf(message, "I proccessed the file \"%s\" correctly. \nTotal changes is %d and it saved in file \"%s\".\n", fileName, changes, fileOutName);
        printf("[SERVER] %s", message);
    }
    
    FreeLibrary(hLib);
    CloseHandle(hIn); 
    CloseHandle(hOut);
    closesocket(funcSocket);
    printf("[SERVER] One client just leave us.\n");
    return 0;
}

int main(int argc, char* argv[])
{
    char buff[1024];
    // Welcome message
    printf("[SERVER] Hi! Starting the server...\n");

    // Try to start networking
    if (WSAStartup(0x0202, (WSADATA *) &buff[0])) {
        printf("[SERVER] Catch an error with WSAStartup [ERROR #%d].\n", WSAGetLastError());
        return -1;
    }

    // Try to create socket object
    SOCKET serverSocket;
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("[SERVER] Catch an error while creating a socket [ERROR #%d].\n", WSAGetLastError());
        WSACleanup();
        return -2;
    }

    // Create a local address for the socket
    struct sockaddr_in localAddr;
    localAddr.sin_family       = AF_INET;	                // Set the address system
    localAddr.sin_port         = htons(DEFAULT_PORT);       // Set the network port
    localAddr.sin_addr.s_addr  = inet_addr(DEFAULT_ADDR);   // Make all addresses available

    // Trying to bind the socket to the local address
    if (bind(serverSocket, (struct sockaddr *) &localAddr, sizeof(localAddr))) {
        // Oh, there's not a big deal, just KILL THE APP if we catch an error
        printf("[SERVER] Catch an error while binding the socket and address [ERROR #%d].\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return -3;
    }

    // Try to listen the socket
    if (listen(serverSocket, 0x100)) {
        printf("[SERVER] Catch an error with listening to the socket [ERROR #%d].\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return -4;
    }
    
    // Yeap, we started
    printf("[SERVER] Server is working. \n");

    /* Don't forget to add a client variables */
    struct sockaddr_in  clientAddr;   
    int                 clientAddrSize = sizeof(clientAddr); 
    SOCKET              clientSocket; 

    // Always wait for a new user
    while(clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &clientAddrSize))
    {
    	HOSTENT *hst;
    	hst = gethostbyaddr((char *)&clientAddr.sin_addr.s_addr, 4, AF_INET);
    	printf("[SERVER] We have a new one! Its name is \"%s\".\n", (hst)?hst->h_name:"");
        // Launch a new thread to process the client
    	DWORD thID;
    	CreateThread(NULL, 0, clientProcessing, &clientSocket, 0, &thID);
    }
    return 0;
}