// Memory test for Task 3
// By: Vdovin Vladislav (vladislickness@gmail.com)

#include <iostream>

using namespace std;

int main() {
    const int count = 1000;
    
    cout << "For a simple memory leak detection, this test will run the program " << count << "times." << endl;
    cout << "If after that the amount of RAM consumed increased, ";
    cout << "it means that there is a memory leak in the program." << endl;
    cout << "Now you should pay attention to how much RAM your system consumes before and after startup." << endl;
    cout << "Press [ENTER] to continue...";
    getchar();

    for (unsigned int i = 0; i < count; i++) system("Task3.exe");

    cout << "Check RAM consumption of your system now." << endl;

    return 0;
}