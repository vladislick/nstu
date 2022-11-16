/* пример простого UDP сервера
 Порт является аргументом, для запуска сервера неободимо ввести:
   # ./[имя_скомпилированного_файла] [номер порта]
   # ./server 57123*/
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

// Функция обработки ошибок
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

  // функция по варианту
  int myfunc(int a, int b)
  {
	return a+b;
  }

int main(int argc, char *argv[])
{
   int n;
   struct sockaddr_in sockaddr_in; // структура адреса сервера
   struct sockaddr_in client_addr; // структура адреса клиента
   char buf[1024]; // буфер
   socklen_t fromlen;
   
   printf("UDP DEMO Server\n");
   
   if (argc < 2) {
      fprintf(stderr, "ERROR, no port provided\n");
      exit(0);
   }
   
   // Шаг 1 - создание сокета
   int mysocket,client_addr_size;
   if ((mysocket=socket(AF_INET,SOCK_DGRAM,0))<0)
		{error("Opening socket");}
   

   // Шаг 2 - связывание сокета с локальным адресом
    struct sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(atoi(argv[1]));
    local_addr.sin_addr.s_addr = INADDR_ANY;
   
   // вызываем bind для связывания
   if (bind(mysocket, (struct sockaddr *)&local_addr, sizeof(local_addr)))
   {    error("binding");}
   client_addr_size = sizeof(struct sockaddr_in);
   
   // Шаг 3 прием и передача датаграмм
   int a = -1;
   int b = -1;
   
while (1) 
   {
		// прием датаграммы
		int bytes_recv = recvfrom(mysocket,&buf[0],sizeof(buf)-1,0,(struct sockaddr *)&client_addr, &client_addr_size);
		if (bytes_recv < 0)
		{error("recvfrom");}
		
		// Определяем IP-адрес клиента и прочие атрибуты
		// и выводим на экран (эта чать кода необязательна)
        struct hostent *hst;
        hst = gethostbyaddr((char *)&client_addr.sin_addr, 4, AF_INET);
        printf("+%s [%s:%d] new DATAGRAM!\n",
        (hst) ? hst->h_name : "Unknown host",
        (char*)inet_ntoa(client_addr.sin_addr),
        ntohs(client_addr.sin_port));
	   
	    // добавление завершающего нуля
        buf[bytes_recv] = 0;
		
		// Вывод на экран
        printf("C=>S:%s\n", &buf[0]);
		
       if(a == -1) // установка a
			a = atoi(buf);
		else if(b == -1) // установка b
			b = atoi(buf);
		
		if(a != -1 && b != -1)
		{
			// вызов функции(если a и b != -1 )
			a = myfunc(a,b);
			//itoa(a,buf,10);
			snprintf(buf, strlen(buf), "%d", a);
			buf[strlen(buf)] = '\n';
			a = -1;
			b = -1;
		}

		// посылка датаграммы клиенту
        n = sendto(mysocket, &buf[0], sizeof(buf), 0,(struct sockaddr *)&client_addr, sizeof(client_addr));
		if (n  < 0) error("sendto");
   }
   close(mysocket);
   return 0;
 }

