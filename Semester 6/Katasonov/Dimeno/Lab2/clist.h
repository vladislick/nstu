#pragma once

template <typename myType>
class clist {
private:
    // Структура узла связанного списка
    struct Node
    {
        // Указатель на следующий узел списка
        Node *next;
        // Данные, которые хранит узел
        myType data;
        // Конструктор структуры node
        Node(myType &value) {
            data = value;
            next = nullptr;
        }
    };
    // Указатель на начало (голову) списка
    Node *Head;
public:
    // Класс итератора
    class iterator {
        friend class clist;
    private:
        Node *node, *prev;

        iterator(Node* pNode, Node* pPrev = nullptr) { 
            node = pNode;
            prev = pPrev;
        }
    public:
        iterator() {
            node = prev = nullptr;
        }
        
        const iterator& operator++(int) {
            if (node != nullptr) {
                // Запоминание текущего узла
                prev = node;
                // Переход к следующему узлу
                node = node->next;
            }
            return *this;
        }
        
        iterator operator+(int n) {
            // Создаём копию итератора
            iterator it = *this;
            // Проходим в цикле по узлам
            for (int i = 0; i < n; i++) {
                // Запоминание текущего узла
                it.prev = it.node;
                // Переход к следующему узлу
                it.node = it.node->next;
            }
            return it;
        }

        const iterator& operator+=(int n) {
            for (int i = 0; i < n; i++) {
                // Запоминание текущего узла
                prev = node;
                // Переход к следующему узлу
                node = node->next;
            }
            return *this;
        }

        bool operator!=(const iterator& it) {
            return node != it.node;
        }

        bool operator==(const iterator& it) {
            return node == it.node;
        }

        const myType& operator*() {
            return node->data;
        }
    };

    // Конструктор класса по умолчанию
    clist() {
        // Обнуляем все переменные
        Head = nullptr;
    }

    // Деструктор класса
    ~clist() {
        // Выбираем начало списка
        Node *temp = Head;
        // Для запоминания адреса следующего узла
        Node *temp2;
        // Пройти по всем узлам списка, высвобождая их из памяти
        while (temp) {
            // Используем temp2 для запоминания адреса следующего узла
            temp2 = temp->next;
            // Высвобождаем текущий узел из памяти
            delete temp;
            // Выбираем следующий узел в качестве текущего
            temp = temp2;
        }
    }

    // Вставить элемент по итератору
    void insert(const iterator& it, myType value) {
        // Создаём новый узел с помощью конструктора структуры Node
        Node *temp = new Node(value);
        // Если список пуст
        if (Head == nullptr) {
            Head = temp;
            return;
        }
        // Если вставка в конец
        if (it.node == nullptr) {
            // Создаем итератор и ищем конец списка
            iterator i(Head);
            for (i; i.node->next != nullptr; i++);
            // Привязываем новый узел к концу списка
            i.node->next = temp;
            return;
        }
        // Если есть предыдущий узел
        if (it.prev != nullptr) it.prev->next = temp;
        // Связываем новый узел с узлом по итератору
        temp->next = it.node;
        // Если вставка была в начало списка
        if (it.node == Head) Head = temp;
    }

    // Удалить элемент по итератору
    void erase(const iterator& it) {
        // Если удаление происходит из начала списка
        if (it.node == Head) Head = Head->next;
        // Если удаление происходит с конца списка
        else if (it.node == nullptr) {
            // Создаем итератор и ищем конец списка
            iterator i(Head);
            for (i; i.node->next != nullptr; i++);
            // Для предпоследнего узла в списке удаляется связь
            i.prev->next = nullptr;
            // Удаление последнего узла в списке
            delete i.node;
            return;
        }
        // Связываем предыдущий узел со следующим
        else it.prev->next = it.node->next;
        // Удаляем высвободившийся узел
        delete it.node;
    }

    // Получить значение узла по итератору
    myType read(const iterator& it) {
        // Если итератор указывает на существующий узел
        if (it.node != nullptr) return it.node->data;
        // Создаем итератор и ищем конец списка
        iterator i(Head);
        for (i; i.node->next != nullptr; i++);
        // Возвращаем значение узла на конце списка
        return i.node->data;
    }
    
    // Возвращает итератор на первый узел списка
    iterator begin() {
        iterator temp(Head);
        return temp;
    }

    // Возвращает итератор несуществующего узла списка
    iterator end() {
        iterator temp(nullptr);
        return temp;
    }
};