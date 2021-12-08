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
            result |= (str[i] - 48) << index++;
        }
    }
    return result;
}

// Преобразует двоичное число в строку
void BinToStr(uint16_t code, char* str, uint16_t length) {
    uint8_t index = 0;
    for (uint8_t i = 0; i < length; i++) {
        if (i % 4 == 0 && i != 0) str[index++] = '\'';
        if (code & (1 << i)) 
            str[index++] = '1';
        else
            str[index++] = '0';    
    }
}

// Кодирует число number в код с защитой повторением
uint16_t coder(uint16_t number, uint16_t length) {
    uint16_t result = 0;
    // Генерируем маски для определения бит
    uint16_t maskI = 0, maskK = 0, maskM = 0;
    for (uint8_t i = 0; (uint8_t)pow(2, i) < length; i++)
        maskK |= (1 << ((uint8_t)pow(2, i) - 1));
    for (uint8_t i = 0; i < (length - 1); i++) {
        maskM <<= 1;
        maskM |= 1;
    }
    maskI = (~maskK) & maskM;

    // Генерируем массив для уравнений ГСК
    uint8_t numOfEq = 0;
    for (uint16_t n = maskK; n; numOfEq++)
        n &= (n - 1);
    uint16_t* solution = calloc(numOfEq, sizeof(uint16_t));
    uint8_t index, mode = 0; // 0 - заполнение нулями, 1 - заполнение единицами
    for (uint8_t i = 0; i < numOfEq; i++) {
        index = (uint8_t)pow(2, i);
        mode = 0;
        for (uint8_t j = 0; j < (length - 1); j++) {
            if ((j + 1) % index == 0) mode = !mode;
            if (mode) solution[i] |= (1 << j);
            else solution[i] &= ~(1 << j);
        }
    }

    // Заносим сообщение в результат по положению бит
    result = 0;
    uint8_t j = 0;
    for (uint8_t i = 0; i < (length - 1 - numOfEq); i++) {
        while ((maskI & (1 << j)) == 0) j++;
        if (number & (1 << ((length - 2 - numOfEq) - i))) result |= (1 << j);
        j++;
    }

    // Выставляем контрольные биты по уравнениям ГСК
    uint16_t temp;
    for (uint8_t i = 0; i < numOfEq; i++) {
        temp = result & solution[i];
        uint8_t count = 0;
        for (; temp; count++) temp &= (temp - 1);
        if (count & 1) result |= (1 << ((uint8_t)pow(2, i) - 1));
    }

    // Выставляем бит чётности
    temp = result;
    uint8_t count = 0;
    for (; temp; count++) temp &= (temp - 1);
    if (count & 1) result |= (1 << (length - 1));

    /*char str[30];
    for (uint8_t i = 0; i < numOfEq; i++) {
        BinToStr(solution[i], str, length);
        printf("Solution %d is %s.\n", i, str);
    }
    BinToStr(maskI, str, length);
    printf("MaskI is %s.\n", str);
    BinToStr(maskK, str, length);
    printf("MaskK is %s.\n", str);
    BinToStr(number, str, length);
    printf("Message is %s.\n", str);
    BinToStr(result, str, length);
    printf("Result is %s.\n", str);*/
    return result;
}

// Декодирует код с защитой повторением в число
uint16_t decoder(uint16_t code, uint16_t length) {
    uint16_t result = 0;
    // Генерируем маски для определения бит
    uint16_t maskI = 0, maskK = 0, maskM = 0;
    for (uint8_t i = 0; (uint8_t)pow(2, i) < length; i++)
        maskK |= (1 << ((uint8_t)pow(2, i) - 1));
    for (uint8_t i = 0; i < (length - 1); i++) {
        maskM <<= 1;
        maskM |= 1;
    }
    maskI = (~maskK) & maskM;

    // Генерируем массив для уравнений ГСК
    uint8_t numOfEq = 0;
    for (uint16_t n = maskK; n; numOfEq++)
        n &= (n - 1);
    uint16_t* solution = calloc(numOfEq, sizeof(uint16_t));
    uint8_t index, mode = 0; // 0 - заполнение нулями, 1 - заполнение единицами
    for (uint8_t i = 0; i < numOfEq; i++) {
        index = (uint8_t)pow(2, i);
        mode = 0;
        for (uint8_t j = 0; j < (length - 1); j++) {
            if ((j + 1) % index == 0) mode = !mode;
            if (mode) solution[i] |= (1 << j);
            else solution[i] &= ~(1 << j);
        }
    }

    // Находим синдром ошибки по уравнениям ГСК
    uint16_t temp, sindrom = 0;
    for (uint8_t i = 0; i < numOfEq; i++) {
        temp = code & solution[i];
        uint8_t count = 0;
        for (; temp; count++) temp &= (temp - 1);
        if (count & 1) sindrom |= 1 << i;
    }

    // Проверяем бит чётности
    temp = code & maskM;
    uint8_t count = 0, parity = 0;;
    for (; temp; count++) temp &= (temp - 1);
    if ((count & 1) != (code >> (length - 1))) parity = 1;
    
    char str[30];

    // Если синдром ненулевой и бит чётности не совпаадет,
    // то корректируем соответствующий бит в сообщении
    if (parity && sindrom) {
        code ^= (1 << sindrom - 1);
    } else if (parity && !sindrom) {
        code ^= (1 << (length - 1));
    } else if (!parity && sindrom) {
        return 0xFFFF;
    }

    // Вычитываем сообщение в результат по положению бит
    result = 0;
    uint8_t j = 0;
    for (int8_t i = (length - 2 - numOfEq); i >= 0; i--) {
        while ((maskI & (1 << j)) == 0) j++;
        if (code & (1 << j)) result |= (1 << i);
        j++;
    }

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
    D = 0.4;

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
        uint16_t temp = decoder(number, n);
        if (temp == 0xFFFF) printf("An errors has occurred while decoding a message.\n");
        else printf("%d\n", temp);
    } else if (mode == 2) {
        BinToStr(coder(number, n), str, n);
        printf("%s\n", str);
    }
    
    return 0;
}