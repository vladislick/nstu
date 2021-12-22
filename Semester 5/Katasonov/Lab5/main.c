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
uint32_t BinStrToInt(char* str) {
    uint32_t result = 0;
    for (uint8_t i = 0; str[i] != '\0'; i++) {
        if (str[i] < 48 || str[i] > 49) {
            if (str[i] != ' ' && str[i] != '_' && str[i] != '\'') return 0xFFFF; 
        } else {
            result <<= 1;
            result |= str[i] - 48;
        }
    }
    return result;
}

// Преобразует двоичное число в строку
void BinToStr(uint32_t code, char* str) {
    uint8_t index = 0, flag = 0;
    for (int8_t i = 31; i >= 0; i--) {
        if (code & (1 << i)) {
            str[index++] = '1';
            flag = 1;
        }
        else if (flag) str[index++] = '0';
        if (i % 4 == 0 && i != 0 && flag) str[index++] = '_';    
    }
    str[index++] = '\0';
}

// Кодирует число number в несистематический код по полиному gx
uint32_t coder(uint16_t number, uint32_t gx) {
    uint32_t result = 0;
    uint32_t workspace = 0;
    uint32_t temp;
    uint8_t  count;
    
    // Осуществляем свертку number и gx
    for (int8_t i = 46; i >= 0; i--) {
        // Выполняем сдвиг
        workspace >>= 1;
        workspace |= ((uint32_t)(number & (1 << 15))) << 16; // Добавляем с конца
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

// Возвращает размер таблицы синдромов
uint32_t syndromeSelectorSize(uint8_t d, uint8_t n) {
    uint8_t r = (d - 1) / 2, done, set;
    uint32_t count = 0;
    for (uint8_t i = 0; i < r; i++) {
        if (i == 0) count++;
        else if (i == 1) count += n - 1;
        else {
            done = 1;
            // Создаём массив для единиц
            uint8_t* word = calloc(i, 1);
            
            // Инициализируем массив
            for (uint8_t j = 0; j < i; j++) word[j] = j;
            count++;
            
            // Проходим по массиву
            while(done) {
                done = 0;
                // Проходим по всем единицам
                for (int8_t k = i - 1; k >= 0; k--) {
                    // Если все единицы, начиная с k-той идут подряд без нулей
                    set = 1;
                    for (int8_t p = k; p > 0; p--)
                        if ((word[p]-1) != word[p - 1]) set = 0;
                    if (set) {
                        if (word[k] == (n - 2)) break;
                        word[k]++;
                        count++;
                        done = 1;

                        // Возвращаем все предыдущие единицы в начало
                        for (int8_t j = (k - 1); j >= 0; j--) word[j] = j;
                    }
                }
            }

            // Удаляем массив
            free(word);
        }
    }
    return count;
}

// Создание таблицы синдромов из условия
uint8_t syndromeSelectorCreate(uint64_t* tableS, uint32_t tableSize, uint8_t d, uint8_t n, uint64_t gx) {
    // Создаём таблицу векторов ошибок
    uint64_t* tableV = calloc(tableSize, 8);
    uint64_t t1, t2;
    uint32_t index = 0;
    uint8_t r = (d - 1) / 2, done, set;
    char str[128];

    // Находим крайний старший бит
    t1 = (uint64_t)1 << (n - 1);
    
    for (uint8_t i = 0; i < r; i++) {
        if (i == 0) tableV[index++] = t1;
        else if (i == 1) for (uint8_t j = 0; j < (n - 1); j++) tableV[index++] = t1 | (1 << j);
        else {
            done = 1;
            // Создаём массив для единиц
            uint8_t* word = calloc(i, 1);
            
            // Инициализируем массив
            tableV[index] = t1;
            for (uint8_t j = 0; j < i; j++) {
                word[j] = j;
                tableV[index] |= 1 << j;
            }
            index++;
            
            // Проходим по массиву
            while(done) {
                done = 0;
                // Проходим по всем единицам
                for (int8_t k = i - 1; k >= 0; k--) {
                    // Если все единицы, начиная с k-той идут подряд без нулей
                    set = 1;
                    for (int8_t p = k; p > 0; p--)
                        if ((word[p]-1) != word[p - 1]) set = 0;
                    if (set) {
                        if (word[k] == (n - 2)) break;
                        word[k]++;
                        done = 1;

                        // Возвращаем все предыдущие единицы в начало
                        for (int8_t j = (k - 1); j >= 0; j--) word[j] = j;

                        // Добавляем вектор ошибки
                        tableV[index] = t1;
                        for (uint8_t j = 0; j < i; j++) tableV[index] |= (uint64_t)1 << word[j];
                        index++;
                    }
                }
            }

            // Удаляем массив
            free(word);
        }
    }

    // Заполняем таблицу синдромов
    uint64_t workspace, code;
    for (uint32_t j = 0; j < tableSize; j++) {
         // Определяем реальную длину порождающего полинома
        t2 = gx; index = 0; workspace = 0;
        while(t2 != 1) { t2 >>= 1; index++; }
        code = tableV[j];

        for (uint8_t i = 0; i < n; i++) {
            workspace <<= 1;
            if (code & t1) workspace |= 1;
            code <<= 1;
            
            if (workspace & ((uint64_t)1 << index)) workspace ^= gx;
        }

        tableS[j] = workspace;
    }

    t2 = 0;
    for (uint32_t i = 0; i < tableSize; i++) {
        for (uint32_t j = 0; j < tableSize; j++) {
            if (j == i) continue;
            if (tableS[i] == tableS[j]) t2 = 1;
        }
    }

    free(tableV);
    return t2;
}

// Декодирует code посредством деления code на gx
uint16_t decoder(uint32_t code, uint32_t gx, uint8_t n, uint64_t* table, uint32_t tableSize) {
    uint16_t result = 0;
    uint32_t codeFixed = 0;
    uint32_t workspace = 0;
    uint32_t temp;
    uint8_t  index = 0;
    char str[64];

    // Определяем реальную длину порождающего полинома
    temp = gx;
    while(temp != 1) { temp >>= 1; index++; }
    temp = code;

    for (uint8_t i = 0; i < (2*n); i++) {
        // Если остаток от деления уже получен
        if (i >= n) { 
            temp = 1 << (n - 1 - (i - n));
            codeFixed |= code & temp;
            if (workspace != 0)
                for (uint32_t j = 0; j < tableSize; j++) 
                    if (table[j] == workspace) 
                        codeFixed ^= temp;
            temp = 0;
        }

        workspace <<= 1;
        if (temp & (1 << (n - 1))) workspace |= 1;
        temp <<= 1;
        
        if (workspace & (1 << index)) workspace ^= gx;
    }

    workspace = 0; temp = codeFixed, result = 0;
    for (uint8_t i = 0; i < n; i++) {
        result <<= 1;

        workspace <<= 1;
        if (temp & (1 << (n - 1))) workspace |= 1;
        temp <<= 1;
        
        if (workspace & (1 << index)) {
            workspace ^= gx;
            result |= 1;
        }
    }

    BinToStr(code, str);
    printf("    Input code is %s\n", str);
    BinToStr(codeFixed, str);
    printf("Corrected code is %s\n", str);

    return result;
}

int main(int argc, char* argv[]) {
    char str[STR_MAX];
    uint8_t mode = 0xFF;
    uint32_t number = 0xFFFF;
    uint64_t* table = NULL;
    uint32_t tableSize;

    uint16_t M, n, k, d, N;
    float D;

    M = 2048;
    // Параметры кода
    n = 31; k = 11; d = 11;
    // Порождающий полином
    uint32_t gx = 0b100001100101100111011;
    //D = 1 - (float)k/n;

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
        // Для использования декодера необходима таблица синдромов
        if (table == NULL) {
            tableSize = syndromeSelectorSize(d, n);
            table = calloc(tableSize, 8);
            syndromeSelectorCreate(table, tableSize, d, n, gx);
            /*printf("Table size is %d\n", tableSize);
            for (uint16_t j = 0; j < tableSize; j++) {
                BinToStr(table[j], str);
                printf("Table[%d] = %s\n", j, str);
            }*/
        }
        uint16_t temp = decoder(number, gx, n, table, tableSize);
        if (temp == 0xFFFF) printf("An errors has occurred while decoding a message.\n");
        else printf("%d\n", temp);
    } else if (mode == 2) {
        BinToStr(coder(number, gx), str);
        printf("%s\n", str);
    }
    
    free(table);
    return 0;
}