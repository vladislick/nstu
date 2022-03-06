// Task 3
// By: Vdovin Vladislav (vladislickness@gmail.com)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *inputFile = NULL, *outputFile = NULL;
const char 
    *inputFileName  = "input.txt", 
    *outputFileName = "output.txt";

int check(const char* input, const char* output) {
    char ch;
    char isEqual;
    int index;
    // Try to open input file for writing
    inputFile = fopen(inputFileName, "w");
    if (inputFile == NULL) {
        printf("ERROR: cannot open input file <<%s>>. Aborted.\n", inputFileName);
        return -1;
    }

    fprintf(inputFile, input);
    fclose(inputFile);

    system("Task3.exe");
    
    // Try to open output file for reading
    outputFile = fopen(outputFileName, "r");
    if (outputFile == NULL) {
        printf("ERROR: cannot open output file <<%s>>. Aborted.\n", outputFileName);
        return -2;
    }

    isEqual = 1;
    index = 0;
    while(fscanf(outputFile, "%c", &ch) != EOF) {
        if (ch != output[index]) isEqual = 0;
        index++;
    }
    
    fclose(outputFile);
    return isEqual;
}

int main() {
    printf("Test 1 is %d\n", check("2 2\n1 0\n1 1\n5 5\n1 1 0 0 0\n0 1 1 0 0\n1 0 0 1 0\n1 1 1 1 0\n0 0 1 1 1", "1 2 * 0 0\n0 2 2 0 0\n2 * 0 1 0\n2 2 1 2 *\n0 0 1 2 2"));
    printf("Test 2 is %d\n", check("1 1\n1\n5 5\n1 1 0 0 0\n0 1 1 0 0\n1 0 0 1 0\n1 1 1 1 0\n0 0 1 1 1", "2 2 0 0 0\n0 2 2 0 0\n2 0 0 2 0\n2 2 2 2 0\n0 0 2 2 2"));
    printf("Test 3 is %d\n", check("1 1\n0\n5 5\n1 1 0 0 0\n0 1 1 0 0\n1 0 0 1 0\n1 1 1 1 0\n0 0 1 1 1", "1 1 * * *\n* 1 1 * *\n1 * * 1 *\n1 1 1 1 *\n* * 1 1 1"));
    printf("Test 4 is %d\n", check("2 6\n1 0 0 1 0 1\n1 1 1 0 1 0\n5 5\n1 1 0 0 0\n0 1 1 0 0\n1 0 0 1 0\n1 1 1 1 0\n0 0 1 1 1", "1 1 0 0 0\n0 1 1 0 0\n1 0 0 1 0\n1 1 1 1 0\n0 0 1 1 1"));
    return 0;
}