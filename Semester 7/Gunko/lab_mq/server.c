#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>

// Имя очереди сообщений от клиента серверу
#define CLIENT_MQ "/toserver.mq"
// Имя очереди сообщений от сервера клиенту
#define SERVER_MQ "/toclient.mq"
// Права доступа к очереди сообщений
#define FILE_MODE S_IRUSR | S_IWUSR
// Флаги для открытия очереди сервера
#define SERVER_FLAGS O_WRONLY | O_CREAT | O_EXCL
// Флаги для открытия очереди клиента
#define CLIENT_FLAGS O_RDONLY 

#define MAXLINE 256

// Структура сообщений
struct msg_struct {
	char filename[MAXLINE];
	char symbol;
};

// Отобразить текст str на экран и отправить его в очередь сообщений mqd_t
int display(mqd_t mq, const char* str) {
	// Выводим на экран
	printf("%s.\n\n", str);
	// Отправляем сообщение клиенту
	if (mq_send(mq, str, strlen(str) + 1, 0) < 0) {
		printf("ERROR: Cannot sent message to the client: %s.\n", strerror(errno));
		return -1;
	}
	return 0;
}

int main(int argc, char** argv) {
	// =============== ПЕРЕМЕННЫЕ ОЧЕРЕДИ СООБЩЕНИЙ ===============
    // Указатель на сообщение сообщение
	struct msg_struct* msg;
	// Создаём дескрипторы очередей сообщений 
    mqd_t client_mq, server_mq;
    // Сообщение от клиента
    void* client_msg;
    // Атрибуты очереди сообщений
	struct mq_attr attr;
	// Флаги для открытия дескриптора сервера
	int server_flags = SERVER_FLAGS;
    // =============== ПЕРЕМЕННЫЕ ОБРАБОТКИ ФАЙЛОВ  ===============
	// Дескрипторы файлов 
	int inputFile, outputFile;
	// Имя выходного файла (или путь к нему)
	char outputFileName[MAXLINE];
	// Количество замен в выходном файле
	int result;
	// Буфер для обмена информацией через канал
	char buff[MAXLINE*2];
	// Количество считанных байт данных
	int n = 0;
	// Дескриптор динамической библиотеки
	void* libfiles;
	// Прототип функции из библиотеки
	int (*fileHandler)(int input, int output, char character);
	
	// Проверка аргументов
	if (argc > 1) {
		// Если указан флаг --force (-f)
		if (argc == 2 && !strcmp(argv[1], "--remove")) {
			// Удаляем очередь сообщений сервера
			if (mq_unlink(SERVER_MQ) < 0) {
				printf("ERROR: Cannot remove message queue of the server: %s.\n", strerror(errno));
				exit(-1);
			}
			printf("Message queue of the server removed successfully.\n");
			exit(0);
		} else {
			// Выводим подсказку
			printf("Usage: %s [ --remove ].\n", argv[0]);
			exit(-1);
		}
	}
	
	// Попытка загрузить библиотеку
	libfiles = dlopen("./libfiles.so", RTLD_LAZY);
	if (!libfiles) {
		printf("ERROR: Cannot open shared library: %s.\n", dlerror());
		exit(-1);
	}
	
    // Открываем очередь сообщений сервера с атрибутами по умолчанию
	server_mq = mq_open(SERVER_MQ, server_flags, FILE_MODE, NULL);
	if (server_mq < 0) {
		printf("ERROR: Cannot open server's message queue: %s.\n", strerror(errno));
		if (errno == EEXIST) printf("If you want to remove an existing one, run this application with the --remove argument.\n");
		exit(-1);
	}

	// Открываем очередь сообщений клиента
	printf("Wait for the client message queue to appear..."); fflush(stdout);
	while((client_mq = mq_open(CLIENT_MQ, CLIENT_FLAGS)) < 0) sleep(1);
	printf("OK.\n\n");
	// Получаем атрибуты очереди сообщений клиента
	mq_getattr(client_mq, &attr);
	// Выделяем память под сообщение клиента
	client_msg = malloc(attr.mq_msgsize);

	while(1) {
		// Чтение сообщения от клиента
		if ((n = mq_receive(client_mq, client_msg, attr.mq_msgsize, NULL)) < 0) {
			printf("ERROR: Cannot receive message: %s.\n", strerror(errno));
			continue;
		} else if (n == 0) {
			printf("Received message is empty.\n");
			continue;
		}
		
		// Читаем сообщение
		msg = client_msg;
		// Если пришло сообщение о завершении работы
		if (!strcmp(msg->filename, "shutdown")) break;
		// Выводим сообщение на экран
		printf("Received message: File: <%s>, Symbol: <%c>.\n", msg->filename, msg->symbol);

		// Открытие входного файла 
		inputFile = open(msg->filename, O_RDONLY);
		if (inputFile == -1) {
			sprintf(buff, "ERROR: Cannot open input file \"%s\"", msg->filename);
			display(server_mq, buff);
			continue;
		}

		// Открытие выходного файла 
		strcpy(outputFileName, msg->filename);
		strcat(outputFileName, ".modified");
		// Создаем файл, если его нет, с правами rx-rx-rx-
		outputFile = open (outputFileName, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		if (outputFile == -1) {
			sprintf(buff, "ERROR: Cannot open output file \"%s\"", outputFileName);
			display(server_mq, buff);
			continue;
		}

		// Поиск нужной функции
		fileHandler = dlsym(libfiles, "fileHandler");	

		// Обработка файла
		result = fileHandler(inputFile, outputFile, msg->symbol);

		// Закрываем входной файл
		if (close (inputFile) == -1 ) {
			sprintf(buff, "ERROR: Cannot close input file");
			display(server_mq, buff);
			continue;
		}
		
		// Закрываем выходной файл
		if (close (outputFile) == -1 ) {
			sprintf(buff, "ERROR: Cannot close output file");
			display(server_mq, buff);
			continue;
		}

		// Отправляем ответ клиенту
		sprintf(buff, "Done. %d changes saved in %s", result, outputFileName);
		display(server_mq, buff);
	}
	
	// Уведомляем, что пришла команда завершения работы
	printf("Shutdown command received.\n");
	// Высвобождаем память под сообщение
	free(client_msg);
	// Закрываем дескрипторы очередей
	mq_close(server_mq);
	mq_close(client_mq);
	// Удаляем очереди сообщений
	if (mq_unlink(SERVER_MQ) < 0) 
		printf("ERROR: Cannot remove message queue of server: %s.\n", strerror(errno));
	if (mq_unlink(CLIENT_MQ) < 0) 
		printf("ERROR: Cannot remove message queue of clients: %s.\n", strerror(errno));
	// Закрываем дескриптор динамической библиотеки
	dlclose(libfiles);
	exit(0);
}
