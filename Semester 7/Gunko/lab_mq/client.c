#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>

// Имя очереди сообщений от клиента серверу
#define CLIENT_MQ "/toserver.mq"
// Имя очереди сообщений от сервера клиенту
#define SERVER_MQ "/toclient.mq"
// Права доступа к очереди сообщений
#define FILE_MODE S_IRUSR | S_IWUSR
// Флаги для открытия очереди сервера
#define SERVER_FLAGS O_RDONLY 
// Флаги для открытия очереди клиента
#define CLIENT_FLAGS O_WRONLY | O_CREAT

#define MAXLINE 256

// Структура сообщений
struct msg_struct {
	char filename[MAXLINE];
	char symbol;
};

int main () {
	// Создаём сообщение
	struct msg_struct msg;
	// Создаём дескрипторы очередей сообщений 
    mqd_t client_mq, server_mq;

	// Количество считанных байт данных
	int n = 0;
	// Буфер для получения символа	
	char buff[MAXLINE];
	// Атрибуты очереди сообщений
	struct mq_attr attr;
	// Ответ от сервера
	void* server_msg;
	
	// Открываем очередь сообщений клиента с атрибутами по умолчанию
	client_mq = mq_open(CLIENT_MQ, CLIENT_FLAGS, FILE_MODE, NULL);
	if (client_mq < 0) {
		printf("ERROR: Cannot open client's message queue.\n");
		return -1;
	}

	// Открываем очередь сообщений сервера
	printf("Wait for the server message queue to appear..."); fflush(stdout);
	while((server_mq = mq_open(SERVER_MQ, SERVER_FLAGS)) < 0) sleep(1);
	printf("OK.\n\n");
	// Получаем атрибуты очереди сообщений сервера
	mq_getattr(server_mq, &attr);
	// Выделяем память под ответ сервера
	server_msg = malloc(attr.mq_msgsize);

    while(1) {
        // Получаем имя файла
        printf("Enter the filename (\'exit\' to exit, \'shutdown\' to exit and shut down the server): ");
        fgets(msg.filename, MAXLINE, stdin);
        // Заменим последний символ на конец строки
        msg.filename[strlen(msg.filename) - 1] = '\0';
		// Если пришла команда выхода
		if (!strcmp(msg.filename, "exit")) break;

        // Получаем символ для обработки
        if (strcmp(msg.filename, "shutdown")) do {
            printf("Enter the symbol: ");
            fgets(buff, MAXLINE, stdin);
            buff[strlen(buff) - 1] = '\0';
        } while(strlen(buff) != 1);
        
        // Получаем реальный символ
        msg.symbol = buff[0];

        // Отправляем сообщение серверу без приоритета
		n = mq_send(client_mq, (const char*)&msg, sizeof(msg), 0);
		
		// Проверка
		if (n == 0) { 
			printf("Message sent successfully.\n");
		} else {
			printf("ERROR: Cannot sent message to the server: %s.\n", strerror(errno));
			continue;
		}
		
		// Если необходимо завершить работу
		if (!strcmp(msg.filename, "shutdown")) break;

        // Ожидаем ответа от сервера
        n = mq_receive(server_mq, server_msg, attr.mq_msgsize, NULL);
        printf("Answer from the server: ");
        if (n > 0)
        	printf("%s", (char*)server_msg);
        else if (n == 0) 
        	printf("empty message");
        else 
        	printf("ERROR: %s", strerror(errno));
        printf(".\n\n");
    }
    
    // Высвобождаем память под сообщение
	free(server_msg);
	// Закрываем дескрипторы очередей
	mq_close(client_mq);	
	mq_close(server_mq);
	
	exit(0);
}
