#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#define MAXLINE 256

// Структура для обмена данными
struct msg_struct {
	char filename[MAXLINE];
	char symbol;
};

// Проверка корректности адреса в виде строки
int addressCheck(const char* address) {
	// Количество точек
	int dots = 0;
	// Флаг наличия числа
	int flag = 0;
	// Проходим в цикле по всем символам
	for (int i = 0; address[i] != '\0'; i++) {
		// Если точка 
		if (address[i] == '.') {
			dots++;
			if (flag) flag = 0;
			else return 0;
		}
		// Если не цифра
		if (address[i] > 57 || address[i] < 48) return 0;
		else flag = 1;
	}
	// Должно быть 3 точки (xx.xx.xx.xx) и несброшенный флаг
	if (dots == 3 && flag == 1) return 1;
	// Иначе тоже ошибка
	return 0;
}

// Проверка корректности введенного порта в виде строки
int portCheck(const char* port) {
	// Количество цифр
	int digits = 0;	
	// Проход по всем символам
	for (int i = 0; port[i] != '\0'; i++) {
		if (port[i] > 57 || port[i] < 48) return 0;
		else digits++;
	}
	// Проверка численного значения
	if (atoi(port) < 65536) return 1;
	return 0;
}

int main () {
	// =============== ПЕРЕМЕННЫЕ СОКЕТА ===============
	// Дескриптор сокета
	int sock;
	// Структура адреса сервера 
	struct sockaddr_in dest_addr;
	
	// =============== ПЕРЕМЕННЫЕ ВСПОМОГАТЕЛЬНЫЕ ===============
	// Сообщение для отправки на сервер
	struct msg_struct msg;
	// Буфер для получения символа	
	char buff[MAXLINE];

	// Создание сокета	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		printf("ERROR: Cannot create socket.\n");
		exit(-1);
	}
	
    // Создание адреса
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(12345);
	dest_addr.sin_addr.s_addr = inet_addr("192.168.2.173");

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

		// Очищаем память
		memset(shm_ptr, 0, sizeof(struct msg_struct));
		// Отправляем сообщение серверу в разделяему память
		*shm_ptr = msg;

		// Говорим серверу, что можно забирать сообщение
		if (sem_post(sem_client) < 0) {
			printf("ERROR: Cannot touch client's semaphore: %s.\n\n", strerror(errno));
			continue;
		}

		// Говорим, что всё ок
		printf("Message sent successfully.\n");

		// Если необходимо завершить работу
		if (!strcmp(msg.filename, "shutdown")) break;

		// Говорим, что ждём сервер
		printf("Wait for the server...\n");

		// Ожидаем ответа от сервера
		if (sem_wait(sem_server) < 0 ) {
			printf("ERROR: Cannot wait for the server's semaphore: %s.\n\n", strerror(errno));
			continue;
		}

		// Выводим на экран ответ сервера
		printf("Answer from the server: \n%s.\n\n", shm_ptr->filename);
	}

	// Закрываем дескрипторы семафоров
	sem_close(sem_client);	
	sem_close(sem_server);

	// Закрываем дескриптор разделяемой памяти
	close(fd);

	exit(0);
}
