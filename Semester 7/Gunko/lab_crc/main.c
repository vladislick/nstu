#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

int main(int argc, char *argv[]) {
	// Массив идентификаторов процессов
	pid_t pid[argc];

	// Результат выполнения дочернего процесса
	int result;

	// Массив каналов для возврата результата дочерними процессами
	int pipes[argc][2];

	// Результат выполнения дочернего процесса
	int status, stat;
	
	// Проверка количества аргументов
	if (argc < 2) {
	    fprintf(stderr, "Usage: %s file1 ... fileN\n", argv[0]);
	    exit(-1);
    }

	// Запускаем цикл по всем файлам
	for (int i = 1; i < argc; i++) { 
		// Создание канала связи
		pipe(pipes[i]);
		// Запускаем дочерний процесс 
		pid[i] = fork(); 
		// Код выполняется дочерним процессом 
		if (pid[i] == 0) {
			// Дескриптор файла
			int fd;
			// Результат обработки файла
			int result;
			// Дескриптор динамической библиотеки
			void* libcrc;
			// Прототип функции из библиотеки
			int (*checkSum)(int);

			// Закрытие канала на чтение данных (дочерний процесс)
			close(pipes[i][0]);

			// Открытие файла
			fd = open (argv[i], O_RDONLY);
			if (fd == -1) {
				fprintf(stderr, "ERROR: Cannot open input file \"%s\": %s.\n", argv[i], strerror(errno)); 
				exit(-1);
			}
			
			// Попытка загрузить библиотеку
			libcrc = dlopen("./libcrc.so", RTLD_LAZY);
			if (!libcrc) {
				fprintf(stderr, "ERROR: Cannot open shared library: %s\n", dlerror());
				exit(-1);
			}

			// Поиск нужной функции
			checkSum = dlsym(libcrc, "checkSum");
			if (checkSum == NULL) {
				fprintf(stderr, "ERROR: Cannot find function in shared library.\n");
				exit(-1);
			}	

			// Обработка файла
			result = checkSum(fd);

			// Закрытие дескриптора файла
			close(fd);

			// Закрытие дескриптора библиотеки
			dlclose(libcrc);

			// Завершение дочернего процесса (вернуть количество байт данных или -1 в случае ошибки)
			exit(write(pipes[i][1], &result, sizeof(int)));
		}
		// Закрытие канала на запись данных (родительский процесс)
		close(pipes[i][1]);	
	}

	sleep(0.1);
	
	// Если выполняется родительский процесс
	// Ожидание окончания выполнения всех запущенных процессов
	for (int i = 1; i < argc; i++) {
		status = waitpid(pid[i], &stat, 0);
		
		// Если завершился необходимый дочерний процесс
		if (pid[i] != status) continue;

		// Если возникли ошибки
		if ((signed char)WEXITSTATUS(stat) == -1) {
			printf("File \"%s\": CRC code: NA.\n", argv[i]);
			continue;
		}
		
		// Считывание результата выполнения из канала и вывод на экран
		read(pipes[i][0], &result, sizeof(int));
		printf("File \"%s\". CRC code: 0x%X.\n", argv[i], result);
	}

	exit(0); 
}