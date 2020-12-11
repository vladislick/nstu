#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <AclAPI.h> // for SetSecurityInfo

// Maximum size of strings in the app
#define BUF_SIZE    256
// Name of library (DLL)
#define LIB_NAME    "maindll.dll"
// The name of function in the library
#define FUNC_NAME   "bufferProcessing"

extern int addLogMessage(const char* text);
int ServiceStart();
void ServiceStop();
// Идентификаторы объектов - событий, которые используются
// для синхронизации задач, принадлежащих разным процессам, для службы - ГЛОБАЛЬНЫЕ!
HANDLE hEventSend;
HANDLE hEventRecv;
HANDLE hEventTermination;
HANDLE hEvents[2];
// Имя объекта-событий для синхронизации записи и чтения из отображаемого файла
CHAR lpEventSendName[] = "Session\\2\\$MylEventSendName$"; 
CHAR lpEventRecvName[] = "Session\\2\\$MylEventRecvName$";
// Имя отображния файла на память
CHAR lpFileShareName[] = "Session\\2\\$MyFileShareName$";
// Идентификатор отображения файла на память
HANDLE hFileMapping;
// Указатель на отображенную область памяти
LPVOID lpFileMap;
// Library descriptor
HINSTANCE hLib;
// Function prototype from the library
int (*bufferProcessing)(CHAR*, int);

