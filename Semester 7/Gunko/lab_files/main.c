#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

// Размер буфера, считывающего входной файл
#define BUF_SIZE 3
// Максимальная длина строки пути к входному файлу
#define FILEPATH_SIZE 128

// Записывает данные, но проверяет, что они действительно были записаны
int safeWrite(int file, void* buf, int count) {
    if (write(file, buf, count) != count) {
        printf ("Couldn't write data to the output file.\n"); 
        exit(-1);
    }
    return count;
}

int main (int argc, char * argv [ ]) {
    // Дескрипторы файлов 
    int inputFile, outputFile;
    // Имя выходного файла (или путь к нему)
    char outputFileName[FILEPATH_SIZE];
    
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

    // Указывает на начало переписываемого участка из буфера
    unsigned short j;
    // Флаг, говорящий о том, что в предыдущем считанном буфере была непустая строка
    unsigned char flag = 0;
    // Сбрасываем счетчик изменений в файле
    unsigned short counter = 0;
    // Буфер, который хранит заданный символ и символ перехода '\n'
    char tmp_buf[2];
    // Количество считанных символов
    unsigned short numRead;
    // Буфер, в который будет считываться файл
    char buf[BUF_SIZE];

    // Считываем весь входной файл буфером последовательно
    while ((numRead = read (inputFile, buf, BUF_SIZE)) > 0) {
        // Проходим по буферу
        j = 0;
        for (unsigned short i = 0; i < numRead; i++) {
            if (buf[i] == '\n') {
                // Переписываем участок буфера, являющийся одной сплошной строкой
                safeWrite(outputFile, buf + j, i - j);
                // Если строка была не пустая, то добавляем заданный символ и инкрементируем счетчик
                if ((i - j) > 0 || flag) {
                    tmp_buf[0] = argv[2][0]; // Кладём заданный символ
                    tmp_buf[1] = buf[i]; // '\n'
                    safeWrite(outputFile, tmp_buf, 2);
                    counter++;
                    flag = 0;
                } 
                // Если строка была пустая, то просто переписываем символ новой строки, то есть игнорируем
                else safeWrite(outputFile, buf + i, 1);
                // Запоминаем, с какого адреса в буфере потом копировать
                j = i + 1;
            } 
            // Если в буфере что-то осталось, переписываем это в выходной файл и выставляем флаг, 
            // мол строка была не пустая
            else if (i == (numRead - 1)) {
                safeWrite(outputFile, buf + j, i - j + 1);
                flag = 1;
            }
        }
    }

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
    printf("Done. Number of changes is %d.\nOutput file is \"%s\".\n", counter, outputFileName);

    // Завершаем 
    exit(counter);
}