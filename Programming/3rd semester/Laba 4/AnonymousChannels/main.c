#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <io.h>

#define STR_SIZE 256
#define CHILD_NAME "child.exe"

int main(int argc, LPTSTR argv[])
{
	DWORD cbWritten, cbRead; // Count of written/read bytes
	HANDLE  hReadPipe1, 
            hWritePipe1, 
            hReadPipe2, 
            hWritePipe2;
    int exitCode, changes = 0, changesindex;
    STARTUPINFO 		sic[255];
    PROCESS_INFORMATION	pic[255];
	SECURITY_ATTRIBUTES PipeSA = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
	char    result[STR_SIZE] = {0},
            command[STR_SIZE] = {0};

    if (argc < 2) {
        printf("[SERVER] No input files");
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

    for (int i = 0; i < (argc - 1); i++) {
        if (changesindex == (i + 1)) continue;
        GetStartupInfo(&sic[i]);
	    /* Создание каналов, 1й - туда, 2й - обратно */
	    CreatePipe(&hReadPipe1, &hWritePipe1, &PipeSA, 0);
	    CreatePipe(&hReadPipe2, &hWritePipe2, &PipeSA, 0);

	    /* Перенаправить стандартный ввод/вывод и создать дочерний процесс. */
	    sic[i].hStdInput = hReadPipe1;
	    sic[i].hStdError = GetStdHandle(STD_ERROR_HANDLE);
	    sic[i].hStdOutput = hWritePipe2;
	    sic[i].dwFlags = STARTF_USESTDHANDLES;
        strcpy(command, argv[i + 1]);
        if (changes != 0) {
            strcat(command, " ");
            char temp[STR_SIZE];
            sprintf(temp, "%d", changes);
            strcat(command, temp);
        }
        if (CreateProcess(NULL, (LPTSTR)CHILD_NAME, NULL, NULL, TRUE, 0, NULL, NULL, &sic[i], &pic[i])) {
		    printf("\n[SERVER] Process started wit PID %lu for file <<%s>>\n", pic[i].dwProcessId, argv[i + 1]);
        } else {
            printf("[SERVER] Process start failed with error %Lu for file <<%s>>\n", GetLastError(), argv[i + 1]);
		    exit(-2);
        }

	    /* Отправить имя файла в канал. */
	    WriteFile(hWritePipe1, command, strlen(command) + 1, &cbWritten, NULL);
	    printf("[SERVER] Message <<%s>> written successfully which size is %d bytes\n", command, (int)cbWritten);
        /* Закрыть дескриптор записи канала, поскольку он больше не нужен, чтобы вторая команда могла обнаружить конец файла. */
        CloseHandle(hWritePipe1);
        /* Ожидать результат дочернего процесса. */
        WaitForSingleObject(pic[i].hProcess, INFINITE);
        GetExitCodeProcess(pic[i].hProcess, &exitCode);
        if (exitCode >= 0) {
            ReadFile(hReadPipe2, result, 80, &cbRead, NULL);
            printf("[SERVER] Readed message <<%s>> which size is %d bytes\n", result, (int)cbRead);
            /* Закрыть дескриптор чтения из канала, поскольку он больше не нужен, чтобы вторая команда могла обнаружить конец файла. */
        }
        CloseHandle(hReadPipe2);
        /* вывод числа пробелов в переданном файле или сообщение об ошибке. */
        if (exitCode >= 0)
            printf("[SERVER] File <<%s>> processed by client with PID %u has %d changes\n", argv[i + 1], pic[i].dwProcessId, atoi(result));
        else
            printf("[SERVER] File <<%s>> can't processed by client with PID %u\n", argv[i + 1], pic[i].dwProcessId, result);
        /* Ожидать завершения дочернего процесса. */
        CloseHandle(pic[i].hProcess);
    }
    printf("\n");
	return 0;
}