#include <iostream>
#include "ctree.h"

using namespace std;

int main() {
    ctree<int, int> tree;
    int data;
    tree.insert(10, 1);
    tree.insert(4, 2);
    tree.insert(15, 3);
    tree.insert(12, 4);
    tree.insert(11, 5);
    tree.insert(14, 6);
    tree.insert(17, 7);
    tree.insert(16, 8);
    tree.insert(20, 9);
    tree.insert(21, 10);
    ctree<int, int> tree2(tree);
    tree.~ctree();

    printf("Before: \n");
    list<int> listing; 
    int i = 0;
    while (true) {
        tree2.atLevel(i, listing);
        if (listing.size() == 0) break;
        printf("L%d: ", i);
        for (auto it = listing.begin(); it != listing.end(); it++) {
            if (it != listing.begin()) printf(", ");
            printf("%d", *it);
        }
        printf(".\n");
        i++;
    }

    tree2.erase(10);

    printf("After: \n");
    i = 0;
    while (true) {
        tree2.atLevel(i, listing);
        if (listing.size() == 0) break;
        printf("L%d: ", i);
        for (auto it = listing.begin(); it != listing.end(); it++) {
            if (it != listing.begin()) printf(", ");
            printf("%d", *it);
        }
        printf(".\n");
        i++;
    }

    int key;
    if (tree2.searchByData(key, 7)) printf("Found node with key %d\n", key);

    
    return 0;
}