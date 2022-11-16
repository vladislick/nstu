#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>

#define MAXLINE 256

// Структура сообщений
struct msg_struct {
	char filename[MAXLINE];
	char symbol;
};

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
	return 0;
}

// Отобразить текст str на экран и отправить его в разделяемую память msg, уведомив клиента семафором sem
int display(int sock, struct sockaddr_in* addr, struct msg_struct* msg, const char* str) {
	// Выводим на экран
	printf("%s.\n", str);
	// Очищаем память
	memset(msg, 0, sizeof(struct msg_struct));
	// Копируем строку
	strcpy(msg->filename, str);
	// Говорим клиенту, что можно забирать сообщение
	if (sendto(sock, msg, sizeof(struct msg_struct), 0,(struct sockaddr *)addr, sizeof(struct sockaddr_in)) < 0) {
		printf("ERROR: Cannot send message to client.\n\n");
		return -1;
	}
	printf("\n");
	return 0;
}

int main(int argc, char** argv) {
	// =============== ПЕРЕМЕННЫЕ СОКЕТОВ ===============
	// Дескриптор сокета
	int sock;
	// Структура адреса сервера 
	struct sockaddr_in server_addr, client_addr;
	// Размер структуры адреса
	unsigned int structlen = sizeof(struct sockaddr_in); 
	// Указатель на разделяемую память
	struct msg_struct msg;

	// =============== ПЕРЕМЕННЫЕ ОБРАБОТКИ ФАЙЛОВ  ===============
	// Дескрипторы файлов 
	int inputFile, outputFile;
	// Имя выходного файла (или путь к нему)
	char outputFileName[MAXLINE];
	// Количество замен в выходном файле
	int result;
	// Буфер для обмена информацией через канал
	char buff[MAXLINE*2];
	// Дескриптор динамической библиотеки
	void* libfiles;
	// Прототип функции из библиотеки
	int (*fileHandler)(int input, int output, char character);

	// Попытка загрузить библиотеку
	libfiles = dlopen("./libfiles.so", RTLD_LAZY);
	if (libfiles == NULL) {
		printf("ERROR: Cannot open shared library: %s.\n", dlerror());
		exit(-1);
	}

	// Поиск нужной функции
	fileHandler = dlsym(libfiles, "fileHandler");
	if (fileHandler == NULL) {
		printf("ERROR: Cannot find function in shared library.\n");
		exit(-1);
	}

	// Создание сокета	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		printf("ERROR: Cannot create socket.\n");
		exit(-1);
	}

	// Создание адреса
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = 0;
	server_addr.sin_addr.s_addr = INADDR_ANY;

	 // Узнаем порт сервера из аргумента
	for (int i = 1; i < argc; i++) 
		if (portCheck(argv[i])) server_addr.sin_port = htons(atoi(argv[i]));
	
	// Если не удалось определить порт из аргуметов
	while (server_addr.sin_port == 0) {
		memset(buff, '\0', MAXLINE);
		printf("Enter server port: ");
		fgets(buff, MAXLINE, stdin);
		buff[strlen(buff) - 1] = '\0';
		if (portCheck(buff)) server_addr.sin_port = htons(atoi(buff)); 
	}

	// Попытка забиндить сокет
	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
		printf("ERROR: Cannot bind socket.\n");
		exit(-1);
	}

	// Говорим, что готовы обрабатывать клиентов
	printf("Wait for the clients...\n\n"); fflush(stdout);

	while(1) {
		// Ожидание сообщения от клиента
		if (recvfrom(sock, &msg, sizeof(msg), 0, (struct sockaddr *) &client_addr, &structlen) < 0) {
			printf("ERROR: Cannot receive client's message.\n");
			exit(-1);
		}

		// Если пришло сообщение о завершении работы
		if (!strcmp(msg.filename, "shutdown")) break;

		// Выводим сообщение на экран
		printf("Received message from [%s]:\nFile: <%s>, Symbol: <%c>.\n", inet_ntoa(client_addr.sin_addr),msg.filename, msg.symbol);

		// Открытие входного файла 
		inputFile = open(msg.filename, O_RDONLY);
		if (inputFile == -1) {
			sprintf(buff, "ERROR: Cannot open input file \"%s\": %s", msg.filename, strerror(errno));
			display(sock, &client_addr, &msg, buff);
			continue;
		}

		// Находим имя выходного файла
		strcpy(outputFileName, msg.filename);
		strcat(outputFileName, ".modified");

		// Создаем файл, если его нет, с правами rx-rx-rx-
		outputFile = open (outputFileName, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		if (outputFile == -1) {
			sprintf(buff, "ERROR: Cannot open output file \"%s\": %s", outputFileName, strerror(errno));
			display(sock, &client_addr, &msg, buff);
			continue;
		}

		// Обработка файла
		result = fileHandler(inputFile, outputFile, msg.symbol);

		// Закрываем входной файл
		if (close (inputFile) == -1 ) {
			sprintf(buff, "ERROR: Cannot close input file: %s", strerror(errno));
			display(sock, &client_addr, &msg, buff);
			continue;
		}

		// Закрываем выходной файл
		if (close (outputFile) == -1 ) {
			sprintf(buff, "ERROR: Cannot close output file: %s", strerror(errno));
			display(sock, &client_addr, &msg, buff);
			continue;
		}

		// Отправляем ответ клиенту
		sprintf(buff, "Done. %d changes saved in %s", result, outputFileName);
		display(sock, &client_addr, &msg, buff);
	}
	
	// Уведомляем, что пришла команда завершения работы
	printf("Shutdown command received.\n");

	// Закрываем дескриптор динамической библиотеки
	dlclose(libfiles);

	// Закрываем сокет
	close(sock);

	exit(0);
}
