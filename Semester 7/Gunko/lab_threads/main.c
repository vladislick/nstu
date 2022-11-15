#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>

#define MAXLINE 256

// Дескриптор динамической библиотеки
void* libfiles;
// Прототип функции из библиотеки
int (*fileHandler)(int input, int output, char character);

// Глобальный мьютекс (сразу инициализируем)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Структура сообщений (для передачи данных потомкам)
struct msg_struct {
    // Порядковый номер потока
    int thread_index;
    // Имя файла для обработки
    char filename[MAXLINE];
    // Символ, используемый при обработке
    char symbol;
    // Результат выполнения
    int result;
};

// Функция-обработчик файла
void* child(void* info) {
    // Блокируем мьютекс
    pthread_mutex_lock(&mutex);
    // Копируем указатель в качестве указателя на структуру сообщения 
    struct msg_struct* msg = info;
    // Дескрипторы файлов 
	int inputFile, outputFile;
	// Имя выходного файла (или путь к нему)
	char outputFileName[MAXLINE];

    // Открытие входного файла 
    inputFile = open(msg->filename, O_RDONLY);
    if (inputFile == -1) {
        printf("<THREAD #%d> ERROR: Cannot open input file \"%s\": %s.\n", msg->thread_index, msg->filename, strerror(errno));
        // Разлокируем мьютекс
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    // Находим имя выходного файла
    strcpy(outputFileName, msg->filename);
    strcat(outputFileName, ".modified");

    // Создаем файл, если его нет, с правами rx-rx-rx-
    outputFile = open (outputFileName, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (outputFile == -1) {
        printf("<THREAD #%d> ERROR: Cannot open output file \"%s\": %s.\n", msg->thread_index, outputFileName, strerror(errno));
        // Разлокируем мьютекс
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    // Обработка файла
    msg->result = fileHandler(inputFile, outputFile, msg->symbol);

    // Закрываем входной файл
    if (close (inputFile) == -1 ) {
        printf("<THREAD #%d> ERROR: Cannot close input file \"%s\": %s.\n", msg->thread_index, msg->filename, strerror(errno));
        // Разлокируем мьютекс
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    // Закрываем выходной файл
    if (close (outputFile) == -1 ) {
        printf("<THREAD #%d> ERROR: Cannot close output file \"%s\": %s.\n", msg->thread_index, outputFileName, strerror(errno));
        // Разлокируем мьютекс
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    // Выводим радостное сообщение
    printf("<THREAD #%d> Good job, done.\n", msg->thread_index);

    // Разлокируем мьютекс
    pthread_mutex_unlock(&mutex);

    // Возвращаем результат
	return &msg->result;
}


int main (int argc, char * argv[]) {
    // Указатель на массив сообщений, передаваемых порождённым процессам
    struct msg_struct* msgs;

    // Указатель на массив дескрипторов потока
    pthread_t* threads;

    // Результат выполнения потока
    void* result;

	// Выводим подсказку, если недостаточно аргументов
	if (argc < 3) {
	    printf("Usage: %s file1 file2 ... character\n", argv[0]);
	    exit(-1);
    }
	
	// Проверяем, что последний аргумент - это символ
	if (strlen(argv[argc - 1]) != 1) {
	    printf("ERROR: There is no character entered.\n");
	    exit(-1);
	}

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

    // Выделяем память под дескрипторы потоков
    threads = calloc(argc - 2, sizeof(pthread_t));

    // Выделяем память под сообщения для потоков
    msgs = calloc(argc - 2, sizeof(struct msg_struct));

	// Запускаем цикл по всем файлам
	for (int i = 1; i < (argc - 1); i++) { 
        // Формируем сообщение для потока
        strcpy(msgs[i - 1].filename, argv[i]);  // Имя входного файла
        msgs[i - 1].symbol = argv[argc - 1][0];    // Символ для обработки
        msgs[i - 1].thread_index = i - 1;       // Порядковый номер потока

        // Запускаем поток
		if (pthread_create(&threads[i - 1], NULL, child, &msgs[i - 1]) != 0) {
            printf("ERROR: Cannot create thread #%d.\n", i - 1);
		    exit(-1);
        }
	}

    sleep(1);

    // Запускаем цикл ожидания работы всех потоков
	for (int i = 0; i < (argc - 2); i++) { 
        // Ожидаем завершения потока
        pthread_join(threads[i], &result);
        if (result == NULL) 
            // Выводим грустное сообщение
            printf("Thread #%d was unable to process the file \"%s\".\n", i, msgs[i].filename);
        else
            // Выводим радостное сообщение
            printf("Thread #%d done. %d changes written in output file \"%s.modified\".\n", i, *((int*)result), msgs[i].filename);
	}

    printf("Done.\n");

    // Высвобождаем память
    free(threads);
    free(msgs);

    return 0;
}