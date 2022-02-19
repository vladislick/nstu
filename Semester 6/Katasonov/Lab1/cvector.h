#ifndef CVECTOR_H
#define CVECTOR_H

// Добавляем библиотеку с типами данных вроде uint16_t
#include <stdint.h>

template <typename vType>
// Класс cvector, массив, где элементы лежат единым блоком
class cvector {
private:
    // Текущий размер массива
    uint32_t vSize;
    // Текущий индекс в массиве (первый пустой элемент)
    uint32_t vIndex;
    // Указатель на массив
    vType* vArray;
public:
    // Конструктор объекта класса по умолчанию
    cvector();
    // Конструктор с заранее определённой длиной
    cvector(uint32_t);
    // Конструктор копирования
    cvector(const cvector&);
    //Деструктор объекта класса
    ~cvector();

    // Определение оператора присваивания
    cvector& operator=(const cvector&);
    // Определение оператора обращения по индексу
    vType& operator[](uint32_t);

    // Получить элемент по индексу
    vType& get(uint32_t);
    // Заменить элемент по индексу
    void set(uint32_t, vType);
    // Выдавить последний элемент из массива
    vType pull();
    // Положить элемент в конец массива
    void push(vType&);
    // Удалить выбранный элемент
    void remove(uint32_t);
    // Получить текущий размер массива
    uint32_t size();
};

template <typename vType>
cvector<vType>::cvector() {
    vSize  = 0;
    vIndex = 0;
    vArray = nullptr;
}

template <typename vType>
cvector<vType>::cvector(uint32_t size) {
    vSize  = size;
    vIndex = vSize - 1;
    vArray = new vType[size];
}

template <typename vType>
cvector<vType>::cvector(const cvector& obj) {
    // Создание копии массива obj
    vSize  = obj.vSize;
    vIndex = obj.vIndex;
    vArray = new vType[vSize];
    // Копирование элементов массива obj
    for (uint32_t i = 0; i < vSize; i++) vArray[i] = obj.vArray[i];
}

template <typename vType>
cvector<vType>::~cvector() {
    // Удаление предыдущего массива, если он был
    if (vArray != nullptr) delete vArray;
}

template <typename vType>
cvector<vType>& cvector<vType>::operator=(const cvector& obj) {
    // Удаление предыдущего массива, если он был
    if (vArray != nullptr) delete vArray;
    // Создание копии массива obj
    vSize  = obj.vSize;
    vIndex = obj.vIndex;
    vArray = new vType[vSize];
    // Копирование элементов массива obj
    for (uint32_t i = 0; i < vSize; i++) vArray[i] = obj.vArray[i];
    // Возврат ссылки на этот объект
    return *this;
}

template <typename vType>
vType& cvector<vType>::operator[](uint32_t index) {
    // Создаем исключение в случае, если запрашивается несуществующий элемент
    if (index >= vSize) throw -1;
    // Возвращаем элемент
    return vArray[index];
}

template <typename vType>
vType& cvector<vType>::get(uint32_t index) {
    // Создаем исключение в случае, если запрашивается несуществующий элемент
    if (index >= vSize) throw -1;
    // Возвращаем элемент
    return vArray[index];
}

template <typename vType>
void cvector<vType>::set(uint32_t index, vType value) {
    // Если индекс указывает на несуществующий элемент
    if (index >= vSize) {
        // Создаём новый, увеличенный массив
        vType* temp = new vType[index + 1];
        // Копируем содержимое текущего массива
        for (uint32_t i = 0; i < vSize; i++) temp[i] = vArray[i];
        // Удаление предыдущего массива, если он был
        if (vArray != nullptr) delete vArray;
        // Указываем новые параметры массива
        vSize = index + 1;
        vIndex = index;
        vArray = temp;
    }
    // Замена значения по индексу index
    vArray[index] = value;
}

template <typename vType>
vType cvector<vType>::pull() {
    // Если массив пуст, генерировать исключение
    if (vSize == 0) throw -2;
    // Возвращаемое значение
    vType value = vArray[vIndex];
    // Создаём новый, уменьшенный массив
    vType* temp = new vType[vSize--];
    // Копируем содержимое текущего массива
    for (uint32_t i = 0; i < vIndex; i++) temp[i] = vArray[i];
    // Удаление предыдущего массива, если он был
    if (vArray != nullptr) delete vArray;
    // Указываем новые параметры массива
    if (vIndex > 0) vIndex--;
    vArray = temp;
    // Возвращаем значение элемента
    return value;
}

template <typename vType>
void cvector<vType>::push(vType& value) {
    // Создаём новый, увеличенный массив
    vType* temp = new vType[vSize + 1];
    // Копируем содержимое текущего массива
    for (uint32_t i = 0; i < vSize; i++) temp[i] = vArray[i];
    // Удаление предыдущего массива, если он был
    if (vArray != nullptr) delete vArray;
    // Указываем новые параметры массива
    vSize++;
    if (vSize > 1) vIndex++;
    vArray = temp;
    // Замена значения по индексу index
    vArray[vIndex] = value;
}

template <typename vType>
void cvector<vType>::remove(uint32_t index) {
    // Создаем исключение в случае, если запрашивается несуществующий элемент
    if (index >= vSize) throw -3;
    // Создаём новый, уменьшенный массив
    vType* temp = new vType[vSize - 1];
    // Копируем содержимое текущего массива до удаляемого элемента
    for (uint32_t i = 0; i < index; i++) temp[i] = vArray[i];
    // Копируем содержимое текущего массива после удаляемого элемента
    for (uint32_t i = index + 1; i < vSize; i++) temp[i - 1] = vArray[i];
    // Удаление предыдущего массива, если он был
    if (vArray != nullptr) delete vArray;
    // Указываем новые параметры массива
    if (vIndex > 0) vIndex--;
    vArray = temp;
    vSize--;
}

template <typename vType>
uint32_t cvector<vType>::size() {
    return vSize;
}

#endif