// Программа из конспекта "Системное программное обеспечение"
// Файлы, отображаемые на память, синхронизация событиями
// стр. ?? 
// Приложение MFE_SERVER (серверное приложение)
// Демонстрация использования файлов, отображаемых на память, 
// для передачи данных между процессами, синхронизированными событиями (Events)

#include <windows.h>
#include <stdio.h>
#include <conio.h>

// Идентификаторы объектов-событий, которые используются
// для синхронизации задач, принадлежащих разным процессам
HANDLE hEventSend;
HANDLE hEventRecv;
HANDLE hEventTermination;
HANDLE hEvents[2];
// Имя объекта-событий для синхронизации записи и чтения из отображаемого файла
CHAR lpEventSendName[] =
"$MylEventSendName$";
CHAR lpEventRecvName[] =
"$MylEventRecvName$";
// Имя объекта-события для завершения процесса
CHAR lpEventTerminationName[] =
"$MyEventTerminationName$";
// Имя отображния файла на память
CHAR lpFileShareName[] =
"$MyFileShareName$";
// Идентификатор отображения файла на память
HANDLE hFileMapping;
// Указатель на отображенную область памяти
LPVOID lpFileMap;

int main()
{
	DWORD dwRetCode;
	CHAR str[80];
	FILE *hdl;
	DWORD  cbWritten;
	DWORD   total = 0;
	// буфер для  сообщения об ошибке, результата
	char message[80] = { 0 };
	printf("Mapped and shared file, event sync, server process\n");
	// Создаем объекты-события для синхронизации 
	// записи и чтения в отображаемый файл, выполняемого в разных процессах
	hEventSend = CreateEvent(NULL, FALSE, FALSE, lpEventSendName);
	hEventRecv = CreateEvent(NULL, FALSE, FALSE, lpEventRecvName);
	// Если произошла ошибка, получаем и отображаем ее код,
	// а затем завершаем работу приложения
	if (hEventSend == NULL || hEventRecv == NULL)
	{
		fprintf(stdout, "CreateEvent: Error %ld\n",
			GetLastError());
		getch();
		return 0;
	}
	// Если объект-событие с указанным именем существует,
	// считаем, что приложение EVENT уже было запущено
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		printf("\nApplication EVENT already started\n"
			"Press any key to exit...");
		getch();
		return 0;
	}
	// Создаем объект-событие для определения момента
	// завершения работы процесса ввода
	hEventTermination = CreateEvent(NULL,
		FALSE, FALSE, lpEventTerminationName);
	if (hEventTermination == NULL)
	{
		fprintf(stdout, "CreateEvent (Termination): Error %ld\n",
			GetLastError());
		getch();
		return 0;
	}
	// Создаем объект-отображение, файл не создаем!!!
	hFileMapping = CreateFileMapping((HANDLE)0xFFFFFFFF,
		NULL, PAGE_READWRITE, 0, 100, lpFileShareName);
	// Если создать не удалось, выводим код ошибки
	if (hFileMapping == NULL)
	{
		fprintf(stdout, "CreateFileMapping: Error %ld\n",
			GetLastError());
		getch();
		return 0;
	}
	// Выполняем отображение файла на память.
	// В переменную lpFileMap будет записан указатель на
	// отображаемую область памяти
	lpFileMap = MapViewOfFile(hFileMapping,
		FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	// Если выполнить отображение не удалось,
	// выводим код ошибки
	if (lpFileMap == 0)
	{
		fprintf(stdout, "MapViewOfFile: Error %ld\n",
			GetLastError());
		getch();
		return 0;
	}
	// Готовим массив идентификаторов событий
	// для функции WaitForMultipleObjects
	hEvents[0] = hEventTermination;
	hEvents[1] = hEventSend;
	//hEvents[2] = hEventRecv;
	// Цикл чтения/записи. Этот цикл завершает свою работу
	// при завершении процесса ввода
	while (TRUE)
	{
		total=0;
		// Выполняем ожидание одного из двух событий:
		//   - завершение клиентского процесса;
		//   - завершение записи данных клиентом
		dwRetCode = WaitForMultipleObjects(2,
			hEvents, FALSE, INFINITE);
		// Если ожидание любого из двух событий было отменено,
		//  или если произошла ошибка, прерываем цикл
		if (dwRetCode == WAIT_ABANDONED_0 ||
			dwRetCode == WAIT_ABANDONED_0 + 1 ||
			dwRetCode == WAIT_OBJECT_0 ||
			//dwRetCode == WAIT_OBJECT_0 + 1||
			dwRetCode == WAIT_FAILED)
			break;
		// если произошло любое из двух событий из массива
		// Читаем данные (имя файла для обработки) из отображенной
		// области памяти, записанный туда клиентским 
		// процессом, и отображаем его в консольном окне
		else
		{
			puts(((LPSTR)lpFileMap));
			// обработка данных
			strcpy(str, ((LPSTR)lpFileMap));
			if (hdl = fopen(str, "rt")){
				// цикл чтения до конца файла 
				while (!feof(hdl)) {
					// чтение одного символа из файла
					if ((char)fgetc(hdl) == 0x20) total++;
				}
				// сообщение в консоль ошибок 
				sprintf(message, "(Server): file:%s, spaces = %d\n", str, total);
				WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message), &cbWritten, NULL);
				// сообщение в канал 
				sprintf(message, "%d", (int)total);
				//WriteFile(hNamedPipe, message, strlen(message) + 1, &cbWritten, NULL);
				strcpy(((LPSTR)lpFileMap), message);
				// закрытие файла
				fclose(hdl);
			}
			else {
				// сообщение в канал 
				sprintf(message, "(Server)Can't open %s!", str);
				WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message) + 1, &cbWritten, NULL);
				printf("\n");
				//WriteFile(hNamedPipe, message, strlen(message) + 1, &cbWritten, NULL);
				strcpy(((LPSTR)lpFileMap), message);
			}
			//sprintf(((LPSTR)lpFileMap), "Send responce!");
			SetEvent(hEventRecv);
		}
	}

	// Закрываем идентификаторы объектов-событий  
	CloseHandle(hEventSend);
	CloseHandle(hEventRecv);
	CloseHandle(hEventTermination);
	// Отменяем отображение файла
	UnmapViewOfFile(lpFileMap);
	// Освобождаем идентификатор созданного
	// объекта-отображения
	CloseHandle(hFileMapping);

	return 0;
}
