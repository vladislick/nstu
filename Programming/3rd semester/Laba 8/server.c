// Пример простого TCP – сервера
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

// Network port
#define PORT    65500
// Maximum size of strings in the app
#define BUF_SIZE    256
// Name of library (DLL)
#define LIB_NAME    "maindll.dll"
// The name of function in the library
#define FUNC_NAME   "bufferProcessing"

// The number of clients
int clients = 0;
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
        sprintf(message, "[Thread] Cannot load library \"%s\".\n", libName);
        printf(message);
        send(funcSocket, message, sizeof(message), 0);
        closesocket(funcSocket);
        ExitThread (-1);
    } 
    
    // Check the function in library
    bufferProcessing = (int(*)(CHAR*, int))GetProcAddress(hLib, funcName);
    if (bufferProcessing == NULL) {
        sprintf(message, "[Thread] Cannot find function \"%s\".\n", funcName);
        printf(message);
        send(funcSocket, message, sizeof(message), 0);
        FreeLibrary(hLib);
        closesocket(funcSocket);
        ExitThread (-2);
    }

    // Send a require for a file name 
    ZeroMemory(buff, BUF_SIZE);
    send(funcSocket, str1, sizeof(str1), 0);
    cbWritten = recv(funcSocket, buff, sizeof(buff), 0);
    if(cbWritten == SOCKET_ERROR) // принятие сообщения от клиента
    {
        sprintf(message, "[Thread] Error while receiving data from clinet [ERROR #%d].\n", GetLastError());
        printf(message);
        send(funcSocket, message, sizeof(message), 0);
        FreeLibrary(hLib);
        closesocket(funcSocket);
        ExitThread (-3);
    }
    strcpy(fileName, buff);
    
    ZeroMemory(buff, BUF_SIZE);
    send(funcSocket, str2, sizeof(str2), 0); // отправляем клиенту сообщение
    cbWritten = recv(funcSocket, buff, sizeof(buff), 0);
    if(cbWritten == SOCKET_ERROR) // принятие сообщения от клиента
    {
        sprintf(message, "[Thread] Error while receiving data from clinet [ERROR #%d].\n", GetLastError());
        printf(message);
        send(funcSocket, message, sizeof(message), 0);
        FreeLibrary(hLib);
        closesocket(funcSocket);
        ExitThread (-4);
    }
    maxChanges = atoi(buff);

    // Get the name of output file 
    strcpy(fileOutName, fileName);
    strcat(fileOutName, ".out");

    // Try to open input file (READ MODE)
    hIn = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); 
    if (hIn == INVALID_HANDLE_VALUE) {
        sprintf(message, "[Thread] Cannot open input file \"%s\". (ERROR #%lu)\n", fileName, GetLastError());
        printf(message);
        send(funcSocket, message, sizeof(message), 0);
        FreeLibrary(hLib);
        closesocket(funcSocket);
        ExitThread (-4);
    } 

    // Try to open output file (WRITE MODE)
    hOut = CreateFile (fileOutName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
    if(hOut == INVALID_HANDLE_VALUE) {
        sprintf(message, "[Thread] Cannot open output file \"%s\". (ERROR #%lu)\n", fileOutName, GetLastError());
        printf(message);
        send(funcSocket, message, sizeof(message), 0);
        FreeLibrary(hLib);
        closesocket(funcSocket);
        CloseHandle(hIn); 
        ExitThread (-5);
    }

    // File reading-writing 
    ZeroMemory(Buffer, BUF_SIZE);
    while(ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) { 
        changes = (*bufferProcessing)(Buffer, maxChanges);
        WriteFile(hOut, Buffer, nIn, &nOut, NULL); 
        if (nIn != nOut) {
            printf(message, "[Thread] Cannot write output file \"%s\" correctly. (ERROR #%lu)\n", fileOutName, GetLastError());
            printf(message);
            send(funcSocket, message, sizeof(message), 0);
            FreeLibrary(hLib);
            closesocket(funcSocket);
            CloseHandle(hIn); 
            CloseHandle(hOut);
            ExitThread (-6);
        }
        ZeroMemory(Buffer, BUF_SIZE);
    }
    sprintf(message, "[Thread] I proccessed the file \"%s\" correctly. Total changes is %d and it saved in file \"%s\".\n", fileName, changes, fileOutName);
    printf(message);
    send(funcSocket, message, sizeof(message), 0);
    FreeLibrary(hLib);
    CloseHandle(hIn); 
    CloseHandle(hOut);
    closesocket(funcSocket);
    clients--;
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
    localAddr.sin_family       = AF_INET;	   // Set the address system
    localAddr.sin_port         = htons(PORT);  // Set the network port
    localAddr.sin_addr.s_addr  = inet_addr("192.168.1.102");   // Make all addresses available

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
    	clients++;
    	HOSTENT *hst;
    	hst = gethostbyaddr((char *)&clientAddr.sin_addr.s_addr, 4, AF_INET);
    	printf("[SERVER] We have a new one! Its name is \"%s\" and its live at \"%s\".\n", (hst)?hst->h_name:"", inet_ntoa(clientAddr.sin_addr));
        // Launch a new thread to process the client
    	DWORD thID;
    	CreateThread(NULL, 0, clientProcessing, &clientSocket, 0, &thID);
    }
    return 0;
}