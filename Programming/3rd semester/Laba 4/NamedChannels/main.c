#include <windows.h>
#include <stdio.h>
#include <conio.h>

#define BUF_SIZE 512
#define CHILD_NAME "client.exe"

int main()
{
	// Startup information of child process
	STARTUPINFO 		SI;
    // Process information of child process
	PROCESS_INFORMATION	PI;
	// Flags of security of child process
	SECURITY_ATTRIBUTES PipeSA = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
	// It is true if child app is connected to the pipe
	BOOL   isConnected;
	// Pipe descriptor
	HANDLE hPipe;
	// Pipe name
	LPSTR  pipeName = "\\\\.\\pipe\\$MyPipe$";
	// Number of read/written bytes from the pipe
	DWORD  cbRead, cbWritten;
	// Message for child to say filename and maximum of changes
	char message[BUF_SIZE] = { 0 };
	// User command from terminal
	char user[BUF_SIZE] = { 0 };
	// Returned exit code from child process
	long int exitCode;
	// Maximum of possible changes in file
	int maxChanges = 99999;
	// Maximum of possible changes in file (str version)
	char maxChangesStr[10] = {0};
	
	// Welcome message
	printf("[SERVER] Hello!\n[SERVER] Type <<exit>> to terminate.\n");
	
	while (1)
	{
		printf("\n[USER] ");
		scanf("%s", user);
		// Check was user command a number
		char isNumbers = 1;
		for (int i = 0; i < strlen(user); i++) {
			if (user[i] < 48 || user[i] > 57) isNumbers = 0;
		}
		// If user command is "exit"
		if (!strcmp(user, "exit")) {
			printf("[SERVER] Server stopped.\n");
			_getch();
			break;
		} 
		// If user command is "clear"
		else if (!strcmp(user, "clear")) {
			system("cls");
		} 
		// If user command is a number
		else if (isNumbers) {
			maxChanges = atoi(user);
			sprintf(maxChangesStr, "%d", maxChanges);
			printf("[SERVER] The maximum amount of changes is set to %d.\n", maxChanges);
		} 
		// If user command is a name of file
		else {
			GetStartupInfo(&SI);
			// Redirecting the output stream for the child process to STDOUT
	    	SI.hStdOutput = hPipe;
			printf("[SERVER] Trying to create a pipe <<%s>> ......... ", pipeName);
			
			// Create the pipe
			hPipe = CreateNamedPipe( pipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, BUF_SIZE, BUF_SIZE, 5000, NULL);
			// Check is the pipe created
			if (hPipe == INVALID_HANDLE_VALUE) {
				printf("Cannot create a pipe (ERROR #%ld).\n", GetLastError());
				_getch();
				return -1;
			}
			printf("Pipe created successfully.\n");
			
			// Launch the child process
			if (CreateProcess(NULL, (LPTSTR)CHILD_NAME, NULL, NULL, TRUE, 0, NULL, NULL, &SI, &PI)) {
		    	printf("[SERVER] Process started for file <<%s>> (PID #%lu).\n", user, PI.dwProcessId);
        	} else {
            	printf("[SERVER] Process start failed (ERROR #%Lu).\n", GetLastError());
		    	CloseHandle(hPipe);
				_getch();
				return -2;
        	}
			
			// Wait for child process
			isConnected = ConnectNamedPipe(hPipe, NULL);
			// If child process cannot connect to the pipe
			if (!isConnected)
			{
				switch (GetLastError())
				{
				case ERROR_NO_DATA:
					printf("[SERVER] Cannot connect to pipe - ERROR_NO_DATA\n");
					break;
				case ERROR_PIPE_CONNECTED:
					printf("[SERVER] Cannot connect to pipe - ERROR_PIPE_CONNECTED\n");
					break;
				case ERROR_PIPE_LISTENING:
					printf("[SERVER] Cannot connect to pipe - ERROR_PIPE_LISTENING\n");
					break;
				case ERROR_CALL_NOT_IMPLEMENTED:
					printf("[SERVER] Cannot connect to pipe - ERROR_CALL_NOT_IMPLEMENTED\n");
					break;
				default:
					printf("[SERVER] Cannot connect to pipe - Error #%d\n", GetLastError());
					break;
				}
				_getch();
				CloseHandle(hPipe);
				return -2;
			}
			printf("[SERVER] Child connected to the pipe successfully.\n");

			// Send input file name and maximum of possible changes to the child by the pipe
			strcpy(message, user);
			strcat(message, " ");
			strcat(message, maxChangesStr);
			WriteFile(hPipe, message, strlen(message) + 1, &cbWritten, NULL);
			
			// Waiting for the completion of the child process
       		WaitForSingleObject(PI.hProcess, INFINITE);
			GetExitCodeProcess(PI.hProcess, &exitCode);
			CloseHandle(PI.hProcess);

			// Checking the result of the child process
			if (exitCode >= 0)
				printf("[SERVER] Child proccess done (Total changes in the file is %d).\n", exitCode);
			else 
				printf("[SERVER] Child process terminated (CHILD ERROR #%d).\n", exitCode);
			CloseHandle(hPipe);
		}
	}
	return 0;
}