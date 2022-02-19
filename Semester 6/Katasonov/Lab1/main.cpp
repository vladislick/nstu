#include <iostream>
#include <stdint.h>
#include "cvector.h"

int main(int argc, char* argv[]) {
    cvector<uint16_t> array;

    uint16_t    a = 10, 
                b = 8, 
                c = 13, 
                d = 17;
    
    array.push(a);
    array.push(b);
    array.push(c);
    array.push(d);
    
    std::cout << "Elements before something" << std::endl;
    for (uint32_t i = 0; i < array.size(); i++)
        std::cout << "Elem[" << i << "] " << array[i] << std::endl;
    
    array.set(6, d);
    array.remove(1);
    
    std::cout << "Elements after something:" << std::endl;
    for (uint32_t i = 0; i < array.size(); i++)
        std::cout << "Elem[" << i << "] " << array[i] << std::endl;
    
    return 0;
}