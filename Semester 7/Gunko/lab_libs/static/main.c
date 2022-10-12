#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

// Максимальная длина строки пути к входному файлу
#define FILEPATH_SIZE 128

extern int fileHandler(int input, int output, char character);

int main (int argc, char * argv [ ]) {
    // Дескрипторы файлов 
    int inputFile, outputFile;
    // Имя выходного файла (или путь к нему)
    char outputFileName[FILEPATH_SIZE];
    // Количество замен в выходном файле
    int result;
    
    // Если неверное количество аргументов
    if (argc != 3) {
        printf("Usage: %s input_file character.\n", argv[0]); 
        exit(-1);
    }
    
    // Открытие входного файла 
    inputFile = open (argv[1], O_RDONLY);
    if (inputFile == -1) {
        printf ("Error opening input file \"%s\".\n", argv[1]) ; 
        exit(-1);
    }

    // Открытие выходного файла 
    strcpy(outputFileName, argv[1]);
    strcat(outputFileName, ".modified");
    // Создаем файл, если его нет, с правами rx-rx-rx-
    outputFile = open (outputFileName, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (outputFile == -1) {
        printf ("Error opening output file \"%s\".\n", outputFileName) ; 
        exit(-1);
    }

    // Проверка на заданный символ
    if (strlen(argv[2]) != 1) {
        printf("Error incorrect character.\n");
        exit(-1);
    }

    // Обработка файла
    result = fileHandler(inputFile, outputFile, argv[2][0]);
    

    // Закрываем входной файл
    if (close (inputFile) == -1 ) {
        printf ("Error closing input file.\n"); 
        exit(-1);
    }
    
    // Закрываем выходной файл
    if (close (outputFile) == -1 ) {
        printf ("Error closing output file.\n"); 
        exit(-1);
    }
    
    // Говорим, что всё хорошо
    printf("Done. Number of changes is %d.\nOutput file is \"%s\".\n", result, outputFileName);

    // Завершаем 
    exit(result);
}