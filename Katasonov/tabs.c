#include <stdio.h>
#include <string.h>

// Максимальная длина строки
#define STR_MAX 256

// Вставляет в строку source строку fragment по индексу index
void strinsert(char* source, int index, char* fragment) {
    // Проходим по всем символам fragment в обратном порядке
    for (int i = strlen(fragment) - 1; i >= 0; i--) {
        // Сдвигаем строку source от index'а на один символ вправо
        for (int j = strlen(source); j >= index; j--)
            source[j+1] = source[j];
        // Вставляем символ из fragment в пустое место
        source[index] = fragment[i];
    }
}

int main(int argc, char* argv[]) {
    // Структура данных исходного файла
    FILE *sourceFile = NULL;
    // Структура данных обработанного файла
    FILE *destinationFile = NULL;
    // Имя исходного файла
    char sourceFileName[STR_MAX];   
    strcpy(sourceFileName, ""); 
    // Имя обработанного файла
    char destinationFileName[STR_MAX];
    strcpy(destinationFileName, "");
    // Считываемый символ из файла
    char rCh;
    
    // Берём имя файла из аргумента программы, если есть
    if (argc > 1) strcpy(sourceFileName, argv[1]);

    // Запускаем цикл по чтению файла
    while(1) {
        // Если имя файла не задано
        if (!strlen(sourceFileName)) {
            printf(">>> Enter the filename: ");
            // Считываем имя файла, введённое пользователем
            fgets(sourceFileName, STR_MAX, stdin);
            // Убираем символ перевода строки из имени файла
            for (int i = 0; sourceFileName[i] != '\0'; i++) 
                if (sourceFileName[i] == '\n' || sourceFileName[i] == '\r') sourceFileName[i] = '\0';
            // Выходим из программы, если введено "exit"
            if (!strcmp(sourceFileName, "exit")) return 0;
        }
        // Формируем имя выходного файла
        strcpy(destinationFileName, sourceFileName);
        for (int i = 0; i < strlen(destinationFileName) + 1; i++)
            if (destinationFileName[i] == '.' || destinationFileName[i] == '\0') {
                strinsert(destinationFileName, i, "_mod");
                break;
            }
        // Если не удалось открыть
        if ((sourceFile = fopen(sourceFileName, "r")) == NULL) {
            // Выводим сообщение об ошибке
            printf("Cannot open input file \"%s\". Please, enter a different filename.\n", sourceFileName);
            // Очищаем строку с именем файла
            strcpy(sourceFileName, "");
        } 
        // Если не удалось открыть файл назначения на запись
        else if ((destinationFile = fopen(destinationFileName, "w")) == NULL) {
            // Выводим сообщение об ошибке
            printf("Cannot open output file \"%s\". Please, enter a different filename.\n", destinationFileName);
            // Очищаем строку с именем файла
            strcpy(sourceFileName, "");
            // Закрываем входной файл
            fclose(sourceFile);
        }
        else break;
    }

    // Заполняем выходной файл
    while((rCh = fgetc(sourceFile)) != EOF) {
        if (rCh == ' ') rCh = '\t';
        fputc(rCh, destinationFile);
    }
    
    // Выводим сообщение об успешной обработке файла
    printf("Done.\nDestination filename is \"%s\".\nPress ENTER to exit...", destinationFileName);

    // Делаем паузу
    getchar();

    // Закрываем входной файл
    fclose(sourceFile);
    // Закрываем выходной файл
    fclose(destinationFile);
    return 0;
}