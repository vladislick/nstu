#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// Преобразует двоичное число в строку
void BinToStr(uint32_t code, char* str) {
    uint8_t index = 0, length = 0;
    uint32_t temp = code;
    if (temp) while(temp != 1) { temp >>= 1; length++; }
    for (int8_t i = 31; i >= 0; i--) {
        if (code & (1 << i)) 
            str[index++] = '1';
        else
            str[index++] = '0';
        //if (i % 4 == 0 && i != 0) str[index++] = '\'';    
    }
    str[index++] = '\0';
}

// Возвращает размер таблицы синдромов
uint16_t syndromeSelectorSize(uint8_t d, uint8_t n) {
    uint8_t r = (d - 1) / 2, count;
    uint32_t t1, t2;
    uint16_t index = 0;
    for (uint8_t i = 0; i < r; i++) {
        // Если вес вектора ошибок r = 1
        if (i == 0) index++;
        // Если вес вектора ошибок r = 2
        else if (i == 1) for (uint8_t j = 0; j < (n - 1); j++) index++;
        // Если вес вектора ошибок r > 2
        else {
            t1 = 1 << (n - 1);
            // Начинаем перебирать все возможные комбинации
            for (uint32_t j = 0; j < t1; j++) {
                // Узнаём вес комбинации
                count = 0; t2 = j;
                for (; t2; count++) t2 &= t2 - 1;
                // Если комбинация подходит, добавляем в таблицу
                if (count == i) index++;
            }
        }
    }
    return index;
}

// Создание таблицы синдромов из условия
uint8_t syndromeSelectorCreate(uint32_t* tableS, uint16_t tableSize, uint8_t d, uint8_t n, uint32_t gx) {
    // Создаём таблицу векторов ошибок
    uint32_t* tableV = calloc(tableSize, 4);
    uint32_t t1, t2;
    uint16_t index = 0, count;
    uint8_t r = (d - 1) / 2;
    char str[128];

    // Находим крайний старший бит
    t1 = 1 << (n - 1);

    // Заполняем таблицу векторов ошибок
    for (uint8_t i = 0; i < r; i++) {
        // Если вес вектора ошибок r = 1
        if (i == 0) tableV[index++] = t1;
        // Если вес вектора ошибок r = 2
        else if (i == 1) for (uint8_t j = 0; j < (n - 1); j++) tableV[index++] = t1 | (1 << j);
        // Если вес вектора ошибок r > 2
        else {
            // Начинаем перебирать все возможные комбинации
            for (uint32_t j = 0; j < t1; j++) {
                // Узнаём вес комбинации
                count = 0; t2 = j;
                for (; t2; count++) t2 &= t2 - 1;
                // Если комбинация подходит, добавляем в таблицу
                if (count == i) tableV[index++] = t1 | j;
            }
        }
    }

    /*for (uint16_t i = 0; i < tableSize; i++) {
        BinToStr(tableV[i], str);
        printf("tableV[%d] = %s\n", i, str);
    }*/

    // Заполняем таблицу синдромов
    uint32_t workspace, code;
    for (uint16_t j = 0; j < tableSize; j++) {
         // Определяем реальную длину порождающего полинома
        t2 = gx; index = 0; workspace = 0;
        while(t2 != 1) { t2 >>= 1; index++; }
        code = tableV[j];

        for (uint8_t i = 0; i < n; i++) {
            workspace <<= 1;
            if (code & t1) workspace |= 1;
            code <<= 1;
            
            if (workspace & (1 << index)) workspace ^= gx;
        }

        tableS[j] = workspace;
    }

    t2 = 0;
    for (uint16_t i = 0; i < tableSize; i++) {
        for (uint16_t j = 0; j < tableSize; j++) {
            if (j == i) continue;
            if (tableS[i] == tableS[j]) t2 = 1;
        }
    }

    /*for (uint16_t i = 0; i < tableSize; i++) {
        BinToStr(tableS[i], str);
        printf("tableS[%d] = %s\n", i, str);
    }*/

    free(tableV);
    return t2;
}

// Выводит все подходящие порождающие полиномы
void findGx(uint8_t k, uint8_t d, uint8_t maxcount) {
    char str[256];
    uint32_t gx, temp, count, index, workspace, n;
    uint32_t poly;

    // Перебираем все возможные полиномы
    for (gx = (1 << d) - 1; gx < pow(2, sizeof(gx)*8); gx++) {
        // Обнуляем все переменные
        count = index = workspace = 0;
        
        // Проверяем вес получившегося полинома
        temp = gx;
        for (; temp; count++) temp &= temp - 1;
        if (count < d) continue;

        temp = gx;
        while(temp != 1) { temp >>= 1; index++; }

        n = k + index;
        poly = (1 << n) - 1;

        for (uint8_t i = 0; i < 32; i++) {
            workspace <<= 1;
            if (poly & (1 << 31)) workspace |= 1;
            poly <<= 1;
            
            if (workspace & (1 << index)) workspace ^= gx;
        }

        if (workspace) continue;

        uint16_t tableSize = syndromeSelectorSize(d, n);
        uint32_t* table = calloc(tableSize, 4);
        if (syndromeSelectorCreate(table, tableSize, d, n, gx)) {
            free(table);
            continue;
        }
        free(table);

        // Выводим в алгебраическом виде
        index = 0;
        for (int8_t i = 31; i >= 0; i--) {
            if (gx & (1 << i)) {
                if (index) {
                    str[index++] = ' ';
                    str[index++] = '+';
                    str[index++] = ' ';
                }
                if (i != 0) {
                    str[index++] = 'x';
                    if (i != 1) {
                        str[index++] = '^';
                        if (i > 9) {
                            str[index++] = 48 + i / 10;
                            str[index++] = 48 + i % 10;
                        } else str[index ++] = 48 + i;
                    }    
                } else str[index++] = '1';
            }
        }
        str[index++] = '\0';

        printf("g(x) may be:\n\t%s,\n\t", str);
        BinToStr(gx, str);
        printf("%s,\n\tThe code is (%d, %d, %d).\n", str, n, k, d);
        
        if (maxcount > 1) maxcount--;
        else break;
    }
}

int main() {
    // Найти подходящие порождающие полиномы 
    // (кол-во информационных бит, кодовое расстояние, количество)
    findGx(11, 7, 2);
    //printf("%d\n", syndromeSelectorSize(11, 28));
}