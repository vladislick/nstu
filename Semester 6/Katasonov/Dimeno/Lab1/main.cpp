#include <iostream>
#include <stdint.h>
#include "cvector.h"

using namespace std;

int main(int argc, char* argv[]) {
    cvector<uint16_t> array(5);
    
    cout << "Elements:" << std::endl;

    array[0] = 4;
    array[2] = 10;
    
    for (auto myIterator = array.rbegin(); myIterator != array.rend(); myIterator--) {
        cout << array[myIterator] << endl;
    }
    
    return 0;
}