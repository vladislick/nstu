#include <iostream>
#include "ctree.h"

using namespace std;

int main() {
    ctree<int, int> tree, tree2(tree);
    return 0;
}