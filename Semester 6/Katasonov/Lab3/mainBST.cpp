#include <iostream>
#include "BST.h"

using namespace std;

int main () {
    BST<int, int> bst;

    bst.insert(5, 17);
    bst.insert(10, 4);
    bst.insert(12, 10);
    bst.insert(9, 18);
    
    bst.print();

    int data;
    if (bst.search(10, data)) cout << "data = " << data << endl;
   

    BST<int, int> bst2(bst);
    bst.~BST();

    cout << "Copu bst" << endl;
    bst2.print();

	/*int n;                              //Количество элементов
    int s;                              //Число, передаваемое в дерево
	int data = 0, in = 0;	
    cout << "Elements ";
    cin >> n;                           //Вводим количество элементов
 
    for (int i = 0; i < n; i++)
    {
        cout << "Num ";
        cin >> s;                       //Считываем элемент за элементом
		bst.BST_IN(&tree, data, s, in);
	}
    cout << "Your tree\n";
    bst.LTR(tree, 0);
    cin.ignore().get(); */

}
