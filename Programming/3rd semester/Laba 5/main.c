#include <windows.h>
#include <stdio.h>
#include <conio.h>

#define BUF_SIZE 512
#define CHILD_NAME "client.exe"

int main()
{
	// Идентификаторы каналов Mailslot
	HANDLE hMailslot1, hMailslot2;
    // Размер сообщения в байтах
	DWORD  cbMessages;
		// Количество сообщений в канале Mailslot2
	DWORD  cbMsgNumber;
    // Startup information of child process
	STARTUPINFO 		SI;
    // Process information of child process
	PROCESS_INFORMATION	PI;
	// Flags of security of child process
	SECURITY_ATTRIBUTES PipeSA = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
	// It is true if child app is connected to the pipe
	BOOL   isConnected;
    // Код возврата из функций
	BOOL   fReturnCode;
	// Pipe descriptor
	HANDLE hPipe;
	// Mailshot name
	LPSTR  mailslotName1 = "\\\\.\\mailslot\\$Channel1$";
	// Mailshot name
	LPSTR  mailslotName2 = "\\\\.\\mailslot\\$Channel2$";
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
	    	        //SI.hStdOutput = stdout;
			
            // Try to create mailslot 2
			printf("[SERVER] Trying to create a mailslot <<%s>> ......... ", mailslotName2);
            hMailslot2 = CreateMailslot(mailslotName2, 0, MAILSLOT_WAIT_FOREVER, NULL);
			if (hMailslot2 == INVALID_HANDLE_VALUE)
            {
                fprintf(stdout, "\n[SERVER] Create mailslot 2 failed (ERROR #%ld)\n", GetLastError());
                CloseHandle(hMailslot2);
                _getch();
                return -1;
            }
			printf("Mailslot created successfully.\n");

            // Launch the child process
			if (CreateProcess(NULL, (LPTSTR)CHILD_NAME, NULL, NULL, TRUE, 0, NULL, NULL, &SI, &PI)) {
		    	printf("[SERVER] Process started for file <<%s>> (PID #%lu).\n", user, PI.dwProcessId);
        	} else {
            	printf("[SERVER] Process start failed (ERROR #%Lu).\n", GetLastError());
				_getch();
				return -2;
        	}

            // Try to create mailslot 1
            printf("[SERVER] Trying to connect to the client's mailslot <<%s>> ......... ", mailslotName1);
			Sleep(200);
			hMailslot1 = CreateFile(mailslotName1, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (hMailslot1 == INVALID_HANDLE_VALUE)
            {
				 fprintf(stdout, "\n[SERVER] Create mailslot 1 failed (ERROR #%ld)\n", GetLastError());
				 return -1;
            }
            printf("Mailslot connected successfully.\n");
			
            ZeroMemory(message, BUF_SIZE);
            strcpy(message, user);
			strcat(message, " ");
			strcat(message, maxChangesStr);

            // Send filename to child process
            if (!WriteFile(hMailslot1, message, strlen(message) + 1, &cbWritten, NULL)) break;
            printf("[SERVER] Sent data to the child: <<%s>> (CHILD PID #%lu).\n", message, PI.dwProcessId);
            
			cbMsgNumber = 0;
            while (!cbMsgNumber) {
				fReturnCode = GetMailslotInfo(hMailslot2, NULL, &cbMessages, &cbMsgNumber, NULL);
		    	if (!fReturnCode)
		    	{
			    	fprintf(stdout, "[SERVER] GetMailslotInfo for reply (ERROR #%ld)\n", GetLastError());
			    	_getch();
			    	break;
		    	}
                ZeroMemory(message, BUF_SIZE);
			    if (cbMsgNumber > 0 && ReadFile(hMailslot2, message, BUF_SIZE, &cbRead, NULL)) {
				    // Выводим принятую строку на консоль 
				    printf("[SERVER] ==================== Received message ====================\n%s", message);
                    printf("[SERVER] =================== End of the message ===================\n");
			    }
		    }

			// Waiting for the completion of the child process
       		WaitForSingleObject(PI.hProcess, INFINITE);
			GetExitCodeProcess(PI.hProcess, &exitCode);
			CloseHandle(PI.hProcess);

			// Checking the result of the child process
			if (exitCode >= 0)
				printf("[SERVER] Child proccess done (Total changes in the file is %d).\n", exitCode);
			else 
				printf("[SERVER] Child process terminated (CHILD ERROR #%d).\n", exitCode);

            CloseHandle(hMailslot1);
            CloseHandle(hMailslot2);
        }
    }
    return 0;
}