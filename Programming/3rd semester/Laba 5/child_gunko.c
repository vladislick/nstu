// Программа из конспекта "Системное программное обеспечение"
// Почтовые ящики в Windows
// стр. ?? 
// Приложение MSLOTCLIENT (клиентское приложение)
// Демонстрация использования почтовых ящиков 
// для передачи данных между процессами

#include <windows.h>
#include <stdio.h>
#include <conio.h>

DWORD main(int argc, char *argv[])
{
	// Идентификаторы каналов Mailslot
	HANDLE hMailslot1, hMailslot2;
	// Имя клиентского канала Mailslot
	LPSTR  lpszReadMailslotName = "\\\\.\\mailslot\\$Channel2$";
	// Буфер для имени канала Mailslot
	char   szMailslotName[256];
	// Буфер для передачи данных через канал
	char   szBuf[512];
	// Код возврата из функций
	BOOL   fReturnCode;
	// Размер сообщения в байтах
	DWORD  cbMessages;
		// Количество сообщений в канале Mailslot2
	DWORD  cbMsgNumber;
	// Количество байт, переданных через канал
	DWORD  cbWritten;
	// Количество байт данных, принятых через канал
	DWORD  cbRead;
	printf("Mailslot client demo\n");
	printf("Syntax: mslotclient [servername]\n");
	// Если при запуске было указано имя срвера,
	// указываем его в имени канала Mailslot
	if (argc > 1)
		sprintf(szMailslotName, "\\\\%s\\mailslot\\$Channel1$",
		argv[1]);
	// Если имя сервера задано не было, создаем канал
	// с локальным процессом
	else
		strcpy(szMailslotName, "\\\\.\\mailslot\\$Channel1$");
	// Открываем канал с процессом MSLOTSERVER
	hMailslot1 = CreateFile(
		szMailslotName, GENERIC_WRITE,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	// Если возникла ошибка, выводим ее код и 
	// завершаем работу приложения
	if (hMailslot1 == INVALID_HANDLE_VALUE)
	{
		fprintf(stdout, "CreateFile for send: Error %ld\n",
			GetLastError());
		_getch();
		return 0;
	}
	// Выводим сообщение о создании канала с сервером
	fprintf(stdout, "Connected. Type 'exit' to terminate\n");
	// Создаем канал Mailslot2, имеющий имя lpszReadMailslotName
	hMailslot2 = CreateMailslot(
		lpszReadMailslotName, 0,
		MAILSLOT_WAIT_FOREVER, NULL);
	// Если возникла ошибка, выводим ее код и зваершаем
	// работу приложения
	if (hMailslot2 == INVALID_HANDLE_VALUE)
	{
		fprintf(stdout, "CreateMailslot for reply: Error %ld\n",
			GetLastError());
			CloseHandle(hMailslot2);
		_getch();
		return 0;
	}
	// Выводим сообщение о создании канала
	fprintf(stdout, "Mailslot for reply created\n");
	// Цикл посылки команд через канал
	while (1)
	{
		// Выводим приглашение для ввода команды
		printf("cmd>");
		// Вводим текстовую строку
		gets(szBuf);
		// Передаем введенную строку серверному процессу в качестве команды
		if (!WriteFile(hMailslot1, szBuf, strlen(szBuf) + 1,
			&cbWritten, NULL))
			break;
		// В ответ на команду "exit" завершаем цикл
		// обмена данными с серверным процессом
		if (!strcmp(szBuf, "exit"))
			break;
		// ожидаем ответ
		fprintf(stdout, "Waiting for reply...\n");
		// Определяем состояние канала Mailslot2
		fReturnCode = GetMailslotInfo(
			hMailslot2, NULL, &cbMessages,
			&cbMsgNumber, NULL);
		if (!fReturnCode)
		{
			fprintf(stdout, "GetMailslotInfo for reply: Error %ld\n",
				GetLastError());
			_getch();
			break;
		}
		// Чтение ответа
		// Если в канале есть Mailslot сообщения,
		// читаем первое из них и выводим на экран
		//if (cbMsgNumber != 0)
		//while (cbMsgNumber)
		{
			if (ReadFile(hMailslot2, szBuf, 512, &cbRead, NULL))
			{
				// Выводим принятую строку на консоль 
				printf("Received: <%s>\n", szBuf);
			}
		}
	}
	// Закрываем идентификатор канала
	CloseHandle(hMailslot1);
	CloseHandle(hMailslot2);
	return 0;
}
