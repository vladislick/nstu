// Task 3
// By: Vdovin Vladislav (vladislickness@gmail.com)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct matrix {
    char**  data;
    int     rows;
    int     columns;
};

// Func read matrix and return non-zero code if errors occured
int readMatrix(FILE* file, struct matrix* m) {
    // Read matrix size, crate matrix and fill it
    if (fscanf(file, "%d", &m->rows) == EOF) return 1;
    if (fscanf(file, "%d", &m->columns) == EOF) return 2;

    // Check size of matrix A
    if (m->rows < 1 || m->columns < 1) return 3;

    // Create matrix
    m->data = (char**)malloc(m->rows * sizeof(char*));
    for (int i = 0; i < m->rows; i++) 
        m->data[i] = (char*)malloc(m->columns * sizeof(char));
    
    // And fill it by input file    
    for (int i = 0; i < m->rows; i++)
        for (int j = 0; j < m->columns; j++)
            if (fscanf(file, "%d", &m->data[i][j]) == EOF) return 4;
            else if (m->data[i][j] > 1 || m->data[i][j] < 0) return 5;
            // Change number format to ASCII
            else m->data[i][j] += 48;
    
    return 0;
}

// Func clear all allocated memory and close all opened files
void clearAll(FILE* file1, FILE* file2, struct matrix* matrix1, struct matrix* matrix2) {
    fclose(file1);
    fclose(file2);
    if (matrix1->data != NULL) {
        for (int i = 0; i < matrix1->rows; i++) free(matrix1->data[i]);
        free(matrix1->data);
    }
    if (matrix2->data != NULL) {
        for (int i = 0; i < matrix2->rows; i++) free(matrix2->data[i]);
        free(matrix2->data);
    }
}

int main() {
    FILE *inputFile = NULL, *outputFile = NULL;
    const char 
        *inputFileName  = "input.txt", 
        *outputFileName = "output.txt";
    
    struct matrix matrixA = { NULL }, matrixB = { NULL };
    int returnCode;
    
    // Try to open input file for reading
    inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL) {
        printf("ERROR: cannot open input file <<%s>>. Aborted.\n", inputFileName);
        return 1;
    }

    // Try to open output file for writing
    outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        printf("ERROR: cannot write to output file <<%s>>. Aborted.\n", outputFileName);
        clearAll(inputFile, outputFile, &matrixA, &matrixB);
        return 2;
    }

    // Read matrix A
    if ((returnCode = readMatrix(inputFile, &matrixA)) != 0) {
        printf("ERROR: cannot read matix A (code #%d). Aborted.\n", returnCode);
        clearAll(inputFile, outputFile, &matrixA, &matrixB);
        return 3;
    }

    // Read matrix B
    if ((returnCode = readMatrix(inputFile, &matrixB)) != 0) {
        printf("ERROR: cannot read matix B (code #%d). Aborted.\n", returnCode);
        clearAll(inputFile, outputFile, &matrixA, &matrixB);
        return 4;
    }
  
    // Check each index where matrix A possible to be in matrix B
    char isEqual;
    for (int i = 0; i < (matrixB.rows - matrixA.rows + 1); i++) {
        for (int j = 0; j < (matrixB.columns - matrixA.columns + 1); j++) {
            // Check if on that index matrix A is equal to the part of matrix B
            isEqual = 1;
            for (int i2 = 0; i2 < matrixA.rows; i2++) {
                for (int j2 = 0; j2 < matrixA.columns; j2++) {
                    if (matrixA.data[i2][j2] != matrixB.data[i+i2][j+j2]) isEqual = 0;
                }
            }

            // If its the same, change matrix B values
            if (isEqual) {
                for (int i2 = 0; i2 < matrixA.rows; i2++) {
                    for (int j2 = 0; j2 < matrixA.columns; j2++) {
                        if (matrixB.data[i+i2][j+j2] == '1')        matrixB.data[i+i2][j+j2] = '2';
                        else if (matrixB.data[i+i2][j+j2] == '0')   matrixB.data[i+i2][j+j2] = '*';
                    }
                }
            }
        }
    }

    // Write updated matrix B to the output file
    for (int i = 0; i < matrixB.rows; i++) {
        if (i != 0) fprintf(outputFile, "\n");
        for (int j = 0; j < matrixB.columns; j++) {
            if (j != 0) fprintf(outputFile, " ");
            fprintf(outputFile, "%c", matrixB.data[i][j]);
        }
    }

    printf("Done.\n");
    clearAll(inputFile, outputFile, &matrixA, &matrixB);
    return 0;
}