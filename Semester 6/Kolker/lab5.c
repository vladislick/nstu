#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>

int main() {
    // Инициализируем необходимые переменные
    int bind_status, server_socket , client_socket , bytes, structlen = sizeof(struct sockaddr_in);
	struct sockaddr_in server_addr, client_addr;
	char message[1000];
	
	// Создаем сокет для сервера
	printf("Server socket creating...");
    server_socket = socket(AF_INET , SOCK_STREAM , 0);
    if (server_socket < 0) {
        printf("Cannot connect socket\n");
        return -1;
    }
    printf("OK\n");
	
	// Создаем адрес сервера
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(12445);
	
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
	
    while (1) {
        // Включаем режим слушания эфира
        listen(server_socket , 1);
        
        // Выводим информацию о работе сервера
        printf("Server is waiting for connections...\n");
        
        // Принимаем поступающее соединение 
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &structlen);
        printf("There is connection request...");
        if (client_socket < 0) {
            printf("Cannot accept connetion\n");
            continue;
        }
        printf("ACCEPTED, IP %s\n", inet_ntoa(client_addr.sin_addr));
        memset(message, 0, 1000);
        
        //Принимаем данные от клиента
        while(1) {
            bytes = recv(client_socket, message, 1000, 0);
            // Если пусто или ошибка, сразу выходим из цикла
            if (bytes <= 0) break;
            // Выводим сообщение от клиента
            printf("Received: %s\n", message);
            // Отправляем клиенту подтверждение
            sprintf(message, "OK");
            write(client_socket , message , strlen(message));
            // Очищаем message
            memset(message, 0, 1000);
        }
        
        if(bytes == 0) {
            printf("Client disconnected\n");
            continue;
        } else if (bytes == -1) {
            printf("Cannot read incoming message correctly\n");
        }
    }
	return 0;
}