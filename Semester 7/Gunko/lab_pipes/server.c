#include <sys/types.h> 
#include <sys/stat.h> 
#include <wait.h> 
#include <fcntl.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>

#define FIFO1 "/tmp/fifo1"
#define FIFO2 "/tmp/fifo2"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define MAXLINE 256
#define FILEPATHMAX 128

// Создаёт канал fifo и выводит сообщение об ошибке, если возникнет
void mkfifo_safe(const char* fifo) {
	if ((mkfifo(fifo, FILE_MODE) < 0) && (errno != EEXIST)) {
		printf("Cannot create pipe %s.", fifo);
		exit(-1);
	}
}

int main() {
    // Создаём дескрипторы каналов
	int readfd, writefd;
	// Дескрипторы файлов 
	int inputFile, outputFile;
	// Имя входного файла (или путь к нему)
	char inputFileName[FILEPATHMAX];
	// Имя выходного файла (или путь к нему)
	char outputFileName[FILEPATHMAX];
	// Количество замен в выходном файле
	int result;
	// Символ для обработки 
	char symbol;
	// Буфер для обмена информацией через канал
	char buff[MAXLINE];
	// Количество считанных байт данных
	int n = 0;
	// Дескриптор динамической библиотеки
	void* libfiles;
	// Прототип функции из библиотеки
	int (*fileHandler)(int input, int output, char character);

	printf("Waiting for the client...\n");
	
    // Создаём два именованных канала
	mkfifo_safe(FIFO1);
	mkfifo_safe(FIFO2);
	// Открываем их на чтение и запись
	readfd = open(FIFO1, O_RDONLY, 0);
	writefd = open(FIFO2, O_WRONLY, 0);

	while(1) {
		// Чтение имени файла как только оно появится в канале
		if ((n = read(readfd, buff, MAXLINE)) == 0) {
			printf(buff, "End-of-file while reading pathname.");
			printf("%s\n", buff);
        	write(writefd, buff, strlen(buff) + 1);
			continue;
		}
		printf("Received message: %s\n", buff);
		
		// Разделяем полученные данные на имя файла и символ для обработки
		int i;
		memset(inputFileName, '\0', FILEPATHMAX);
		for (i = 0; buff[i] != '|'; i++) inputFileName[i] = buff[i];
		symbol = buff[++i];

		// Попытка загрузить библиотеку
		libfiles = dlopen("./libfiles.so", RTLD_LAZY);
		if (!libfiles) {
			sprintf(buff, "Error opening shared library: %s.", dlerror());
			printf("%s\n", buff);
        	write(writefd, buff, strlen(buff) + 1);
			continue;
		}

		// Открытие входного файла 
		inputFile = open(inputFileName, O_RDONLY);
		if (inputFile == -1) {
			sprintf(buff, "Error opening input file \"%s\".", inputFileName);
			printf("%s\n", buff);
        	write(writefd, buff, strlen(buff) + 1);
			continue;
		}

		// Открытие выходного файла 
		strcpy(outputFileName, inputFileName);
		strcat(outputFileName, ".modified");
		// Создаем файл, если его нет, с правами rx-rx-rx-
		outputFile = open (outputFileName, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		if (outputFile == -1) {
			sprintf(buff, "Error opening output file \"%s\".", outputFileName);
			printf("%s\n", buff);
        	write(writefd, buff, strlen(buff) + 1);
			continue;
		}

		// Поиск нужной функции
		fileHandler = dlsym(libfiles, "fileHandler");	

		// Обработка файла
		result = fileHandler(inputFile, outputFile, symbol);

		// Закрываем динамическую библиотеку
		dlclose(libfiles);
		

		// Закрываем входной файл
		if (close (inputFile) == -1 ) {
			sprintf(buff, "Error closing input file.");
			printf("%s\n", buff);
        	write(writefd, buff, strlen(buff) + 1);
			continue;
		}
		
		// Закрываем выходной файл
		if (close (outputFile) == -1 ) {
			sprintf(buff, "Error closing output file.");
			printf("%s\n", buff);
        	write(writefd, buff, strlen(buff) + 1);
			continue;
		}

		// Отправляем ответ клиенту
		sprintf(buff, "%d changes saved in %s", result, outputFileName);
		printf("%s\n", buff);
		write(writefd, buff, strlen(buff) + 1);
	}
	
	return 0;
}