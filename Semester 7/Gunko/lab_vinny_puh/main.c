#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

// Количество мёда у Винни-Пуха под лапами
static float* honey;
// Жив ли Винни-Пух
static int* run;

int main(int argc, char *argv[]) 
{
	// Универсальный буффер
	char buff[64];
	// Количество пчёл
	int bee_count;
	// Сколько мёда приносит одна пчела
	float bee_load = 5;
	// Размер порций Пуха
	float puh_eat_count;
	// Через какое время кормить Пуха в секундах
	int puh_eat_time;

	// Массив PID процессов пчёл
	pid_t* pid;
	
	// Создание области разделяемой памяти
	honey = mmap(NULL, sizeof *honey, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (honey == MAP_FAILED) {
		printf("Невозможно выделить память под хранение запасов мёда!\n");
		exit(-1);
	}

	// Создание области разделяемой памяти
	run = mmap(NULL, sizeof *run, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (run == MAP_FAILED) {
		printf("Невозможно выделить память под хранение жизни медвежонка!\n");
		exit(-1);
	}

	// Пух изначально жив
	*run = 1;

	// Начальное количество мёда
	*honey = 100;

	// Запрос количества пчёл
	do {
		printf("Сколько пчёл приносят Винни-Пуху мёд (каждая пчела может приносить по %.1fл мёда)? ", bee_load);
		scanf("%s", buff);
		bee_count = atoi(buff);
	} while(bee_count <= 0);

	// Выделение памяти под хранение PID процессов пчёл
	pid = calloc(bee_count, sizeof *pid);
	if (pid == NULL) {
		printf("Не удалось выделить память под хранение PID процессов пчёл.\n");
		exit(-1);
	}

	// Запрос времени между кормёшками Винни-Пуха
	do {
		printf("Через какое время Винни-Пух ест мёд (в секундах)? ");
		scanf("%s", buff);
		puh_eat_time = atoi(buff);
	} while(puh_eat_time <= 0);

	// Запрос размера порций Винни-Пуха
	do {
		printf("Как много Винни-Пух ест мёд (если изначально у него %.1fл мёда)? ", *honey);
		scanf("%s", buff);
		sleep(1);
		puh_eat_count = atof(buff);
	} while(puh_eat_count <= 0);

	//srand(time(NULL));

	// Вывов стаи пчёл на помощь Пуху
	for (int i = 0; i < bee_count; i++) {
		pid[i] = fork();
		// Если это пчела
		if (pid[i] == 0) {
			while(1) {
				srand(i + time(NULL));
				sleep(rand() % 9 + 1);
				msync(honey, sizeof *honey, MS_SYNC);
				*honey += bee_load;
				if (*run <= 0) break; 
				printf("Пчела №%d прилетела. Стало мёда: %.1fл.\n", i + 1, *honey); fflush(stdout); 
				msync(honey, sizeof *honey, MS_SYNC);
			}
			exit(0);
		}
	}

	printf("\nВинни-Пух ест мёд каждые %d секунд.\n\n", puh_eat_time);

	// Если код выполняет Винни-Пух
	while(*run > 0) {
		msync(honey, sizeof *honey, MS_SYNC);
		*honey -= puh_eat_count;
		msync(honey, sizeof *honey, MS_SYNC);
		if (*honey <= 0) {
			*run = 0;
			break;
		}
		printf("Пух съел часть мёда. Осталось мёда: %.1fл.\n", *honey); fflush(stdout);
		sleep(puh_eat_time);
	}

	// Вывод сообщения
	printf("\nВинни-Пуху нечего съесть! Увы, медвежонка нужно кормить всегда.\n\n");

	// Ожидание прилёта всех пчёл
	for (int i = 0; i < bee_count; i++) wait(NULL);

	munmap(honey, sizeof *honey);
	munmap(run, sizeof *run);

	exit(0); 
}