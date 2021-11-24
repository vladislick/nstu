#include <stdio.h>
#include <string.h>
#include <stdint.h>
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

// Кодирует число number в код с защитой повторением
uint16_t coder(uint16_t number) {
    return (number & 0xFF) | (number << 8);
}

// Декодирует код с защитой повторением в число
uint16_t decoder(uint16_t code) {
    uint16_t result;
    if ((code & 0xFF) == (code >> 8)) result = code & 0xFF; 
    else result = 0xFFFF;
    return result;
}

int main(int argc, char* argv[]) {
    char str[STR_MAX];
    uint8_t mode = 0xFF;
    uint16_t number = 0xFFFF;

    uint16_t M, n, k, d, N;
    float D;
    
    N = 2;
    M = 220;
    k = ceil(log2(M));
    n = k*N;
    d = N;
    D = 1.0 - 1.0/N;

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
        if (decoder(number) == 0xFFFF) printf("An error has occurred while decoding a message.\n");
        else printf("%d\n", decoder(number));
    } else if (mode == 2) {
        BinToStr(coder(number), str);
        printf("%s\n", str);
    }
    
    return 0;
}