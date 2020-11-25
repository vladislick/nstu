// Пример простого TCP-клиента
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

//gcc client.c -o client.exe -lws2_32

#define PORT 65500
#define SERVERADDR "192.168.1.102"

int main(int argc, char* argv[])
 {
	 char buff[1024];
     printf("TCP DEMO CLIENT\n");

     // Шаг 1 - инициализация библиотеки Winsock
     if (WSAStartup(0x202, (WSADATA *)&buff[0]))
     {
         printf("WSAStart error %d\n", WSAGetLastError());
         return -1;
     }

     // Шаг 2 - создание сокета
     SOCKET my_sock;
     my_sock = socket(AF_INET, SOCK_STREAM, 0);
     if (my_sock < 0)
     {
         printf("Socket() error %d\n", WSAGetLastError());
         return -1;
     }

     // Шаг 3 - установка соединения
     // заполнение структуры sockaddr_in - указание адреса и порта сервера
     struct sockaddr_in dest_addr;
     dest_addr.sin_family = AF_INET;
     dest_addr.sin_port = htons(PORT);
     HOSTENT *hst;

     // получение IP адреса и установка соединения
	 if (inet_addr(SERVERADDR) != INADDR_NONE)
         dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR); //преобразование IP адреса из символьного в сетевой формат

     // адрес сервера получен - пытаемся установить соединение
     if (connect(my_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)))
     {
         printf("Connect error %d\n", WSAGetLastError());
         return -1;
     }

     printf("Connect with %s succed\n \
             Type quit for quit\n\n", SERVERADDR);

     // Шаг 4 - чтение и передача сообщений
     int nsize;
     while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
     {
         // ставим завершающий ноль в конце строки
         buff[nsize] = 0;

         // выводим на экран
         printf("S=>C:%s", buff);

         // читаем пользовательский ввод с клавиатуры
         ZeroMemory(buff, 1024);
         printf("S<=C:"); scanf("%s", buff);

         // проверка на "quit"
         if (!strcmp(&buff[0], "quit\n"))
         {
             // Корректный выход
             printf("Exit...");
             closesocket(my_sock);
             WSACleanup();
             return 0;
         }

         // передаем строку клиента серверу
         send(my_sock, buff, strlen(buff), 0);
     }
     printf("Recv error %d\n", WSAGetLastError());
     closesocket(my_sock);
     WSACleanup();
     return -1;
 }