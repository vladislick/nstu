#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

// Таблица соответствия BCD и DEC кодов
uint8_t table[10];
// Модификация BCD кода
uint8_t mod[4] = { 8, 4, 2, 1 };

// Заполнить таблицу соответствия согласно модификации
void tableCreate(uint8_t* table, uint8_t* mod) {
    // Впомогательная переменная
    uint8_t number;
    // Проходим по всем 10-ти цифрам
    for (uint8_t i = 0; i < 10; i++) {
        table[i] = 0;
        number = i;
        // Проходим по всей модификации кода
        for (uint8_t j = 0; j < 4; j++) {
            if (number >= mod[j]) {
                number -= mod[j];
                table[i] |= 1 << (3 - j);
            }
        }
    }
}

// Переводит BCD число в строку str
void BCDToStr(uint16_t number, char* str, uint8_t digits) {
    uint8_t index = 0;
    for (int8_t i = digits*4-1; i >= 0; i--) {
        str[index++] = 48 + ((number & (1 << i)) > 0);
        if ((i % 4) == 0 && i != 0) str[index++] = '_';
    }
}

// Переводит строку str в BCD код
uint16_t StrToBCD(char* str) {
    uint8_t index = 0;
    uint16_t result = 0;
    for (int8_t i = strlen(str)-1; i >= 0; i--) {
        if (str[i] == '1') result |= 1 << index++;
        else if (str[i] == '0') index++;
    }
    return result;
}

// Закодировать DEC в BCD код
uint16_t coder(uint16_t decimal, uint8_t digits) {
    uint16_t result = 0;
    for (uint8_t i = 0; i < digits; i++)
        result |= table[decimal / (uint16_t)pow(10, i) % 10] << (4*i);
    return result;
}

// Декодировать BCD в DEC код
uint16_t decoder(uint16_t bcd, uint8_t digits) {
    uint16_t result = 0;
    for (uint8_t i = 0; i < digits; i++) {
        uint8_t flag = 1;
        for (uint8_t j = 0; j < 10; j++)
            if (table[j] == (bcd & 0b1111)) {
                result += j * (uint16_t)pow(10, i);
                flag = 0;
            }
        bcd >>= 4; if (flag) return 0xFFFF;
    }
    return result;
}

int main(int argc, char* argv[]) {
    tableCreate(table, mod);
    uint16_t M = 820;
    uint8_t n = ceil(log10(M))*4;
    uint16_t k = n;
    uint16_t d = 1;
    float D = 1 - (log10(M))/(log10(pow(2, k)));
    // Текущая команда
    uint8_t command = 0;
    // Аргумент
    uint16_t argument = 0xFFFF;
    char str[30];
    uint16_t result;

    printf("M = %d\n", M);
    printf("n = %d\n", n);
    printf("k = %d\n", k);
    printf("d = %d\n", d);
    printf("D = %f\n", D);

    // Проверяем аргументы
    for (uint8_t i = 1; i < argc; i++) {
        strcpy(str, argv[i]);
        strlwr(str);
        if (!strcmp(str, "encode") || !strcmp(str, "coder") \
        || !strcmp(str, "--encode") || !strcmp(str, "-e")) {
            command = 1;
        }
        else if (!strcmp(str, "decode") || !strcmp(str, "decoder") \
        || !strcmp(str, "--decode") || !strcmp(str, "-d")) {
            command = 2;
        }
        else if (str[0] > 47 && str[0] < 58) {
            if (command == 1) argument = atof(str);
            else if (command == 2) argument = StrToBCD(str);
        }
    }

    while(1) {
        if (command == 0) {
            printf("CMD$ ");
            scanf("%s", str);
            strlwr(str);
            if (!strcmp(str, "encode")) command = 1;
            else if (!strcmp(str, "decode")) command = 2;
            else if (!strcmp(str, "exit")) return 0;
        }
        // Coder
        if (command == 1) {
            printf("Coder selected.\n");
            if (argument == 0xFFFF) {
                printf("ARG$ ");
                scanf("%s", str);
                argument = atof(str);
            }
            result = coder(argument, n / 4);
            BCDToStr(result, str, n / 4);
            printf("Result is %s.\n", str);
        }
        // Encoder
        if (command == 2) {
            printf("Decoder selected.\n");
            if (argument == 0xFFFF) {
                printf("ARG$ ");
                scanf("%s", str);
                argument = StrToBCD(str);
            }
            result = decoder(argument, n / 4);
            if (result != 0xFFFF) printf("Result is %d.\n", result);
            else printf("Result is broken.\n");
        }

        if (argc > 1) return 0;
        command = 0;
        argument = 0xFFFF;
    }
}