#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// Размер буфера, считывающего входной файл
#define BUF_SIZE 3

// Записывает данные, но проверяет, что они действительно были записаны
int safeWrite(int file, void* buf, int count) {
    if (write(file, buf, count) != count) {
        printf ("Couldn't write data to the output file.\n"); 
        exit(-1);
    }
    return count;
}

// Обработать файл
int fileHandler(int input, int output, char character) {
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
    while ((numRead = read (input, buf, BUF_SIZE)) > 0) {
        // Проходим по буферу
        j = 0;
        for (unsigned short i = 0; i < numRead; i++) {
            if (buf[i] == '\n') {
                // Переписываем участок буфера, являющийся одной сплошной строкой
                safeWrite(output, buf + j, i - j);
                // Если строка была не пустая, то добавляем заданный символ и инкрементируем счетчик
                if ((i - j) > 0 || flag) {
                    tmp_buf[0] = character; // Кладём заданный символ
                    tmp_buf[1] = buf[i]; // '\n'
                    safeWrite(output, tmp_buf, 2);
                    counter++;
                    flag = 0;
                } 
                // Если строка была пустая, то просто переписываем символ новой строки, то есть игнорируем
                else safeWrite(output, buf + i, 1);
                // Запоминаем, с какого адреса в буфере потом копировать
                j = i + 1;
            } 
            // Если в буфере что-то осталось, переписываем это в выходной файл и выставляем флаг, 
            // мол строка была не пустая
            else if (i == (numRead - 1)) {
                safeWrite(output, buf + j, i - j + 1);
                flag = 1;
            }
        }
    }
    // Возвращаем количество замен
    return counter;
}