int Server()
{
	DWORD 	dwRetCode;
	// Library name and function in the library name, just for fun
    char    libName[] = LIB_NAME,
            funcName[] = FUNC_NAME;
	DWORD  cbWritten;
    // Buffer for read/write the files
    CHAR Buffer[BUF_SIZE];
    // File descriptors (hIn - input file, hOut - output file)
    HANDLE hIn, hOut; 
    // Numbers of read/written bytes
    DWORD nIn, nOut;
    // Message from server app received by the pipe
    char    input[BUF_SIZE] = { 0 };
    // Name of input file
    char    fileName[BUF_SIZE] = { 0 };
    // Name for output file
    char    fileOutName[BUF_SIZE] = { 0 };
    int     maxChanges = 0;
    // Current number of changes in file
    int     changes = 0, clientNum = 0;
	FILE 	*hdl;
	DWORD   total = 0;
	// буфер для  сообщения об ошибке, результата
	char message[80] = { 0 };
	addLogMessage("Server Ready!");
	while (TRUE)
	{
		clientNum++;
		sprintf(message, "Wait a message #%d...", clientNum);
		addLogMessage(message);
		dwRetCode = WaitForSingleObject(hEventSend, INFINITE);
		// Если ожидание было отменено, или если произошла ошибка, прерываем цикл
		if (dwRetCode == WAIT_ABANDONED_0 || dwRetCode == WAIT_FAILED)
			break;
		// Читаем данные (имя файла для обработки) из отображенной области памяти, 
		// записанный туда клиентским процессом, и отображаем его в консольном окне
		else
		{
			puts(((LPSTR)lpFileMap));
			// обработка данных
			strcpy(input, (LPSTR)lpFileMap);
			sprintf(message, "Get message: %s.", input);
			addLogMessage(message);
			
			// Check the library
            hLib = LoadLibrary(libName); 
			if (hLib == NULL) {
                sprintf(message, "Cannot load library \"%s\" (ERROR #%ld)", libName, GetLastError());
                addLogMessage(message);
				strcpy((LPSTR)lpFileMap, message);
				SetEvent(hEventRecv);
				continue;
            } 

            // Check the function in library
            bufferProcessing = (int(*)(CHAR*, int))GetProcAddress(hLib, funcName);
            if (bufferProcessing == NULL) {
                sprintf(message, "Cannot find function \"%s\" in the library (ERROR #%ld)", funcName, GetLastError());
                addLogMessage(message);
				FreeLibrary(hLib);
				strcpy((LPSTR)lpFileMap, message);
				SetEvent(hEventRecv);
				continue;
            }

            // Get filename and maximum of changes
            strcpy(fileName, input);
            char isFile = 1, quotes = 0;
            maxChanges = 0;
            for (int i = 0; i < strlen(input); i++) {
                if (input[i] == '"') {
					quotes++;
				} else if (input[i] == ' ' && quotes % 2 == 0) {
                    ZeroMemory(fileName, BUF_SIZE);
                    strncpy(fileName, input, i);
                    isFile = 0;
                } else if (!isFile && input[i] > 47 && input[i] < 58) {
                    if (maxChanges > 0) maxChanges *= 10;
                    maxChanges += input[i] - 48;
                }
            }
			
			if (quotes % 2 == 0)
				for (int i = 0; i < strlen(fileName); i++)
					if (fileName[i] == '"') 
						for (int j = i; j < strlen(fileName); j++)
							fileName[j] = fileName[j + 1];
            
			if (maxChanges == 0) maxChanges = 99999;

            // Get the name of output file 
            ZeroMemory(fileOutName, BUF_SIZE);
            if (strchr(fileName, '.') == NULL) // If input file hasn't file extension
                strcpy(fileOutName, fileName);
            else 
                strncpy(fileOutName, fileName, strchr(fileName, '.') - fileName);
            strcat(fileOutName, ".out");

            // Сheck the recognized data
            if (!strcmp(fileName, "")) {
                sprintf(message, "Incorrect data from a client");
                addLogMessage(message);
				FreeLibrary(hLib);
				strcpy((LPSTR)lpFileMap, message);
				SetEvent(hEventRecv);
				continue;
            }

            // Try to open input file (READ MODE)
            hIn = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); 
            if (hIn == INVALID_HANDLE_VALUE) {
                sprintf(message, "Cannot open input file \"%s\" (ERROR #%ld)", fileName, GetLastError());
                addLogMessage(message);
				FreeLibrary(hLib);
				strcpy((LPSTR)lpFileMap, message);
				SetEvent(hEventRecv);
				continue;
            } 

            // Try to open output file (WRITE MODE)
            hOut = CreateFile (fileOutName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
            if(hOut == INVALID_HANDLE_VALUE) {
                sprintf(message, "Cannot open output file \"%s\" (ERROR #%ld)", fileOutName, GetLastError());
                addLogMessage(message);
				FreeLibrary(hLib);
				CloseHandle(hIn);
				strcpy((LPSTR)lpFileMap, message);
				SetEvent(hEventRecv);
				continue;
            }

            // File reading-writing 
            ZeroMemory(Buffer, BUF_SIZE);
            while(ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) { 
                changes = (*bufferProcessing)(Buffer, maxChanges);
                WriteFile(hOut, Buffer, nIn, &nOut, NULL); 
                if (nIn != nOut) {
                    sprintf(message, "Catch an error with file reading/writing");
                	addLogMessage(message);
					FreeLibrary(hLib);
					CloseHandle(hIn);
					CloseHandle(hOut);
					strcpy((LPSTR)lpFileMap, message);
					SetEvent(hEventRecv);
					continue;
                }
                ZeroMemory(Buffer, BUF_SIZE);
            }

            // An answer to the client what was happened
            sprintf(message, "File proccess done. Maximum changes is set to %d.\n%d changes written in file \"%s\"", maxChanges, changes, fileOutName);
            addLogMessage(message);
            strcpy((LPSTR)lpFileMap, message);

            // Close files and library
            FreeLibrary(hLib);
            CloseHandle(hIn); 
            CloseHandle(hOut);
			addLogMessage("The answer send to a client!");
			SetEvent(hEventRecv);
		}
	}

	// Закрываем идентификаторы объектов-событий  в ServiceStop()
	return 0;
}
int ServiceStart()
{
	char message[80] = { 0 };
	DWORD res;
	// Создаем объекты-события для синхронизации записи и чтения в отображаемый файл, выполняемого в разных процессах
	/*/ Здесь нужны атрибуты безопасности, позволяющие использовать объекты ВСЕМ юзерам
	// а не создателю и администраторам, как по умолчанию...
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
	addLogMessage("Creating security attributes ALL ACCESS for EVERYONE!!!\n");
	// Создаем дескриптор безопасности
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	// DACL не установлен (FALSE) - объект незащищен
	SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
	// Настраиваем атрибуты безопасности, передавая туда указатель на дескриптор безопасности sd и создаем объект-событие
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor= &sd;
	sa.bInheritHandle = FALSE;
	// проверяем структуру дескриптора безопасности
	if (!IsValidSecurityDescriptor(&sd))
	{
		res = GetLastError();
		addLogMessage("Security descriptor is invalid.\n");
		sprintf(message, "The last error code: %u\n", res);
		return -(int)res;
	}*/
	// устанавливаем новый дескриптор безоп
	hEventSend = CreateEvent(NULL, FALSE, FALSE, lpEventSendName);
	hEventRecv = CreateEvent(NULL, FALSE, FALSE, lpEventRecvName);

	// альтернативное задание прав всем на все
	SetSecurityInfo(hEventSend,	SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL,NULL,NULL,	NULL);
	SetSecurityInfo(hEventRecv, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL);
	// Если произошла ошибка, получаем и отображаем ее код, а затем завершаем работу приложения
	addLogMessage("Creating events");
	if (hEventSend == NULL || hEventRecv == NULL)
	{
		sprintf(message, "Cannot execute CreateEvent (ERROR #%ld)", GetLastError());
		addLogMessage(message);
		return (-1);
	}
	// Создаем объект-отображение, файл не создаем!!!
	hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE, lpFileShareName);
	SetSecurityInfo(hFileMapping, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL);
	addLogMessage("Creating Mapped file");
	// Если создать не удалось, выводим код ошибки
	if (hFileMapping == NULL)
	{
		sprintf(message, "Cannot execute CreateFileMapping (ERROR #%ld)", GetLastError());
		addLogMessage(message);
		//getch();
		return -2;
	}
	// Выполняем отображение файла на память.
	// В переменную lpFileMap будет записан указатель на отображаемую область памяти
	lpFileMap = MapViewOfFile(hFileMapping,
		FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	// Если выполнить отображение не удалось, выводим код ошибки
	if (lpFileMap == 0)
	{
		sprintf(message, "MapViewOfFile: Error %ld\n", GetLastError());
		addLogMessage(message);
		return -3;
	}
	return 0;
}

void ServiceStop()
{
	CloseHandle(hEventSend);
	CloseHandle(hEventRecv);
	// Отменяем отображение файла
	UnmapViewOfFile(lpFileMap);
	// Освобождаем идентификатор созданного объекта-отображения
	CloseHandle(hFileMapping);
	addLogMessage("All Kernel objects closed!");
}