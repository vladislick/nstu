#include <iostream>
#include "BST.h"

using namespace std;

int main () {
    BST<int, int> bst;

    bst.insert(5, 10);
    bst.insert(10, 4);
    bst.insert(12, 17);

    bst.insert(3, 15);
    bst.insert(92, 16);
    bst.insert(13, 22);

    bst.insert(9, 18);
    bst.insert(91, 19);
    bst.insert(2, 24);
    
   // bst.print();

    int data;
    if (bst.search(10, data)) cout << "data = " << data << endl;
   
    BST<int, int> bst2(bst);
    bst.~BST();

    cout << "Copu bst" << endl;
    bst2.print();

    bst2.erase(4);
    cout << "After delete bst" << endl;
    bst2.print();
    cout << "Length is " <<  bst2.length() << endl;
   
}
