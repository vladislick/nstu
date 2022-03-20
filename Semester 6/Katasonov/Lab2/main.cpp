#include <iostream>
#include <stdint.h>
#include "clist.h"

using namespace std;

int main() {
    clist<int> list;
    
    list.insert(list.end(), 2);
    list.insert(list.end(), 3);
    list.insert(list.end(), 4);
    list.insert(list.begin(), 1);

    cout << "List is:" << endl;
    for (auto it = list.begin(); it != list.end(); it++)
        cout << "-> " << *it << endl;

    cout << "Pop is " << list.pop(list.end()) << endl;

    cout << "List is:" << endl;
    for (auto it = list.begin(); it != list.end(); it++)
        cout << "-> " << *it << endl;


    return 0;
}