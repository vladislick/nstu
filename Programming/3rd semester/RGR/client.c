// Программа из конспекта "Системное программное обеспечение"
// Файлы, отображаемые на память, синхронизация событиями
// стр. ?? 
// Приложение MF_CLIENT (клиентское приложение)
// Демонстрация использования файлов, отображаемых на память, 
// для передачи данных между процессом и службой, синхронизированными событиями (Events)

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

// Идентификаторы объектов-событий, которые используются
// для синхронизации, область видимости - сеанс клиента
HANDLE hEventSend;
HANDLE hEventRecv;
// Имя объекта-событий для синхронизации записи и чтения из отображаемого файла
CHAR lpEventSendName[] ="Session\\2\\$MylEventSendName$"; 
CHAR lpEventRecvName[] ="Session\\2\\$MylEventRecvName$";
// Имя отображния файла на память
CHAR lpFileShareName[] ="Session\\2\\$MyFileShareName$";
// Идентификатор отображения файла на память
HANDLE hFileMapping;
// Указатель на отображенную область памяти
LPVOID lpFileMap;

int main(int argc, char *argv[])
{
	CHAR str[80];
	DWORD dwRetCode;
	 printf("Mapped and shared file, event sync, client process.\nEnter <Exit> to terminate...\n\n");
	// Открываем объекты-события для синхронизации  чтения и записи
	hEventSend = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpEventSendName);
	hEventRecv = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpEventRecvName);
	if (hEventSend == NULL || hEventRecv == NULL)
	{
		fprintf(stdout, "OpenEvent: Error %ld\n",
			GetLastError());
		getch();
		CloseHandle(hEventSend);
		CloseHandle(hEventRecv);
		return-1;
	}
	// Открываем объект-отображение
	hFileMapping = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, lpFileShareName);
	// Если открыть не удалось, выводим код ошибки
	if (hFileMapping == NULL)
	{
		fprintf(stdout, "OpenFileMapping: Error %ld\n",
			GetLastError());
		getch();
		CloseHandle(hFileMapping);
		return -2;
	}
	// Выполняем отображение файла на память.
	// В переменную lpFileMap будет записан указатель на отображаемую область памяти
	lpFileMap = MapViewOfFile(hFileMapping,	FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	// Если выполнить отображение не удалось, выводим код ошибки
	if (lpFileMap == 0)
	{
		fprintf(stdout, "MapViewOfFile: Error %ld\n",
			GetLastError());
		getch();
		return -3;
	}
	// Цикл чтения/записи данных. Этот цикл завершает свою работу,
	// когда пользователь набирает коанду <Exit> 
	while (TRUE)
	{
		printf("====> ");
		// Читаем введенную строку
		gets(str);
		// Записываем строку в отображенную память, доступную серверному процессу
		strcpy((LPSTR)lpFileMap, str);
		// Если введена команда <Exit>, прерываем цикл
		if (!strcmp(str, "exit") || !strcmp(str, "Exit") || !strcmp(str, "EXIT"))
			break;
		// Устанавливаем объект-событие в отмеченное состояние
		SetEvent(hEventSend);
		// ждем ответа
		dwRetCode = WaitForSingleObject(hEventRecv, INFINITE);
		// если ответ получен - выводим, если ошибка - выходим
		if (dwRetCode == WAIT_OBJECT_0) puts((LPSTR)lpFileMap);
		if (dwRetCode == WAIT_ABANDONED_0 || dwRetCode == WAIT_FAILED)
		{
			printf("\nError waiting responce!\n)");
			//break;
		}
		printf("\n");
	}
	// Закрываем идентификаторы объектов-событий
	CloseHandle(hEventSend);
	CloseHandle(hEventRecv);
	// Отменяем отображение файла
	UnmapViewOfFile(lpFileMap);
	// Освобождаем идентификатор созданного объекта-отображения
	CloseHandle(hFileMapping);
	return 0;
}

