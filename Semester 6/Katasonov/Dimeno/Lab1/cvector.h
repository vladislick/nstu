#pragma once

// Добавляем библиотеку с типами данных вроде uint16_t
#include <stdint.h>

template <typename myType>
// Класс cvector, массив, где элементы лежат единым блоком
class cvector {
private:
    // Текущий размер массива
    uint32_t size;
    // Указатель на массив
    myType* array;
public:
    // Объявление класса итератора
    class iterator {
        friend class cvector;
    private:
        int32_t index;
        // Скрытый от пользователя конструктор
        iterator(int32_t address) {
            index = address;
        }
    public:
        iterator() {
            // По умолчанию указывать на начало массива
            index = 0;
        }

        const iterator& operator++(int) {
            index++;
            return *this;
        }

        const iterator& operator--(int) {
            index--;
            return *this;
        }

        bool operator!=(const iterator& it) {
            return index != it.index;
        }

        bool operator==(const iterator& it) {
            return index == it.index;
        }

        bool operator<(const iterator& it) {
            return index < it.index;
        }

        bool operator>(const iterator& it) {
            return index > it.index;
        }
    };

    // Конструктор объекта класса по умолчанию
    cvector() {
        size = 0;
        array = nullptr;
    }

    // Конструктор с заранее определённой длиной
    cvector(uint32_t arraySize) {
        size = arraySize;
        array = new myType[arraySize];
    }

    // Конструктор копирования
    cvector(const cvector& object) {
        // Создание копии массива obj
        size  = object.size;
        array = new myType[size];
        // Копирование элементов массива obj
        for (uint32_t i = 0; i < size; i++) array[i] = object.array[i];
    }

    //Деструктор объекта класса
    ~cvector() {
        // Удаление предыдущего массива, если он был
        if (array != nullptr) delete array;
    }

    // Определение оператора присваивания
    cvector& operator=(const cvector& object) {
        // Удаление предыдущего массива, если он был
        if (array != nullptr) delete array;
        // Создание копии массива obj
        size  = object.size;
        array = new myType[size];
        // Копирование элементов массива obj
        for (uint32_t i = 0; i < size; i++) array[i] = object.array[i];
        // Возращение указателя на этот объект класса
        return *this;
    }

    // Определение оператора обращения по индексу
    myType& operator[](uint32_t index) {
        // Возвращаем элемент
        return array[index];
    }

    // Определение оператора обращения по индексу
    myType& operator[](const iterator& it) {
        // Возвращаем элемент
        return array[it.index];
    }

    // Вернуть указатель на начало массива
    iterator begin() {
        return 0;
    }

    // Вернуть указатель на конец массива
    iterator end() {
        return size;
    }

    // Вернуть указатель на начало массива (реверсный)
    iterator rbegin() {
        return size - 1;
    }
    
    // Вернуть указатель на конец массива (реверсный)
    iterator rend() {
        return -1;
    }

    // Получить текущий размер массива
    uint32_t getSize() {
        return size;
    }
};