#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define STR_MAX 256

// Преобразует строку в число
uint16_t DecStrToInt(char* str) {
    uint16_t result = 0;
    for (uint8_t i = 0; str[i] != '\0'; i++) {
        if (str[i] < 48 || str[i] > 57) return 0xFFFF;
        result *= 10;
        result += str[i] - 48;
    }
    return result;
}

// Преобразует двоичную строку в число
uint16_t BinStrToInt(char* str) {
    uint16_t result = 0, index = 0;
    for (uint8_t i = 0; str[i] != '\0'; i++) {
        if (str[i] < 48 || str[i] > 49) {
            if (str[i] != ' ' && str[i] != '_') return 0xFFFF; 
        } else {
            result <<= 1;
            result |= str[i] - 48;
        }
    }
    return result;
}

// Преобразует двоичное число в строку
void BinToStr(uint16_t code, char* str) {
    uint8_t index = 0;
    for (int8_t i = 15; i >= 0; i--) {
        if (code & (1 << i)) 
            str[index++] = '1';
        else
            str[index++] = '0';
        if (i % 4 == 0 && i != 0) str[index++] = '_';    
    }
}

// Кодирует число number в несистематический код по полиному gx
uint32_t coder(uint16_t number, uint32_t gx) {
    uint32_t result = 0;
    uint32_t workspace = 0;
    uint32_t temp;
    uint8_t  count;
    
    // Осуществляем свертку number и gx
    for (int8_t i = 31; i >= 0; i--) {
        // Выполняем сдвиг
        workspace >>= 1;
        workspace |= (number & (1 << 15)) << 16; // Добавляем с конца
        number <<= 1;
        if (workspace == 0) continue; // Пока ноль, смысла проверять нет
        // Выполняем лоичесгкую сумму произведений 
        count = 0; temp = gx & workspace;
        for (; temp; count++) temp &= temp - 1;
        // Если результат нечетный, добавляем 1-цу в результат
        if (count & 1) result |= 1 << i;
    }

    return result;
}

uint16_t createSyndromeSelector(uint32_t* table, uint8_t tableSize, uint8_t n, uint32_t gx) {
    // Сначала заполняем table векторами ошибки с единичным старшим битом
    /*

    for (uint8_t i = 0; i < n; i++) {
        result <<= 1;

        workspace <<= 1;
        if (code & (1 << 31)) workspace |= 1;
        code <<= 1;
        
        if (workspace & (1 << index)) {
            result |= 1;
            workspace ^= gx;
        }

        BinToStr(workspace, str);
        printf("Workspace for i=%d is %s\n", i, str);

    }*/
}

// Декодирует code посредством деления code на gx
uint16_t decoder(uint32_t code, uint32_t gx) {
    uint16_t result = 0;
    uint16_t workspace = 0;
    uint32_t temp;
    uint8_t flag, index = 0;
    char str[64];

    // Определяем реальную длину порождающего полинома
    temp = gx;
    while(temp != 1) { temp >>= 1; index++; }

    for (uint8_t i = 0; i < 64; i++) {
        result <<= 1;

        workspace <<= 1;
        if (code & (1 << 31)) workspace |= 1;
        code <<= 1;
        
        if (workspace & (1 << index)) {
            result |= 1;
            workspace ^= gx;
        }

        BinToStr(workspace, str);
        printf("Workspace for i=%d is %s\n", i, str);

    }

    BinToStr(result, str);
    printf("Result: %s\n", str);
    BinToStr(workspace, str);
    printf("Modulo: %s\n", str);

    return result;
}

int main(int argc, char* argv[]) {
    char str[STR_MAX];
    uint8_t mode = 0xFF;
    uint16_t number = 0xFFFF;

    uint16_t M, n, k, d, N;
    float D;

    k = 6;
    n = 11;
    M = (uint16_t)pow(2, k);
    d = 4;
    D = 1 - (float)k/n;

    if (argc == 1) printf("M = %d\nn = %d\nk = %d\nd = %d\nD = %f\n", M, n, k, d, D);

    // Проверяем аргументы командной строки
    for (uint8_t i = 1; i < argc; i++) {
        strcpy(str, argv[i]);
        strlwr(str);
        if (!strcmp(str, "-d") || !strcmp(str, "--decoder") || !strcmp(str, "--decode"))
            mode = 1;
        else if (!strcmp(str, "-c") || !strcmp(str, "--coder") || !strcmp(str, "--encode"))
            mode = 2;
        else if (mode != 0xFF) {
            if (mode == 1) number = BinStrToInt(str);
            else if (mode == 2) number = DecStrToInt(str);
        }
    }

    while (mode == 0xFF) {
        printf("Do you want to encode or decode your message?\n");
        scanf("%s", str);
        strlwr(str);
        if (!strcmp(str, "decode") || !strcmp(str, "d") || !strcmp(str, "decoder")) mode = 1;
        else if (!strcmp(str, "encode") || !strcmp(str, "c") || !strcmp(str, "coder")) mode = 2;
    }

    while (number == 0xFFFF) {
        if (mode == 1) printf("Which message do you want to decode?\n");
        else if (mode == 2) printf("Which message do you want to encode?\n");
        scanf("%s", str);
        if (mode == 1) number = BinStrToInt(str);
        else if (mode == 2) number = DecStrToInt(str);
    }

    if (mode == 1) {
        uint16_t temp = decoder(number, 0b101011);
        if (temp == 0xFFFF) printf("An errors has occurred while decoding a message.\n");
        else printf("%d\n", temp);
    } else if (mode == 2) {
        BinToStr(coder(number, 0b101011), str);
        printf("%s\n", str);
    }
    
    return 0;
}