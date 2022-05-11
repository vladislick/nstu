#include <iostream>
#include <stdint.h>
#include "cvector.h"

using namespace std;

int main(int argc, char* argv[]) {
    cvector<uint16_t> array(5);
    
    array[0] = 4;
    array[2] = 10;
    cout << "Elements before:" << std::endl;
    for (auto myIterator = array.begin(); myIterator != array.end(); myIterator++) {
        cout << "Elem[" << myIterator.getIndex() << "] = " << array[myIterator] << ";" << endl;
    }

    array.erase(1);
    array.insert(array.begin() + 2, 15);

    cout << "Elements after:" << std::endl;
    for (auto myIterator = array.begin(); myIterator != array.end(); myIterator++) {
        cout << "Elem[" << myIterator.getIndex() << "] = " << array[myIterator] << ";" << endl;
    }
    
    return 0;
}