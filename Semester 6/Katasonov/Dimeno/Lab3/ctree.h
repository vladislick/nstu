#pragma once

#include <list>

template <typename Tdata, typename Tkey>
class ctree {
private:
    // Структура узла дерева
    struct Node {
        // Ключ для поиска нужного узла
        Tkey    key;
        // Данные, хранящиеся в узле
        Tdata   data;
        // Указатель на левый узел
        Node    *pLeft;
        // Указатель на правый узел
        Node    *pRight;
        // Конструктор узла
        Node(const Tkey &KEY, const Tdata& DATA) {
            key = KEY;
            data = DATA;
            pLeft = pRight = nullptr;
        }
    };
    
    // Указатель на начало списка
    Node *pRoot;

public:
    // Конструктор по умолчанию
    ctree() {
        pRoot = nullptr;
    }

    // Конструктор копирования с обходом TLR
    ctree(const ctree& tree) {
        // Проверяем, существует ли дерево
        if (tree.pRoot == nullptr) return;
        // Для обхода дерева создаем стэк
        std::list<Node*> stack;
        // Вспомогательный указатель        
        Node *pTemp;
        // Кладем в стэк указатель на начало дерева
        stack.push_back(tree.pRoot);
        // В цикле обрабатываем стэк
        while (!stack.empty()) {
            // Достаем из стэка адрес узла
            pTemp = stack.back(); stack.pop_back();
            // Добавляем в стек адрес правого узла при наличии
            if (pTemp->pRight != nullptr)
                stack.push_back(pTemp->pRight);
            // Добавляем в стек адрес левого узла при наличии
            if (pTemp->pLeft != nullptr)
                stack.push_back(pTemp->pLeft);
            // Обработка узла
            insert(pTemp->key, pTemp->data);
        }
    }

    // Деструктор класса с методом обхода TLR
    ~ctree() {
        // Проверяем, существует ли дерево
        if (pRoot == nullptr) return;
        // Для обхода дерева создаем стэк
        std::list<Node*> stack;
        // Вспомогательный указатель        
        Node *pTemp;
        // Кладем в стэк указатель на начало дерева
        stack.push_back(pRoot);
        // В цикле обрабатываем стэк
        while (!stack.empty()) {
            // Достаем из стэка адрес узла
            pTemp = stack.back(); stack.pop_back();
            // Добавляем в стек адрес правого узла при наличии
            if (pTemp->pRight != nullptr)
                stack.push_back(pTemp->pRight);
            // Добавляем в стек адрес левого узла при наличии
            if (pTemp->pLeft != nullptr)
                stack.push_back(pTemp->pLeft);
            // Обработка узла
            delete pTemp;
        }
        // Очищаем указатель на начало списка
        pRoot = nullptr;
    } 

    // Втавить новый узел в дерево
    void insert(const Tkey& KEY, const Tdata &DATA) {
        // Создаем временный указатель на узел и кладем адрес начала дерева
        Node *temp = pRoot;
        // Если дерево пустое
        if (temp == nullptr) {
            pRoot = new Node(KEY, DATA);
            return;
        }
        // В цикле проходим по дереву
        while(true) {
            if (temp->key > KEY) {
                if (temp->pLeft != nullptr) temp = temp->pLeft;
                else {
                    temp->pLeft = new Node(KEY, DATA);
                    break;
                }
            } else if (temp->key < KEY) {
                if (temp->pRight != nullptr) temp = temp->pRight;
                else {
                    temp->pRight = new Node(KEY, DATA);
                    break;
                }
            } else {
                temp->data = DATA;
                temp->key = KEY;
                break;
            }
        }
    }

    // Поиск нужного узла
    bool search(const Tkey& KEY, Tdata &DATA) {
        Node *temp = pRoot;
        while(temp != nullptr) {
            if (temp->key > KEY) temp = temp->pLeft;
            else if (temp->key < KEY) temp = temp->pRight;
            else {
                DATA = temp->data;
                return true;
            }
        }
        return false;
    }

