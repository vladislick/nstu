#include <endian.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int pid[argc - 1], status, stat;
	
	// для всех файлов, перечисленных в командной строке 
	if (argc < 3) {
	    printf("Usage: %s textfile1 textfile2 ... character\n", argv[0]);
	    exit(-1);
    }
	
	// Проверяем, что последний аргумент - это символ
	if (strlen(argv[argc - 1]) != 1) {
	    printf("ERROR: There is no character entered!\n");
	    exit(-1);
	}

	// Запускаем цикл по всем файлам
	for (int i = 1; i < (argc - 1); i++) { 
		// Запускаем дочерний процесс 
		pid[i] = fork(); 
		// Если выполняется дочерний процесс 
		if (pid[i] == 0) {
			if (execl("./main.bin", "./main.bin", argv[i], argv[argc - 1], NULL) < 0) {
				printf("ERROR: Cannot start processing file %s\n", argv[i]);
				exit(-1);
			}
			else printf( "Processing of file %s started (pid=%d)\n", argv[i], pid[i]);
		}
	} // Если выполняется родительский процесс
	sleep(1);
	// Ожидание окончания выполнения всех запущенных процессов
	for (int i = 1; i < (argc - 1); i++) { 
		status = waitpid(pid[i], &stat, 0);
		if (pid[i] == status) {
			printf("File %s done, result is %d\n", argv[i], (signed char)WEXITSTATUS(stat));
		}
	}
	return 0; 
}
