#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <winsock2.h>
#include <windows.h>

//gcc client.c -o client.exe -lws2_32

// Network port
#define DEFAULT_PORT 65500
// Network address
#define DEFAULT_ADDR "192.168.1.102"

int main(int argc, char* argv[])
{
    int cbWritten;
    char buff[1024];

     // Try to start networking
    if (WSAStartup(0x0202, (WSADATA *) &buff[0])) {
        printf("[CLIENT] Catch an error with WSAStartup [ERROR #%d].\n", WSAGetLastError());
        return -1;
    }

    // Try to create socket object
    SOCKET mySocket;
    if ((mySocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("[CLIENT] Catch an error while creating a socket [ERROR #%d].\n", WSAGetLastError());
        WSACleanup();
        return -2;
    }

    struct sockaddr_in destAddr;
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(DEFAULT_PORT);
   
    if (inet_addr(DEFAULT_ADDR) != INADDR_NONE)
        destAddr.sin_addr.s_addr = inet_addr(DEFAULT_ADDR);
    else {
        printf("[CLIENT] Cannot read network address.\n");
        return -3;
    }
    
    if (connect(mySocket, (struct sockaddr *) &destAddr, sizeof(destAddr))) {
        printf("[CLIENT] Cannot connect to the server \"%s:%d\" [ERROR #%d].\n", DEFAULT_ADDR, DEFAULT_PORT, WSAGetLastError());
        return -4;
    }

    printf("[CLIENT] Connected to the server \"%s:%d\".\n", DEFAULT_ADDR, DEFAULT_PORT);

    while (1) {
        cbWritten = recv(mySocket, buff, sizeof(buff), 0);
        if (cbWritten == SOCKET_ERROR) {
            printf("[CLIENT] Cannot receive data from server [ERROR #%d].\n", WSAGetLastError());
            return -1;
        }

        printf("[SERVER] %s\n", buff);
        printf("[USER] ");
        ZeroMemory(buff, 1024);
        scanf("%s", buff);
        Sleep(50);

        send(mySocket, buff, strlen(buff), 0);
        if (!strcmp(buff, "exit")) break;
    }
    closesocket(mySocket);
    WSACleanup();
    printf("Press any key to continue...");
    getch();
    return 0;
}