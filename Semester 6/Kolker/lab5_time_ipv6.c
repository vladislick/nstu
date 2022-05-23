#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main() {
    // Инициализируем необходимые переменные
    int bind_status, server_socket , client_socket , bytes, structlen = sizeof(struct sockaddr_in6);
	struct sockaddr_in6 server_addr, client_addr;
    char SENDER_ADDR[INET6_ADDRSTRLEN];
    time_t rawtime;
	char message[1000];
	
	// Создаем сокет для сервера
	printf("Server socket creating...");
    server_socket = socket(AF_INET6 , SOCK_STREAM , 0);
    if (server_socket < 0) {
        printf("Cannot connect socket\n");
        return -1;
    }
    printf("OK\n");
	
	// Создаем адрес сервера
	server_addr.sin6_family = AF_INET6;
	server_addr.sin6_addr = in6addr_any;
	server_addr.sin6_port = htons(12445);
	
	// Занимаем для сервера порт
    while(1) {
        printf("Binding socket and address...");
        bind_status = bind(server_socket, (struct sockaddr *)&server_addr , sizeof(server_addr));
        if (bind_status < 0) {
            printf("Cannot bind\n");
            sleep(4);
        } else break;
	} 
	printf("OK\n");

    // Включаем режим слушания эфира
    listen(server_socket , 1);
    
    // Выводим информацию о работе сервера
    printf("Server is waiting for connections...\n");
    
    // Принимаем поступающее соединение 
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &structlen);
    printf("There is connection request...");
    if (client_socket < 0) {
        printf("Cannot accept connetion\n");
        return -3;
    }
    inet_ntop(client_addr.sin6_family, get_in_addr((struct sockaddr *)&client_addr),SENDER_ADDR, sizeof SENDER_ADDR); 
    printf("ACCEPTED, IP %s\n", SENDER_ADDR);
    memset(message, 0, 1000);
    
    //Принимаем данные от клиента
    time(&rawtime);
    sprintf(message, "%d", rawtime);
    printf("Current time: %d\n", rawtime);
    write(client_socket , message , strlen(message));
    sleep(2);
    memset(message, 0, 1000);            
    bytes = recv(client_socket, message, 1000, 0);
    // Если пусто или ошибка, сразу выходим из цикла
    if (bytes > 0) {
        // Выводим сообщение от клиента
        printf("Received: %s\n", message);
    }
    
    if(bytes == 0) {
        printf("Client disconnected\n");
    } else if (bytes == -1) {
        printf("Cannot read incoming message correctly\n");
    }
	return 0;
}