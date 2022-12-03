#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

// Остаток золота в шахте
static int* reserves;

int main(int argc, char *argv[]) 
{
	// Универсальный буффер
	char buff[64];
	// Количество юнитов
	int units;
	// Грузоподъемность одного юнита
	int unit_load;
	// Универсальная переменная
	int temp;
	// PID дочерних процессов
	pid_t* pid;
	// Время начала выполнения задания
	time_t begin;
	
	// Создание области разделяемой памяти
	reserves = mmap(NULL, sizeof *reserves, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (reserves == MAP_FAILED) {
		printf("Невозможно выделить память под хранение запасов золота: %s!\n", strerror(errno));
		exit(-1);
	}

	// Запрос количества юнитов
	do {
		printf("Введите количество юнитов: ");
		scanf("%s", buff);
		units = atoi(buff);
	} while(units <= 0);

	// Выделение памяти под хранение PID дочерних процессов
	pid = calloc(units, sizeof *pid);
	if (pid == NULL) {
		printf("Не удалось выделить память под хранение PID Юнитов: %s!\n", strerror(errno));
		exit(-1);
	}

	// Запрос грузоподъемности одного юнита
	do {
		printf("Введите грузоподъемность юнита: ");
		scanf("%s", buff);
		unit_load = atoi(buff);
	} while(unit_load <= 0);

	// Запрос размера шахты
	do {
		printf("Введите размер шахты (количество золота): ");
		scanf("%s", buff);
		temp = atof(buff);
	} while(temp <= 0);

	// Установка необходимого размера шахты
	*reserves = temp;

	// Создание юнитов
	for (int i = 0; i < units; i++) {
		pid[i] = fork();
		// Если не удалось создать дочерний процесс
		if (pid[i] == -1) {
			printf("Не удалось запустить юнит #%d: %s.\n", i, strerror(errno));
			continue;
		}
		// Если это юнит выполняет код
		if (pid[i] == 0) {
			// В бесконечном цикле добывает золото
			while(1) {
				// Обновление генератора псевдослучайных чисел
				srand(i + time(NULL));
				// Юнит задерживается на случайное время
				sleep(rand() % 5 + 1);
				// Синхронизация разделяемой памяти
				msync(reserves, sizeof *reserves, MS_SYNC);
				// Юнит добывает золото из шахты
				if (*reserves > 0) *reserves -= unit_load;
				// Или завершает свою работу
				else {
					printf("Юнит #%d: Шахта пуста, возврат на базу.\n", i+1); fflush(stdout); 
					break;
				}
				// Вывод статистики на экран
				printf("Юнит #%d: Добыл %d золота, в шахте ещё осталось %d золота.\n", i+1, unit_load, *reserves); fflush(stdout); 
				// Синхронизация разделяемой памяти
				msync(reserves, sizeof *reserves, MS_SYNC);
			}
			exit(0);
		}
	}

	printf("\nВсе юниты отправлены на задание.\n\n");
	// Запоминание текущего времени
	begin = time(NULL);

	// Ожидание всех юнитов 
	for (int i = 0; i < units; i++) wait(NULL);

	printf("\nШахта добыта полностью. Юниты возвращены на базу.\nНа задание суммарно было затрачено %ldс.\n", time(NULL) - begin);

	munmap(reserves, sizeof *reserves);
	free(pid);

	exit(0); 
}