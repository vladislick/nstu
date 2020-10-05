#include	<stdio.h>
#include   	<windows.h>
#include   	<stdlib.h>
#include 	<string.h>

int main(int argc, char* argv[])
{
	int 				changes = 0, changesindex = 0;
	char				changesstr[20];
    DWORD 				finish, result, total = 0;
    char 				launchcmd[80];
    HANDLE 				ht[255];
    STARTUPINFO 		si[255];
    PROCESS_INFORMATION	pi[255];
    
	if (argc < 2) {
        printf("=========   Error    =========\n");
        printf("No input files\n");
        exit(-1);
    }
	
	for (int i = 0; i < (argc - 1); i++) {
		char isNumber = 1;
		for (int j = 0; j < strlen(argv[i + 1]); j++) {
			if (!(argv[i + 1][j] > 47 && argv[i + 1][j] < 58)) isNumber = 0; 
		}
		if (isNumber) {
			changes = atoi(argv[i + 1]);
			changesindex = i + 1;
			break;
		}
	}

	if (changesindex == 0) {
		printf("=========   Error    =========\n");
        printf("Number of changes not defined\n");
        exit(-3);
	}

	// Create multiple processes.
	for (int i = 0; i < (argc - 1); i++) {
		if (i + 1 == changesindex) continue;
		sprintf(changesstr, "%d", changes);
		strcpy(launchcmd, "child.exe");
		strcat(launchcmd, " ");
		strcat(launchcmd, argv[i + 1]);
		strcat(launchcmd, " ");
		strcat(launchcmd, changesstr);
		ZeroMemory(&si[i], sizeof(si[i]));
		si[i].cb = sizeof(si);
		ZeroMemory(&pi[i], sizeof(pi[i]));
		printf("EXECUTING: %s\n", launchcmd);
		if(CreateProcess(NULL, launchcmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si[i], &pi[i])) {
			printf("========= Process %lu started for file <<%s>> =========\n", pi[i].dwProcessId, argv[i+1]);
			ht[i] = pi[i].hProcess;
			Sleep(500);
		}
		else {
			printf("=========   Error    =========\n");
			printf( "CreateProcess failed with code %Lu.\n", GetLastError());
			exit(-2);
		}
	}
	
	for (int i = 0; i < (argc - 1); i++)
	{
	  finish = WaitForSingleObject(pi[i].hProcess, INFINITE);
	  if (finish == WAIT_OBJECT_0) {
	  	//printf("Process %Lu is finished\n",pi[i].dwProcessId);
	  	GetExitCodeProcess(pi[i].hProcess, &result);
	  	printf("========= Process %lu finished by exit code %d =========\n",pi[i].dwProcessId,result);
	  	total += result;
	  } else {
	  	// Close process and thread handles.
	  	CloseHandle(pi[i].hProcess);
	  	CloseHandle(pi[i].hThread);
	  	TerminateProcess(pi[i].hProcess, 0);
	  }
	}
	printf("%d File(s) processed with %d total changes.\n", argc-2, total);
	return 0;
}