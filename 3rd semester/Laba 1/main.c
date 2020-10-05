#include <windows.h> 
#include <stdio.h> 
#define BUF_SIZE 256
 
int bufferProcessing(CHAR* buf, int maxChanges) {
    int changes = 0;
    // Replace numbers on spaces
    for (int i = 0; buf[i] != '\0'; i++) {
        if (changes >= maxChanges) break;
        if (buf[i] > 47 && buf[i] < 58) {
            buf[i] = ' ';
            changes++;
        }
    }
    return changes;
}

int main(int argc, LPTSTR argv[]) {
    HANDLE hIn, hOut; 
    DWORD nIn, nOut; 
    CHAR Buffer[BUF_SIZE];
    CHAR FileOut[BUF_SIZE];
    int Changes = 0, MAX_CHANGES;
    
    printf("\n");
    if(argc != 3) {
        printf("=========   Error    =========\n");
        printf("Please, use: *PROGRAM_NAME* input_file max_changes\n\n"); 
        return -1;
    } 
    
    // Try to open input file (READ MODE)
    hIn = CreateFile(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); 
    if (hIn == INVALID_HANDLE_VALUE) {
        printf("=========   Error    =========\n");
        printf("Cannot open input file. Error: %x\n\n", GetLastError()); 
        return -1;
    } 
    
    // Get maximum count of changes
    MAX_CHANGES = atoi(argv[2]);
 
    // Get the name of output file 
    if (strchr(argv[1], '.') == NULL) // If input file hasn't file extension
        strcpy(FileOut, argv[1]);
    else 
        strncpy(FileOut, argv[1], strchr(argv[1], '.') - argv[1]);
    strcat(FileOut, ".out");
 
    // Try to open output file (WRITE MODE)
    hOut = CreateFile (FileOut, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
    if(hOut==INVALID_HANDLE_VALUE) {
        printf("=========   Error    =========\n");
        printf("Cannot open output file. Error: %x\n\n", GetLastError());
        return -1; 
    }
 
    // File reading-writing 
    while(ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) { 
        // Sometime there is an error with fantom characters
        Buffer[nIn] = '\0';
        printf("========= Recognized ========= \n%s\n\n", Buffer);
        Changes = bufferProcessing(Buffer, MAX_CHANGES);
        printf("========= Processed  ========= \n%s\n\n", Buffer);
        WriteFile (hOut, Buffer, nIn, &nOut, NULL); 
        if (nIn != nOut) {
            printf("=========   Error    =========\n");
            printf("Writing error: %x\n\n", GetLastError());
            return -1;
        }
    }
 
    printf("============ Done ============ \n\n");
    CloseHandle (hIn); 
    CloseHandle (hOut); 
    return Changes;
}
