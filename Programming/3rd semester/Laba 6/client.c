#include <windows.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 256

// Many variables of semaphores and mapping file. 
// It's not a deal to describe each variable.
// If you know, so you know :)
HANDLE 	hSemSend, 
		hSemRecv, 
		hSemTermination,
		hFileMapping;
CHAR 	lpSemSendName[] = "$MySemSendName$",
		lpSemRecvName[] = "$MySemRecvName$",
		lpSemTerminationName[] = "$MySemTerminationName$",
		lpFileShareName[] = "$MyFileShareName$";

// Mapping file pointer
LPVOID lpFileMap;

// Create all connection objects
int sync() {
	// Clear errors
	SetLastError(0);
	// Open this semaphore as sending "blink"
	hSemSend = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, lpSemSendName);
	// Open this semaphore as reciving "blink"
	hSemRecv = OpenSemaphore(SYNCHRONIZE, FALSE, lpSemRecvName);
	if (hSemSend == NULL || hSemRecv == NULL)
		fprintf(stdout, "[SYSTEM ERROR #%d] Cannot open recv/send semaphore. \n", GetLastError());
	// Try to open this semaphore as signal that we are running down
	hSemTermination = OpenSemaphore(SEMAPHORE_MODIFY_STATE,	FALSE, lpSemTerminationName); //меняем значение здесь
	if (hSemTermination == NULL)
		fprintf(stdout, "[SYSTEM ERROR #%ld] Cannot open termination semaphore. \n", GetLastError());
	// Try to open mapping file
	hFileMapping = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, lpFileShareName);
	if (hFileMapping == NULL)
		fprintf(stdout, "[SYSTEM ERROR #%ld] Cannot open mapping file. \n", GetLastError());
	// Try to view mapping file 
	lpFileMap = MapViewOfFile(hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (lpFileMap == 0)
		fprintf(stdout, "[SYSTEM ERROR #%ld] Cannot view mapping file. \n", GetLastError());
    // Check if we have any errors
	if (!GetLastError()) {
		printf("All systems online.\n");
		return 1;
	} else {
		printf("Some systems did not started correctly.\n");
		return 0;
	}
}

int main()
{
    unsigned int maxChanges = 0;
	// Command (server - for server, user - from user, lower - from user but in lower case)
	CHAR 	serverCMD[BUF_SIZE],
			userCMD[BUF_SIZE],
			lowerCMD[BUF_SIZE];
	// Code of waiting server result
	DWORD 	returnCode;

	// Try to connect to the server
	printf("Hi!\nStarting all systems...\n\n");
	sync();

	// Main ui loop
	while (1) {
        BOOL onlyDigits = 1;
        // Let user type a command
        printf("\n> ");
        scanf("%s", userCMD);
		printf("\n");
        strcpy(lowerCMD, userCMD);

		// Check the user's answer
        for (int i = 0; i < strlen(userCMD); i++) {
            lowerCMD[i] = tolower(userCMD[i]);          // We need a lower version of command
            if (!isdigit(userCMD[i])) onlyDigits = 0;   // If this is all digits
        }
        
		// Check if it was exit command
        if (!strcmp(lowerCMD, "exit") || !strcmp(lowerCMD, "quit")) {
            break;
        }

		// Check if it was a restart command
		if (!strcmp(lowerCMD, "reconnect") || !strcmp(lowerCMD, "restart")) {
			printf("Restarting the connection to the server...\n");
			CloseHandle(hSemSend);
			CloseHandle(hSemRecv);
			CloseHandle(hSemTermination);
			UnmapViewOfFile(lpFileMap);
			CloseHandle(hFileMapping);
			sync();
			printf("\n");
			continue;
        }

        // Check if it was a number
        if (onlyDigits) {
            maxChanges = atoi(userCMD);
            printf("The maximum of changes is set to %d.\n", maxChanges);
            continue;
        }

        // Send data to the server
        if (maxChanges == 0) 
            sprintf(serverCMD, "%s", userCMD);
        else
            sprintf(serverCMD, "%s %d", userCMD, maxChanges);
        
		// Check if we can send any data to the server
        if (lpFileMap == 0)
        {   
            fprintf(stdout, "[SYSTEM ERROR #%ld] Cannot connect to the server. \n", GetLastError());
            return 0;
        } else 
			// If we can, then send data to the server through mapping file
			strcpy((char*)lpFileMap, serverCMD);
        
		// Turn on the send semaphore to wake up the server
		ReleaseSemaphore(hSemSend, 1, NULL);

        // Wait for the server answer
		returnCode = WaitForSingleObject(hSemRecv, INFINITE);

        // Check the answer from the server
		if (returnCode == WAIT_OBJECT_0)
            printf("Answer from the server:\n%s\n", (LPSTR)lpFileMap);
		else if (returnCode == WAIT_ABANDONED || returnCode == WAIT_FAILED)
			fprintf(stdout, "[SYSTEM ERROR #%ld] Cannot receive message from the server. \n", GetLastError());
	}
	// Massive leaving actions
	ReleaseSemaphore(hSemTermination, 1, NULL);
	CloseHandle(hSemSend);
	CloseHandle(hSemRecv);
	CloseHandle(hSemTermination);
	UnmapViewOfFile(lpFileMap);
	CloseHandle(hFileMapping);
	return 0;
}


