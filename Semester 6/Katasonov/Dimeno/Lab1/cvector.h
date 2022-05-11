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
    public:
        // Конструктор по умолчанию
        iterator() {
            index = 0;
        }

        // Конструктор с параметром
        iterator(int address) {
            index = address;
        }

        const iterator& operator++(int) {
            index++;
            return *this;
        }

        const iterator& operator--(int) {
            index--;
            return *this;
        }

        const iterator& operator+=(int n) {
            index += n;
            return *this;
        }

        const iterator& operator-=(int n) {
            index -= n;
            return *this;
        }

        const iterator& operator=(int n) {
            index = n;
            return *this;
        }

        iterator operator+(int n) {
            return index + n;
        }

        iterator operator-(int n) {
            return index - n;
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

        bool operator<=(const iterator& it) {
            return index <= it.index;
        }

        bool operator>=(const iterator& it) {
            return index >= it.index;
        }

        int32_t getIndex() {
            return index;
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
    myType& operator[](const iterator& it) {
        // Возвращаем элемент
        return array[it.index];
    }

    // Вставить элемент в массив по итератору
    void insert(const iterator& it, myType value) {
        // Создаём новый массив, увеличенный на единицу
        myType *temp = new myType[size+1];
        // Копируем в новый массив элементы до итератора
        for (uint32_t i = 0; i < it.index; i++) temp[i] = array[i];
        // Копируем в новый массив элементы после итератора
        for (uint32_t i = it.index + 1; i < (size+1); i++) temp[i] = array[i-1];
        // Копируем в новый массив вставляемое значение
        temp[it.index] = value;
        // Удаляем текущий массив
        delete array;
        // Присваиваем в качестве текущего массива только что созданный
        array = temp;
        // Увеличиваем размер массива
        size++;
    }

    // Удалить элемент в массиве по итератору
    void erase(const iterator& it) {
        // Создаём новый массив, уменьшенный на единицу
        myType *temp = new myType[size-1];
        // Копируем в новый массив элементы до итератора
        for (uint32_t i = 0; i < it.index; i++) temp[i] = array[i];
        // Копируем в новый массив элементы после итератора
        for (uint32_t i = it.index; i < (size-1); i++) temp[i] = array[i+1];
        // Удаляем текущий массив
        delete array;
        // Присваиваем в качестве текущего массива только что созданный
        array = temp;
        // Уменьшаем размер массива
        size--;
    }

    // Пересоздать массив с новым размером
    void setSize(uint32_t n) {
         // Удаление предыдущего массива, если он был
        if (array != nullptr) delete array;
        // Создание нового массива
        size = n;
        array = new myType[size];
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
    uint32_t length() {
        return size;
    }
};