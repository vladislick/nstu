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
            temp = 0;
        }

        iterator(lNode* iNode, lNode* iTemp = 0) { 
            node = iNode;
            temp = iTemp;
        }
        
        iterator& operator++() {
            if (!node) {
                node = temp;
                return *this;
            }
            temp = node;
            node = node->lNext;
            return *this;
        }
        
        iterator& operator--() {
            if (!node) {
                node = temp;
                return *this;
            }
            temp = node;
            node = node->lPrev;
            return *this;
        }
        
        iterator operator++(int) {
            if (!node) {
                node = temp;
                return 0;
            }
            temp = node;
            node = node->lNext;
            return temp;
        }
        
        iterator operator--(int) {
            if (!node) {
                node = temp;
                return 0;
            }
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

    // Вспомогательный итератор
    iterator iTemp;

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

    // Выдавить элемент по итератору
    lType pop(iterator& it) {
        lType result;
        if (!it.node) result = lTail->lData;
        else result = it.node->lData;
        // Если был указан несуществующий узел (удалить последний узел)
        if (!it.node) {
            lTail = lTail->lPrev;
            delete lTail->lNext;
            lTail->lNext = 0;
            it.node = lTail;
        } else {
            // Создаём связь предыдущего элемента
            if (it.node->lPrev) {
                it.node->lPrev->lNext = it.node->lNext;
                it.temp = it.node->lPrev;
            }
            else lHead = it.node->lNext;

            // Создаём связь следующего элемента
            if (it.node->lNext) {
                it.node->lNext->lPrev = it.node->lPrev;
                it.temp = it.node->lNext;
            }
            else lTail = it.node->lPrev;
            
            delete it.node;
            it.node = it.temp;
        }
        return result;   
    }

    void erase(iterator& it) {
        pop(it);
    }

    // Возвращает итератор на первый узел списка
    iterator& begin() {
        iTemp.node = lHead;
        return iTemp;
    }

    // Возвращает итератор на последний узел списка
    iterator& rbegin() {
        iTemp.node = lTail;
        return iTemp;
    }

    // Возвращает итератор несуществующего узла списка
    iterator& end() {
        iTemp.node = 0;
        iTemp.temp = lTail;
        return iTemp;
    }

    // Возвращает итератор несуществующего узла списка
    iterator& rend() {
        iTemp.node = 0;
        iTemp.temp = lHead;
        return iTemp;
    }
};