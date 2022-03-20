#pragma once

template <typename lType>
class clist {
private:
    // Структура узла связанного списка
    struct lNode
    {
        // Указатель на следующий узел списка
        lNode *lNext;
        // Указатель на предыдущий узел списка
        lNode *lPrev;
        // Данные, которые хранит узел
        lType lData;
        // Конструктор структуры lNode
        lNode(lType &data) {
            lNext = lPrev = 0;
            lData = data;
        }
    };

    // Указатель на начало (голову) списка
    lNode* lHead;
    // Указатель на конец (хвост) списка
    lNode* lTail;

public:
    // Класс итератор
    class iterator {
        
    public:
        lNode* node;
        lNode* temp;

        iterator() {
            node = 0;
        }

        iterator(lNode* iNode) { 
            node = iNode; 
        }
        
        iterator& operator++() {
            node = node->lNext;
            return *this;
        }
        
        iterator& operator--() {
            node = node->lPrev;
            return *this;
        }
        
        iterator operator++(int) {
            temp = node;
            node = node->lNext;
            return temp;
        }
        
        iterator operator--(int) {
            temp = node;
            node = node->lPrev;
            return temp;
        }
        
        iterator operator+(int n) {
            temp = node;
            for (int i = 0; i < n; i++)
                temp = temp->lNext;
            return temp;
        }

        iterator operator-(int n) {
            temp = node;
            for (int i = 0; i < n; i++)
                temp = temp->lPrev;
            return temp;
        }

        iterator& operator+=(int n) {
            for (int i = 0; i < n; i++)
                node = node->lNext;
            return *this;
        }

        iterator& operator-=(int n) {
            for (int i = 0; i < n; i++)
                node = node->lPrev;
            return *this;
        }

        bool operator!=(const iterator& it) {
            return node != it.node;
        }

        bool operator==(const iterator& it) {
            return node == it.node;
        }

        lType& operator*() {
            return node->lData;
        }
    };

    // Конструктор класса по умолчанию
    clist() {
        // Обнуляем все переменные
        lHead = lTail = 0;
    }

    // Деструктор класса
    ~clist() {
        // Выбираем начало списка
        lNode *lTemp = lHead;
        // Пройти по всем узлам списка, высвобождая их из памяти
        while (lTemp) {
            // Используем lTail для запоминания адреса следующего узла
            lTail = lTemp->lNext;
            // Высвобождаем текущий узел из памяти
            delete lTemp;
            // Выбираем следующий узел в качестве текущего
            lTemp = lTail;
        }
    }

    // Вставить элемент по итератору
    void insert(iterator it, lType data) {
        // Создаём новый узел
        lNode *lTemp = new lNode(data);
        // Если конец списка не существует, значит список пуст
        if (!lTail) {
            lHead = lTail = lTemp;
            return;
        }
        // Если был указан несуществующий узел
        if (!it.node) {
            // Создаём связь с последним узлом в списке
            lTail->lNext = lTemp;
            lTemp->lPrev = lTail;
        } else {
            // Создаём связь с предыдущим элементом, если он есть
            if (it.node->lPrev) {
                it.node->lPrev->lNext = lTemp;
                lTemp->lPrev = it.node->lPrev;
            }
            // Создаём связь со следующим узлом
            it.node->lPrev = lTemp;
            lTemp->lNext = it.node;
        }     
        // Меняем начало и конец списка
        if (lHead->lPrev)       lHead = lTemp;
        else if (lTail->lNext)  lTail = lTemp;
    }

    // Возвращает итератор на первый узел списка
    iterator begin() {
        iterator it(lHead);
        return it;
    }

    // Возвращает итератор на последний узел списка
    iterator rbegin() {
        iterator it(lTail);
        return it;
    }

    // Возвращает итератор несуществующего узла списка
    iterator end() {
        iterator it;
        return it;
    }

    // Возвращает итератор несуществующего узла списка
    iterator rend() {
        iterator it;
        return it;
    }
};