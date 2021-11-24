#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Задаём модификацию кода
const uint8_t mod[4] = { 5, 3, 2, 1 };
// Таблица соответствия десятичных цифр и BCD-кода
uint8_t table[10];
// Максимальное кол-во десятичных цифр в сообщении
uint8_t d = 3;

// Заполнить таблицу соответствия
void fillTable() {
    uint8_t binary, decimal;
    for (uint8_t i = 0; i < 10; i++) {
        binary = 0;
        decimal = i;
        for (uint8_t j = 0; j < 4; j++) {
            if ((decimal - mod[j]) >= 0) {
                decimal -= mod[j];
                binary |= 1 << (3 - j);
            }
            if (decimal == 0) break;
        }
        table[i] = binary;
        //printf("%d -> ", i);
        //for (int8_t k = 3; k >= 0; k--) 
        //    printf("%c", 48 + ((table[i] & (1 << k)) > 0));
        //printf("\n");
    }
}

// Возвращает message в двоично-десятичном коде
uint16_t DecToBCD(uint16_t message) {
    uint16_t result = 0;
    for (uint8_t i = 0; i < d; i++)
        result |= table[message/(uint16_t)powf((uint8_t)10,i) % 10] << (4*i);
    return result;
}

// Возвращает message в десятичном коде
uint16_t BCDToDec(uint16_t message) {
    uint16_t result = 0;
    uint8_t status;
    for (uint8_t i = 0; i < d; i++) {
        status = 0;
        for (uint8_t j = 0; j < 10; j++) {
            if ((message & 0xF) == table[j]) {
                result += j * (uint16_t)powf((uint8_t)10, i);
                status = 1;
                message = message >> 4;
                break;
            }
        }
        if (status == 0) return 0xFFFF;
    }
    return result;
}

// Преобразует BCD-код из строки в числовую переменную
uint16_t StrToBCD(char* str) {
    uint16_t result = 0;
    uint8_t length = 0;
    uint8_t index = 0;
    // Находим размер строки
    for (uint8_t i = 0; i < 256; i++) {
        if (str[i] == '\0') {
            length = i;
            break;
        }
    }

    for (int16_t i = length - 1; i >= 0; i--) {
        if (str[i] == '_') continue;
        if (str[i] == '1') result |= 1 << index;
        index++;
    }

    return result;
}

// Вывести двоично-десятичное число
void printBCD(uint16_t number) {
    for (int8_t i = d * 4 - 1; i >= 0; i--) {
        printf("%c", 48 + ((number & (1 << i)) > 0));
        if (i % 4 == 0 && i != 0) printf("_");
    }
}

int main(int argv, char* argc[]) {
    char cmd[256];
    uint8_t dialog = 0;
    uint16_t memory = 0;
    uint8_t memoryType = 0; // 0 - Двоично-десятичный, 1 - Десятичный

    fillTable();

    printf("Welcome! ");
    printf("BCD modification is %d-%d-%d-%d.\n", mod[0], mod[1], mod[2], mod[3]);
    printf("");
    
    while(1) {
        if (dialog == 0) printf("CMD");
        else printf("ARG");
        printf("$ ");
        scanf("%s", cmd);

        if (strlen(cmd) == 0) continue;

        if (dialog == 0) {
            if (!strcmp(cmd, "decode")) {
                dialog = 1;
                printf("Which BCD number you would like to decode?\n");
            } else if (!strcmp(cmd, "encode")) {
                dialog = 2;
                printf("Which decimal number you would like to encode?\n");
            } else if (!strcmp(cmd, "memory")) {
                if (memoryType) {
                    printf("Memory is %d (Decimal).\n", memory);
                } else {
                    printf("Memory is ");
                    if (BCDToDec(memory) != 0xFFFF) printf("%d ", BCDToDec(memory));
                    printf("[");
                    printBCD(memory);
                    printf("] (Binary-Decimal).\n");
                }
            } else if (!strcmp(cmd, "bcd")) {
                printf("Which BCD number you would like to store?\n");
                dialog = 3;
            } else if (!strcmp(cmd, "decimal") || !strcmp(cmd, "dec")) {
                printf("Which decimal number you would like to store?\n");
                dialog = 4;
            } else if (!strcmp(cmd, "clear")) {
                printf("Memory cleared.");
                memoryType = 0;
                memory = 0;
            } else if (!strcmp(cmd, "distort")) {
                if (memoryType) {
                    printf("This function is available only\
                    for BCD number in memory.\n");
                    continue;
                }
                printf("Which bit of BCD message you would like to distort?\n");
                dialog = 5;
            } else if (!strcmp(cmd, "exit")) {
                return 0;
            }
        } else if (dialog == 1 || dialog == 2) {
            if (!strcmp(cmd, "memory")) {}
            else if (atoi(cmd) > 0) {
                if (dialog == 2) memory = atoi(cmd);
                else memory = StrToBCD(cmd);
            }
            else if (atoi(cmd) == 0) {
                if (cmd[0] == '0') memory = 0;
                else {
                    printf("Incorrect input.\n");
                    continue;
                }
            }

            if (dialog == 2) {
                memory = DecToBCD(memory);
                memoryType = 0;
                printf("Encoded number is "); printBCD(memory); printf(".\n");
                printf("It is saved in memory.\n");
                dialog = 0;
            } else {
                if (BCDToDec(memory) == 0xFFFF) {
                    printf("Found an error while decoding number.\n");
                    memoryType = 0;
                    dialog = 0;
                    continue;
                }
                memory = BCDToDec(memory);
                memoryType = 1;
                printf("Decoded number is %d.\n", memory);
                printf("It is saved in memory.\n");
                dialog = 0;
            }
        } else if (dialog == 3) {
            if (atoi(cmd) > 0 || (atoi(cmd) == 0 && cmd[0] == '0')) {
                memory = StrToBCD(cmd);
                memoryType = 0;
                printf("BCD number ");
                if (BCDToDec(memory) != 0xFFFF) printf("%d ", BCDToDec(memory));
                printf("[");
                printBCD(memory);
                printf("] saved in memory.\n");
                dialog = 0;
            } else {
                printf("Incorrect input.\n");
                continue;
            }
        } else if (dialog == 4) {
            if (atoi(cmd) > 0 || (atoi(cmd) == 0 && cmd[0] == '0')) {
                memory = atoi(cmd);
                memoryType = 1;
                printf("Decimal number %d saved in memory.\n", memory);
                dialog = 0;
            } else {
                printf("Incorrect input.\n");
                continue;
            }
        } else if (dialog == 5) {
            if ((atoi(cmd) > 0 && atoi(cmd) < 16) || (atoi(cmd) == 0 && cmd[0] == '0')) {
                printf("Memory before distort is ");
                if (BCDToDec(memory) != 0xFFFF) printf("%d ", BCDToDec(memory));
                printf("[");
                printBCD(memory);
                printf("].\n");
                if (memory & (1 << atoi(cmd)))
                    memory &= ~(1 << atoi(cmd));
                else
                    memory |= (1 << atoi(cmd));
                printf("Memory after distort is ");
                if (BCDToDec(memory) != 0xFFFF) printf("%d ", BCDToDec(memory));
                printf("[");
                printBCD(memory);
                printf("].\n");
                dialog = 0;
            } else {
                printf("Incorrect input. Correct input is from 0 to 15.\n");
                continue;
            }
        }
    }
}