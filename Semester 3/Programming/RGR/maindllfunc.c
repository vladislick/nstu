#include <windows.h>
#include <stdio.h>

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