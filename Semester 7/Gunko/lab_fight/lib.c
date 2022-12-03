#include <unistd.h>
#include <stdio.h>

// Размер буфера, считывающего входной файл
#define BUF_SIZE 3

// Вычисление контрольной суммы файла
int checkSum(int fd) {
    // Количество считанных байт
    int n = 0;
    // Буфер, в который будет считываться файл
    char buf[BUF_SIZE];
    // Сумма всех символов файла fd
    int result = 0;

    // Считываем весь файл буфером последовательно
    while ((n = read(fd, buf, BUF_SIZE)) > 0)
        for (int i = 0; i < n; i++) result += buf[i];

    // Возвращаем количество замен
    return result;
}