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
    };
    
    // Указатель на начало списка
    Node *pRoot;

    // Функция обхода дерева, начиная с root, методом TLR
    void TLR(Node* root, void (ctree<Tdata,Tkey>::*func)(Node*)) {
        // Проверяем, существует ли дерево
        if (root == nullptr) return;
        // Для обхода дерева создаем стэк
        std::list<Node*> stack;
        // Вспомогательный указатель        
        Node *pTemp;
        // Кладем в стэк указатель на начало дерева
        stack.push_back(root);
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
            (this->*func)(pTemp);
        }
    }

    // Функция удаления узла
    void destructor_Func(Node* node){
        delete node;
    } 

public:
    // Конструктор по умолчанию
    ctree() {
        pRoot = nullptr;
    }

    // Конструктор копирования с обходом TLR
    ctree(const ctree& tree) {
        // Если копируемое дерево пустое, то выходим
        if (tree.pRoot == nullptr) return;
        // Для обхода копируемого дерева создаем стэк
        std::list<Node*> stack;
        std::list<Node**> stackCopy;
        // Вспомогательные указатели
        Node *pTemp, **pCopyTemp;
        // Кладем в стэк указатель на начало копируемого дерева
        stack.push_back(tree.pRoot);
        // Кладем в стэк указатель на начало копируемого дерева
        stackCopy.push_back(&pRoot);
        // В цикле обрабатываем стэк
        while (!stack.empty()) {
            // Достаем из стэка адрес узла
            pTemp = stack.back(); stack.pop_back();
            pCopyTemp = stackCopy.back(); stack.pop_back();
            // Создаем первый узел для дерева
            (*pCopyTemp) = new Node();
            // Копируем данные в новый узел
            (*pCopyTemp)->key = pTemp->key;
            (*pCopyTemp)->data = pTemp->data;
            (*pCopyTemp)->pLeft = (*pCopyTemp)->pRight = nullptr;
            // Создаем дочерний узел справа при наличии
            if (pTemp->pRight != nullptr) {
                // Заносим узел копируемого дерева в стэк
                stackCopy.push_back(&((*pCopyTemp)->pRight));
                // Заносим узел копируемого дерева в стэк
                stack.push_back(pTemp->pRight);
            }
            // Создаем дочерний узел слева при наличии
            if (pTemp->pLeft != nullptr) {
                // Заносим узел копируемого дерева в стэк
                stackCopy.push_back(&((*pCopyTemp)->pLeft));
                // Заносим узел копируемого дерева в стэк
                stack.push_back(pTemp->pLeft);
            }
        }
    }

    // Поиск нужного узла
    bool search(Tkey KEY, Tdata &DATA) {
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
    

    // Деструктор класса с методом обхода TLR
    ~ctree() {
        // Обходим дерево, удаляя каждый узел
        TLR(pRoot, &ctree<Tdata,Tkey>::destructor_Func);
    } 
};