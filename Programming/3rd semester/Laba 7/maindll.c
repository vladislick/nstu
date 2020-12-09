#include <windows.h>
#include <stdio.h>

__declspec(dllexport) int __cdecl  bufferProcessing(CHAR*, int);
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		printf("DLL is loaded\n");
		break;
	case DLL_THREAD_ATTACH:
		printf("A thread is created in this process\n");
		break;
	case DLL_THREAD_DETACH:
		printf("A thread is destroyed in this process\n");
		break;
	case DLL_PROCESS_DETACH:
		printf("DLL is unloaded\n");
		break;
	}
	return TRUE;
}