    // Поиск нужного узла
    bool searchByData(Tkey& KEY, const Tdata &DATA) {
        // Проверяем, существует ли дерево
        if (pRoot == nullptr) return false;
        // Для обхода дерева создаем стэк
        std::list<Node*> stack;
        // Вспомогательный указатель        
        Node *pTemp;
        // Кладем в стэк указатель на начало дерева
        stack.push_back(pRoot);
        // В цикле обрабатываем стэк
        while (!stack.empty()) {
            // Достаем из стэка адрес узла
            pTemp = stack.back(); stack.pop_back();
            // Добавляем в стек адрес правого узла при наличии
            if (pTemp->pRight != nullptr)
                stack.push_back(pTemp->pRight);
            // Добавляем в стек адрес левого узла при наличии
            if (pTemp->pLeft != nullptr)
                stack.push_back(pTemp->pLeft);
            if (pTemp->data == DATA) {
                KEY = pTemp->key;
                return true;
            }
        }
        return false;
    }

    bool erase(const Tkey& KEY) {
        // Указатель на текущий узел
        Node *temp = pRoot;
        // Адрес переменной, которая хранит адрес на текущий узел
        Node **prev = &pRoot;
        while(true) {
            // Если выпали на несуществующий узел
            if (temp == nullptr) return false;
            // Узнаем адрес с указателем
            else if (temp->key > KEY) {
                prev = &(temp->pLeft);
                temp = temp->pLeft;
            }
            else if (temp->key < KEY) {
                prev = &(temp->pRight);
                temp = temp->pRight;
            }
            // Если находимся на нужном узле
            else {
                // Если у удаляемого узла нет потомков
                if (temp->pLeft == nullptr && temp->pRight == nullptr) {
                    *prev = nullptr;
                    delete temp;
                } 
                // Если у удаляемого узла два потомка
                else if (temp->pLeft != nullptr && temp->pRight != nullptr) {
                    // Поиск замещающего узла
                    Node *t = temp, **tprev;
                    tprev = &(t->pRight);
                    t = t->pRight;
                    while(t->pLeft != nullptr) {
                        tprev = &(t->pLeft);
                        t = t->pLeft;
                    }
                    // Копирование данных из замещающего узла
                    temp->key = t->key;
                    temp->data = t->data;
                    // Восстановление связи
                    *tprev = t->pRight;
                    // Удаление замещающего узла
                    delete t;
                } 
                // Если у удаляемого узла только левый потомок
                else if (temp->pLeft != nullptr) {
                    *prev = temp->pLeft;
                    delete temp;
                } 
                // Если у удаляемого узла только правый потомок
                else if (temp->pRight != nullptr) {
                    *prev = temp->pRight;
                    delete temp;
                }
                return true;
            }
        }
    }
    
    // Вывести все узлы на заданной глубине дерева
    void atLevel(unsigned int level, std::list<Tkey> &nodes) {
        // Проверяем, существует ли дерево
        if (pRoot == nullptr) return;
        // Очищаем список узлов
        nodes.clear();
        // Для обхода дерева создаем стэк
        std::list<Node*> stack;
        // Глубина узла
        unsigned int tLevel;
        // Искомый ключ
        Tkey neededKey;
        // Вспомогательный указатель        
        Node *pTemp;
        // Кладем в стэк указатель на начало дерева
        stack.push_back(pRoot);
        // В цикле обрабатываем стэк
        while (!stack.empty()) {
            // Достаем из стэка адрес узла
            pTemp = stack.back(); stack.pop_back();
            // Добавляем в стек адрес правого узла при наличии
            if (pTemp->pRight != nullptr)
                stack.push_back(pTemp->pRight);
            // Добавляем в стек адрес левого узла при наличии
            if (pTemp->pLeft != nullptr)
                stack.push_back(pTemp->pLeft);
            // Вычисляем уровень найденного узла
            tLevel = 0;
            // Запоминаем ключ найденного узла
            neededKey = pTemp->key;
            // Начинаем с начала дерева (0-ой уровень)
            pTemp = pRoot;
            while(neededKey != pTemp->key) {
                if (neededKey > pTemp->key) pTemp = pTemp->pRight;
                else pTemp = pTemp->pLeft;
                tLevel++;
            }
            // Если глубина соответствует, то добавляем в список
            if (tLevel == level) nodes.push_back(neededKey);
        }
    }

};