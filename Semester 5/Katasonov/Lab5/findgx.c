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
    for (int8_t i = length; i >= 0; i--) {
        if (code & (1 << i)) 
            str[index++] = '1';
        else
            str[index++] = '0';
        if (i % 4 == 0 && i != 0) str[index++] = '\'';    
    }
    str[index++] = '\0';
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
    findGx(11, 11, 3);
}