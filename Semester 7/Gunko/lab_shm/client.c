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

// Структура для обмена данными
struct msg_struct {
	char filename[MAXLINE];
	char symbol;
};

int main () {
	// =============== ПЕРЕМЕННЫЕ СЕМАФОРОВ И ПАМЯТИ ===============
	// Создаём дескрипторы семафоров
    sem_t *sem_client, *sem_server;
	// Дескриптор разделяемой памяти
	int fd;
	// Указатель на разделяемую память
	struct msg_struct* shm_ptr;

	// =============== ПЕРЕМЕННЫЕ ВСПОМОГАТЕЛЬНЫЕ ===============
	// Сообщение для отправки на сервер
	struct msg_struct msg;
	// Буфер для получения символа	
	char buff[MAXLINE];
	
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
