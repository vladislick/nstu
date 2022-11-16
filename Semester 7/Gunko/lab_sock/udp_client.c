// пример протого UDP клиента
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Функция обработки ошибок
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
   int sock, n; // дескрипторы
   unsigned int length; // размер структуры адреса
   struct sockaddr_in server, from; // структуры адресов сервера и клиента соответсвенно
   struct hostent *hp; // структура хоста
   char buffer[256]; // буфер
   
   if (argc != 3) { printf("Usage: server port\n");
                    exit(1);
   }
	// Шаг 1 - создание сокета
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) error("socket");
	
	// Шаг 2 - связывание
	server.sin_family = AF_INET; // указание адресации
	hp = gethostbyname(argv[1]); // извлечение хоста
	if (hp==0) error("Unknown host");
	bcopy((char *)hp->h_addr, 
		(char *)&server.sin_addr,
		 hp->h_length);
	server.sin_port = htons(atoi(argv[2])); // извлечение порта
	length=sizeof(struct sockaddr_in);
	
	// Шаг 3 - прием и отправка датаграмм
	while(1)
	{

		// отправка датаграммы
		printf("\nS<=C:");
		bzero(buffer,256);
		fgets(&buffer[0], sizeof(buffer) - 1, stdin);
		//fgets(buffer,255,stdin);
		// проверка на quit
		if (!strcmp(&buffer[0], "quit\n")) break;
		
		n=sendto(sock,&buffer[0],strlen(buffer),0,(const struct sockaddr *)&server,length);
		if (n < 0) error("Sendto");
		
		// структура sockaddr_in и ее размер
        	struct sockaddr_in server_addr;
       		int server_addr_size = sizeof(server_addr);
		
		// прием датаграммы
		//n = recvfrom(sock,buffer,256,0,(struct sockaddr *)&from, &length);
		int n = recvfrom(sock, &buffer[0], sizeof(buffer) - 1, 0,(struct sockaddr *)&server_addr, &server_addr_size);
		if (n < 0) error("recvfrom");
		/*write(1,"Got an ack: ",12);
		write(1,buffer,n);*/
		// Вывод принятого с сервера сообщения на экран
        	printf("\nS=>C:%s", &buffer[0]);
	}
	// Шаг 4 - закрытие сокета
	close(sock);
	return 0;
}
