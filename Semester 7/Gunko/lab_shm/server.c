#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>

// Имя семафора, которым управляет клиент
#define SEM_CLIENT "/client"
// Имя семафора, которым управляет сервер
#define SEM_SERVER "/server"
// Имя области разделяемой памяти
#define SHM "/shared_memory"
// Права доступа к семафорам
#define SEM_FILE_MODE S_IRUSR | S_IWUSR
// Права доступа к разделяемой памяти
#define SHM_FILE_MODE S_IRUSR | S_IWUSR
// Флаги для открытия семафора сервера
#define SEM_SERVER_FLAGS O_RDWR | O_CREAT
// Флаги для открытия семафора клиента
#define SEM_CLIENT_FLAGS O_RDWR | O_CREAT
// Флаги для разделяемой памяти
#define SHM_FLAGS O_RDWR | O_CREAT

#define MAXLINE 256

// Структура сообщений
struct msg_struct {
	char filename[MAXLINE];
	char symbol;
};

// Отобразить текст str на экран и отправить его в разделяемую память msg, уведомив клиента семафором sem
int display(sem_t* sem, struct msg_struct* msg, const char* str) {
	// Выводим на экран
	printf("%s.\n\n", str);
	// Очищаем память
	memset(msg, 0, sizeof(struct msg_struct));
	// Копируем строку
	strcpy(msg->filename, str);
	// Говорим клиенту, что можно забирать сообщение
	if (sem_post(sem) < 0) {
		printf("ERROR: Cannot touch server's semaphore: %s.\n", strerror(errno));
		return -1;
	}
	return 0;
}

int main(int argc, char** argv) {
	// =============== ПЕРЕМЕННЫЕ СЕМАФОРОВ И ПАМЯТИ ===============
	// Создаём дескрипторы семафоров
	sem_t *sem_client, *sem_server;
	// Указатель на разделяемую память
	struct msg_struct* shm_ptr;
	// Дескриптор разделяемой памяти
	int fd;

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

	// Открываем семафор клиента, если не существует, создаём
	sem_client = sem_open(SEM_CLIENT, SEM_CLIENT_FLAGS, SEM_FILE_MODE, 0);
	if (sem_client == SEM_FAILED) {
		printf("ERROR: Cannot open client's semaphore: %s.\n", strerror(errno));
		exit(-1);
	}

	// Открываем семафор сервера, если не существует, создаём
	sem_server = sem_open(SEM_SERVER, SEM_SERVER_FLAGS, SEM_FILE_MODE, 0);
	if (sem_server == SEM_FAILED) {
		printf("ERROR: Cannot open server's semaphore: %s.\n", strerror(errno));
		exit(-1);
	}

	// Открываем или создаём разделяемую память
	fd = shm_open(SHM, SHM_FLAGS, SHM_FILE_MODE);
	if (fd < 0) {
		printf("ERROR: Cannot open shared memory: %s.\n", strerror(errno));
		exit(-1);
	}

	// Обозначаем длину разделяемой памяти
	ftruncate(fd, sizeof(struct msg_struct));

	// Отображаем память в адресное пространство процесса
	shm_ptr = mmap(NULL, sizeof(struct msg_struct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shm_ptr == MAP_FAILED) {
		printf("ERROR: Cannot map shared memory: %s.\n", strerror(errno));
		exit(-1);
	}

	// Говорим, что готовы обрабатывать клиентов
	printf("Wait for the clients...\n\n"); fflush(stdout);

	while(1) {
		// Ожидание сообщения от клиента
		if (sem_wait(sem_client) < 0) {
			printf("ERROR: Cannot wait from client's semaphore: %s.\n", strerror(errno));
			continue;
		}

		// Если пришло сообщение о завершении работы
		if (!strcmp(shm_ptr->filename, "shutdown")) break;

		// Выводим сообщение на экран
		printf("Received message: File: <%s>, Symbol: <%c>.\n", shm_ptr->filename, shm_ptr->symbol);

		// Открытие входного файла 
		inputFile = open(shm_ptr->filename, O_RDONLY);
		if (inputFile == -1) {
			sprintf(buff, "ERROR: Cannot open input file \"%s\": %s", shm_ptr->filename, strerror(errno));
			display(sem_server, shm_ptr, buff);
			continue;
		}

		// Находим имя выходного файла
		strcpy(outputFileName, shm_ptr->filename);
		strcat(outputFileName, ".modified");

		// Создаем файл, если его нет, с правами rx-rx-rx-
		outputFile = open (outputFileName, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		if (outputFile == -1) {
			sprintf(buff, "ERROR: Cannot open output file \"%s\": %s", outputFileName, strerror(errno));
			display(sem_server, shm_ptr, buff);
			continue;
		}

		// Обработка файла
		result = fileHandler(inputFile, outputFile, shm_ptr->symbol);

		// Закрываем входной файл
		if (close (inputFile) == -1 ) {
			sprintf(buff, "ERROR: Cannot close input file: %s", strerror(errno));
			display(sem_server, shm_ptr, buff);
			continue;
		}

		// Закрываем выходной файл
		if (close (outputFile) == -1 ) {
			sprintf(buff, "ERROR: Cannot close output file: %s", strerror(errno));
			display(sem_server, shm_ptr, buff);
			continue;
		}

		// Отправляем ответ клиенту
		sprintf(buff, "Done. %d changes saved in %s", result, outputFileName);
		display(sem_server, shm_ptr, buff);
	}
	
	// Уведомляем, что пришла команда завершения работы
	printf("Shutdown command received.\n");

	// Закрываем дескрипторы семафоров
	sem_close(sem_client);
	sem_close(sem_server);

	// Удаляем семафоры
	sem_unlink(SEM_CLIENT);
	sem_unlink(SEM_SERVER);

	// Закрываем дескриптор динамической библиотеки
	dlclose(libfiles);

	// Закрываем дескриптор разделяемой памяти
	close(fd);

	// Удаляем сегмент разделяемой памяти
	shm_unlink(SHM);

	exit(0);
}
