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
			continue;
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
	// Проход по всем символам
	for (int i = 0; port[i] != '\0'; i++)
		if (port[i] > 57 || port[i] < 48) return 0;
	// Проверка численного значения
	if (atoi(port) < 65536) return 1;
	return 0;
}

int main (int argc, char** argv) {
	// =============== ПЕРЕМЕННЫЕ СОКЕТА ===============
	// Дескриптор сокета
	int sock;
	// Структура адреса сервера 
	struct sockaddr_in dest_addr;
	// Размер структуры адреса
	unsigned int structlen = sizeof(struct sockaddr_in); 
	
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
	dest_addr.sin_port = 0;
	dest_addr.sin_addr.s_addr = 0;

    // Узнаем адрес и порт сервера из аргументов
	for (int i = 1; i < argc; i++) {
		if (addressCheck(argv[i])) dest_addr.sin_addr.s_addr = inet_addr(argv[i]);
		if (portCheck(argv[i])) dest_addr.sin_port = htons(atoi(argv[i]));
	}

	// Если не удалось определить адрес из аргуметов
	while (dest_addr.sin_addr.s_addr == 0) {
		memset(buff, '\0', MAXLINE);
		printf("Enter server address: ");
		fgets(buff, MAXLINE, stdin);
		buff[strlen(buff) - 1] = '\0';
		if (addressCheck(buff)) dest_addr.sin_addr.s_addr = inet_addr(buff); 
	}

	// Если не удалось определить порт из аргуметов
	while (dest_addr.sin_port == 0) {
		memset(buff, '\0', MAXLINE);
		printf("Enter server port: ");
		fgets(buff, MAXLINE, stdin);
		buff[strlen(buff) - 1] = '\0';
		if (portCheck(buff)) dest_addr.sin_port = htons(atoi(buff)); 
	}

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

		// Попытка отправить пакет
		if (sendto(sock, &msg, sizeof(msg), 0, (const struct sockaddr *) &dest_addr, sizeof(dest_addr)) < 0) {
			printf("ERROR: Cannot send message to the server.\n\n");
			continue;
		}

		// Говорим, что всё ок
		printf("Message sent successfully.\n");

		// Если необходимо завершить работу
		if (!strcmp(msg.filename, "shutdown")) break;

		// Говорим, что ждём сервер
		printf("Wait for the server...\n");

		// Если пришёл ответ
		if (recvfrom(sock, &msg, sizeof(msg), 0, (struct sockaddr *) &dest_addr, &structlen) < 0) {
			printf("ERROR: Cannot receive answer from the server.\n\n");
			continue;
		}

		// Выводим на экран ответ сервера
		printf("Answer from the server: \n%s.\n\n", msg.filename);
	}

	// Закрываем сокет
	close(sock);

	exit(0);
}
