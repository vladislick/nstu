#include <sys/types.h> 
#include <sys/stat.h> 
#include <wait.h> 
#include <fcntl.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define FIFO1 "/tmp/fifo1"
#define FIFO2 "/tmp/fifo2"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define MAXLINE 256

int main () {
	// Создаём дескрипторы каналов
    int readfd, writefd;
    // Буфер для обмена информацией через канал
	char filename[MAXLINE];
    char buff[MAXLINE];
    char symbol;
    // Длина строки
    int length;

	// Количество считанных байт данных
	int n = 0;

    printf("Waiting for the server...\n");

    // Пробуем открыть канал для записи
    writefd = open(FIFO1, O_WRONLY, 0);

    // Пробуем открыть канал для чтения
    readfd = open(FIFO2, O_RDONLY, 0);
    
	if (writefd < 0 || readfd < 0) {
		printf("Cannot open pipe(s)\n");
		return -1;
	}

    while(1) {
        // Получаем имя файла
        printf("Enter the filename: ");
        fgets(filename, MAXLINE, stdin);
        length = strlen(filename);
        // Заменим последний символ на конец строки
        filename[length - 1] = '\0';

        // Получаем символ для обработки
        do {
            printf("Enter the symbol: ");
            fgets(buff, MAXLINE, stdin);
            buff[strlen(buff) - 1] = '\0';
        } while(strlen(buff) != 1);
        
        // Получаем реальный символ
        symbol = buff[0];

        memset(buff, '\0', MAXLINE);
        strcpy(buff, filename);
        strcat(buff, "|");
        buff[strlen(buff)] = symbol;

        // Отправляем имя файла серверу
        write(writefd, buff, strlen(buff) + 1);

        // Ожидаем ответа от сервера
        n = read(readfd, buff, MAXLINE);
        if (n > 0) printf("Answer from server: %s\n\n", buff);
        else printf("Error while receiving answer from server\n\n");
    }

	close(readfd);
	close(writefd);
	unlink(FIFO1);
	unlink(FIFO2);
	exit(0);
}
