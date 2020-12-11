// Программа из конспекта "Системное программное обеспечение"
// Версия для Windows
// стр. ?? 
// Приложение Service
// Служба Windows, использующая объекты ядра
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
// -----------------------------------------------------
// Глобальные переменные
// -----------------------------------------------------
// Код ошибки
DWORD dwErrCode;
// Текущее состояние сервиса
SERVICE_STATUS ss;
// Идентификатор сервиса
SERVICE_STATUS_HANDLE ssHandle;
#define MYServiceName 	"Sample service"
#define LOG_FILE_PATH 	"C:\\logfile.log"
#define BUF_SIZE		1024
// функции службы, выполняющие работу
extern int	Server();
extern int 	ServiceStart();
extern void	ServiceStop();
extern HANDLE hEventSend, hEventRecv, hFileMapping, lpFileMap;
// функции управления службой
void WINAPI ServiceMain(DWORD dwArgc, LPSTR *lpszArv);
void WINAPI ServiceControl(DWORD dwControlCode);
void ReportStatus(DWORD dwCurrentState,	DWORD dwWin32ExitCode, DWORD dwWaitHint);
PROCESS_INFORMATION pid;

// Logging
int addLogMessage(const char* text)
{
    // Log file descriptor
    HANDLE hFile;
    // Count of written bytes
    DWORD cdWritten;
    // Size of the log file
    DWORD Sz;
    // Date and time variable
    time_t currentTime = time(NULL);
    // Buffer with processed log message
    char buf[BUF_SIZE];
    
    hFile = CreateFile(LOG_FILE_PATH, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
    if (!hFile) 
        return (-1);
    else {
        // Find the end of the log file
        GetFileSize(hFile, &Sz);
        SetFilePointer(hFile, 0, NULL, FILE_END);
        // Write a new information
        sprintf(buf, "===> %s%s\n\n", ctime(&currentTime), text);
        WriteFile(hFile, buf, strlen(buf), &cdWritten, NULL);
        // Close the file
        CloseHandle(hFile);
        return (int)cdWritten;
    }
}

void main(int agrc, char *argv[])
{
    char buffer[BUF_SIZE];
    // Entry point table (It's Windows)
    SERVICE_TABLE_ENTRY DispatcherTable[] = {
        {
            // Service name
            MYServiceName,
            // Service's main function
            (LPSERVICE_MAIN_FUNCTION) ServiceMain
        },
        { NULL, NULL }
    };
    addLogMessage("Sample service entry point");
    // Dispatcher launch
    if (!StartServiceCtrlDispatcher(DispatcherTable))
    {
        sprintf(buffer,	"Cannot execute StartServiceCtrlDispatcher (ERROR #%ld)", GetLastError());
        addLogMessage(buffer);
        return;
    }
}

void WINAPI ServiceMain(DWORD argc, LPSTR *argv)
{
    char buf[256];
    int res = 0;

    // Регистрируем управляющую функцию сервиса
    ssHandle = RegisterServiceCtrlHandler(MYServiceName, ServiceControl);
    if (!ssHandle)
    {
        addLogMessage("Error RegisterServiceCtrlHandler");
         return;
    }
    // Устанавливаем состояние сервиса
    // Сервис работает как отдельный процесс
    ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    // Код ошибки при инициализации и завершения сервиса не используется
    ss.dwServiceSpecificExitCode = 0;
    // Начинаем запуск сервиса.
    // Прежде всего устанавливаем состояние ожидания запуска сервиса
    ReportStatus(SERVICE_START_PENDING, NO_ERROR, 30000);
    addLogMessage("Service starting...");
    // Вызываем функцию, которая выполняет все необходимые инициализирующие действия
    res = ServiceStart();
    if (res < 0)
    {
        sprintf(buf, "Error init server %d", res);
        addLogMessage(buf);
        ServiceControl(SERVICE_CONTROL_STOP);
        return;
    }
    // После завершения инициализации устанавливаем состояние работающего сервиса
    ReportStatus(SERVICE_RUNNING, NOERROR, 0);
    addLogMessage("Service started!");
    // основное тело службы
    if (Server() > 0)
    {
        addLogMessage("Server MF started!");
    }
    else
    {
        sprintf(buf, "Error starting server %d", res);
        addLogMessage(buf);
        ServiceControl(SERVICE_CONTROL_STOP);
    }
    return;
}
// -----------------------------------------------------
// Функция ServiceControl
// Точка входа функции обработки команд
// -----------------------------------------------------
void WINAPI ServiceControl(DWORD dwControlCode)
{
    // Анализируем код команды и выполняем эту команду
    switch (dwControlCode)
    {
        // Команда остановки сервиса
    case SERVICE_CONTROL_STOP:
    {
        // Устанавливаем состояние ожидания остановки
        ss.dwCurrentState = SERVICE_STOP_PENDING;
        ReportStatus(ss.dwCurrentState, NOERROR, 0);
        addLogMessage("Service stopping...");
        // Выполняем остановку сервиса, вызывая функцию, которая выполняет все необходимые для этого действия
        ServiceStop();
        // Отмечаем состояние как остановленный сервис
        ReportStatus(SERVICE_STOPPED, NOERROR, 0);
        addLogMessage("Service stopped!");
        break;
    }
    // Определение текущего состояния сервиса
    case SERVICE_CONTROL_INTERROGATE:
    {
        // Возвращаем текущее состояние сервиса
        ReportStatus(ss.dwCurrentState, NOERROR, 0);
        break;
    }
    // В ответ на другие команды просто возвращаем текущее состояние сервиса
    default:
    {
        ReportStatus(ss.dwCurrentState, NOERROR, 0);
        break;
    }
    }
}
// -----------------------------------------------------
// Функция ReportStatus
// Посылка состояния сервиса системе управления сервисами
// -----------------------------------------------------
void ReportStatus(DWORD dwCurrentState,
    DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    // Счетчик шагов длительных операций
    static DWORD dwCheckPoint = 1;
    // Если сервис не находится в процессе запуска, его можно остановить
    if (dwCurrentState == SERVICE_START_PENDING)
        ss.dwControlsAccepted = 0;
    else
        ss.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    // Сохраняем состояние, переданное через параметры функции
    ss.dwCurrentState = dwCurrentState;
    ss.dwWin32ExitCode = dwWin32ExitCode;
    ss.dwWaitHint = dwWaitHint;
    // Если сервис не работает и не остановлен, увеличиваем значение счетчика шагов длительных операций
    if ((dwCurrentState == SERVICE_RUNNING) ||
        (dwCurrentState == SERVICE_STOPPED))
        ss.dwCheckPoint = 0;
    else
        ss.dwCheckPoint = dwCheckPoint++;
    // Вызываем функцию установки состояния
    SetServiceStatus(ssHandle, &ss);
}
