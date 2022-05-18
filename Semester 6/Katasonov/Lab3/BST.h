#pragma once

#include <iostream>

using namespace std;

template <typename bType, typename bKey>
class BST {
	private:
		struct bNode
		{
			bType Data;
			bKey key;
			bNode* pleft;
			bNode* pright;
			bNode(const bKey &Key, const bType &data){
				Data = data;
				key = Key;
				pleft = NULL;
				pright = NULL;
			}
		};
		
		bNode* ROOT;

		//Вставка по ключу. 
		//t-текущий узел, key-указанный ключ узла, data-данные, in-флаг.
		bool BST_IN(bNode** t, const bType &data, const bKey &key) {
			//Дерево пустое или узла не существует.
			if (*t == NULL) {
				*t = new bNode(key, data);
				return 1;
			}
			//Ключ совпал
			if ((*t)->key == key) {
				return 0;
			}
			//Спускаемся по дереву влево или вправо
			if (key < (*t)->key) {
				return BST_IN(&((*t)->pleft), data, key);
			} 
			else {
				return BST_IN(&((*t)->pright), data, key);
			}
		}

		//Обход дерева
		void BST_LTR(bNode* t, int k = 1) {
			if (t == NULL) {
				return;
			} 
			BST_LTR(t->pleft, k + 1);
			for (int i = 0; i < k; i++) cout << "|";
			cout << " " << t->key << endl;            
			BST_LTR(t->pright, k + 1);
		}

		//Обход дерева
		void BST_DELETE_LTR(bNode* t) {
			if (t == NULL) {
				return;
			} 
			BST_DELETE_LTR(t->pleft);
			//cout << "Node with key " << t->key << " deleted" << endl;         
			BST_DELETE_LTR(t->pright);
			delete t;   
		}
	
		//Поиск по ключу
		bool BST_SEARCH(bNode* t, const bKey &key, bType &data) {
			//Узла не существует
			if (t == NULL) {
				return 0;
			}
			//Ключи совпали-узел найден
			if (key == t->key) {
				data = t->Data;
				return true;
			}
			//Если ключ меньше идем влево иначе идем вправо
			if (key < t->key) {
				return BST_SEARCH(t->pleft, key, data);
			} else {
				return BST_SEARCH(t->pright, key, data);
			}
		}
		//конструктор копирования 
		void BST_COPY(bNode** t, bNode* original) {
			if (original == NULL) {
				return;
			}
			BST_IN(t, original->Data, original->key);
			BST_COPY(t, original->pleft);
			BST_COPY(t, original->pright);
		}		
	
	public:

		BST() {
			ROOT = NULL;
		}
		
		BST(const BST& tree) {
			ROOT = NULL;
			BST_COPY(&ROOT, tree.ROOT);
		}

		~BST() {
			BST_DELETE_LTR(ROOT);
			ROOT = NULL;
		}

		bool insert(const bType &data, const bKey &key) {
			return BST_IN(&ROOT, data, key);
		}

		void print() {
			BST_LTR(ROOT);
		}

		//Поиск по ключу - обертка
		bool search (const bKey &key, bType &data) {
			return BST_SEARCH(ROOT, key, data);
		}


	
	/*//Поиск замещающего узла
	bNode* Del(bNode* pt, bNode* p0) {
		if (pt != NULL) {
			pt -> pleft = Del(pt->pleft, p0);
			return pt;
		}
		p0->key = pt->key;
		p0->data = pt->data;
		temp = pt->pright;
		delete pt;
		return temp;
	} 
	//Удаление узла по ключу key-искомый, pt-текущий, del-флаг
	bNode* BST_DEl( bKey key, bNode* pt, bool &del) {
		//Узла не существует
		if (pt == NULL) {
			del = false;
			return NULL;
		} 
		//Искомый меньше текущего идем влево
		if (pt->key > key) {
			pt->pleft = BST_DEl(key, pt->pleft, &delet);
			del = delet;
			return pt;
		}
		//Искомый больше текущего идем вправо
		if ((pt->key < key) {
			pt->pright = BST_DEl(key, pt->pright, &delet);
			del = delet;
			return pt;
		}
		//У узла нет потомков-удаляем
		if (pt->pleft ==  NULL && pt->pright == NULL) {
			delete pt;
			del = true;
			return NULL;
		} 
		//У узла только левый потомок, запоминаем связь с левым потомком, удаляем узел.
		if (pt->pright ==  NULL) {
			temp = pt->pleft;
			delete pt;
			del = true;
			return NULL;
		}
		//У узла только правый потомок, запоминаем связь с правым потомком, удаляем узел.
		if (pt->pleft ==  NULL) {
			temp = pt->pright;
			delete pt;
			del = true;
			return NULL;
		}
		//У узла оба потомка, ищем замещающий узел
		pt->pright = Del(pt->pright, pt);
		del = true;
		return pt;
	}*/
	
	
};
