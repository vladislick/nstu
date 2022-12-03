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
#include <time.h>

// Факториал числа n
int f(int n) {
	if (n == 0) return 1;
	return n * f(n - 1);
}

// Структура типа массив
typedef struct {
	// Значение элементов массива
	int* value;
	// Количество элементов
	unsigned int count;
} arr_t;

int main(int argc, char *argv[]) {
	// Идентификатор процесса, в котором осуществляется бойня
	arr_t pid;
	// Количество бойцов команды
	arr_t team_units;
	// Общий список команд
	arr_t team_list;
	// Номер текущей битвы
	int index = 0;
	// Номер команды, не попавшей в битву
	int excluded_team = 0;
	// Универсальный буфер
	char buff[64];

	int status, stat;

	// Запрос количества команд
	do {
		printf("Введите количество команд: ");
		scanf("%s", buff);
		team_units.count = team_list.count = atoi(buff);
	} while(team_units.count <= 1);

	// Создание нужного количества команд
	team_units.value = calloc(team_units.count, sizeof(int));
	team_list.value = calloc(team_list.count, sizeof(int));
	// Выделение памяти под хранение PID дочерних процессов
	pid.value = calloc(team_list.count, sizeof(int));
	
	if (pid.value == NULL) {
		printf("ОШИБКА: Невозможно выделить память под хранение массива pid!\n");
		exit(-1);
	}

	if (team_units.value == NULL) {
		printf("ОШИБКА: Невозможно выделить память под хранение массива team_units!\n");
		exit(-1);
	}

	if (team_list.value == NULL) {
		printf("ОШИБКА: Невозможно выделить память под хранение массива team_list!\n");
		exit(-1);
	}
	
	// Заполнение данных о командах
	for (int i = 0; i < team_units.count; i++) {
		// По три бойца каждой команде
		team_units.value[i] = 3;
		// Запись номера команды в список
		team_list.value[i] = i;
	}

	// Цикл битв команд, пока в списке участников не останется лишь одна команда
	while (team_list.count > 1) {
		// Количество битв из текущего списка команд
		pid.count = team_list.count / 2;
		
		// Запуск битв в дочерних процессах
		for (int i = 0; i < pid.count; i++) {
			// Увеличение номера битвы
			index++;
			srand(index + time(NULL));
			// Запуск дочернего процесса 
			pid.value[i] = fork();
			// Если выполняется код дочернего процесса
			if (pid.value[i] == 0) {
				// Номер команды №1
				int team1 = team_list.value[i * 2];
				// Номер команды №2
				int team2 = team_list.value[i * 2 + 1];
				// Значения прибавления или убийства бойцов
				int temp;
				// Вывод приветственного сообщения
				printf("\n[Битва №%d] Сражаются команды №%d (%d бойцов) и №%d (%d бойцов)!\n", index, team1 + 1, team_units.value[team1], team2 + 1, team_units.value[team2]);
				
				// Сражение до тех пор, пока не погибнет какая-либо из команд
				while(1) {
					// Вызов помощи команде №1
					temp = rand() % 3;
					team_units.value[team1] += temp;
					if (temp > 0) 
						printf("[Битва №%d] К команде №%d примкнуло %d бойцов. Теперь их %d!\n", index, team1 + 1, temp, team_units.value[team1]);
					else 
						printf("[Битва №%d] К команде №%d никто не пришёл помочь. Очень жаль!\n", index, team1 + 1);

					// Вызов помощи команде №2
					temp = rand() % 3;
					team_units.value[team2] += temp;
					if (temp > 0) 
						printf("[Битва №%d] К команде №%d примкнуло %d бойцов. Теперь их %d!\n", index, team2 + 1, temp, team_units.value[team2]);
					else 
						printf("[Битва №%d] К команде №%d никто не пришёл помочь. Очень жаль!\n", index, team2 + 1);

					// Потери команды №1
					temp = rand() % 5;
					team_units.value[team1] -= temp;
					// Проверка, жив ли кто-нибудь у команды
					if (team_units.value[team1] <= 0) {
						printf("[Битва №%d] К сожалению, у команды №%d погибли все бойцы!\n", index, team1 + 1);
						printf("[Битва №%d] Сражение завершено. Побеждает и проходит дальше команда №%d!\n", index, team2 + 1); 
						exit(team2);
					}
					if (temp > 0) 
						printf("[Битва №%d] У команды №%d погибло %d бойцов. Их всего осталось %d!\n", index, team1 + 1, temp, team_units.value[team1]);
					else 
						printf("[Битва №%d] Команде №%d очень повезло и у них никто не погиб! Так что их всё ещё %d.\n", index, team1 + 1, team_units.value[team1]);

					// Потери команды №2
					temp = rand() % 5;
					team_units.value[team2] -= temp;
					// Проверка, жив ли кто-нибудь у команды
					if (team_units.value[team2] <= 0) {
						printf("[Битва №%d] К сожалению, у команды №%d погибли все бойцы!\n", index, team2 + 1);
						printf("[Битва №%d] Сражение завершено. Побеждает и проходит дальше команда №%d!\n", index, team1 + 1); 
						exit(team1);
					}
					if (temp > 0) 
						printf("[Битва №%d] У команды №%d погибло %d бойцов. Их всего осталось %d!\n", index, team2 + 1, temp, team_units.value[team2]);
					else 
						printf("[Битва №%d] Команде №%d очень повезло и у них никто не погиб! Так что их всё ещё %d.\n", index, team2 + 1, team_units.value[team2]);
				}
			}
			// Для более читаемого вывода задрежка перед началом новой битвы
			usleep(1000);
		}

		// Есть ли ещё команда, которая не вошла в битву
		if (team_list.count % 2) excluded_team = team_list.value[team_list.count - 1];
		else excluded_team = 0;
		
		// Создание нового списка оставшихся команд
		team_list.count = pid.count;
		
		// Если есть команда, которая не попапа в битву
		if (excluded_team > 0) team_list.count++;
		
		// Если есть неучаствующая команда, записать её номер в конец списка
		team_list.value[team_list.count - 1] = excluded_team;

		// Ожидание завершения каждой из битв
		for (int i = 0; i < pid.count; i++) {
			status = waitpid(pid.value[i], &stat, 0);
			// Если завершился необходимый дочерний процесс
			if (pid.value[i] != status) continue;

			// Запись индекса выигравшей команды в новый список команд 
			team_list.value[i] = WEXITSTATUS(stat);
		}

		//for (int i = 0; i < team_units.count; i++) printf("Команда №%d: бойцов %d.\n", i + 1, team_units.value[i]);
	}

	// Если битв было много
	if (index > 3) {
		printf("\nБитвы завершены! Это было долгое и напряжённое состязание между командами.\n");
		printf("Все команды проявили себя отлично, боролись до самого конца.\n");
		printf("И у нас есть победитель, показавший себя с самой лучшей стороны.\n");
		printf("Итак, победителем всех состязаний по праву признаётся...\nКоманда №%d! Поздравляем победителей!\n", team_list.value[0] + 1);
	} 
	// Иначе оратор скромничает
	else {
		printf("\nБитвы завершены!\nПобедила команда №%d! Поздравляем победителей!\n", team_list.value[0] + 1);
	}

	free(team_list.value);
	free(team_units.value);
	free(pid.value);

	exit(0); 
}