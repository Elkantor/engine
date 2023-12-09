#pragma once

#include <Windows.h>
#include <DbgHelp.h>
#include <stddef.h>
#include <string.h>
#include "../../../window.c"

typedef struct
{
    int m_elems[256];
    size_t m_size;
} keysPressedArray_t;

size_t keysPressedArrayCapacityGet(void)
{
    return offsetof(keysPressedArray_t, m_size) / sizeof(int);
}

typedef struct
{
	char m_name[256];
    keysPressedArray_t m_keysPressed;
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_startCount;
} app_t;

size_t appNameCapacityGet(void)
{
	return offsetof(app_t, m_keysPressed) / sizeof(char);
}

int kickstart(int _argc, char** _argv);

typedef BOOL(__stdcall *miniDumpWriteFunc)(IN HANDLE hProcess, IN DWORD ProcessId, IN HANDLE hFile, IN MINIDUMP_TYPE DumpType,
	IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	OPTIONAL IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    OPTIONAL IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL);

LONG WINAPI crashHandlerExceptionFilterGet(EXCEPTION_POINTERS* _pEx)
{
	HMODULE dbghelp = LoadLibraryA("dbghelp.dll");
	miniDumpWriteFunc miniDumpWrite = (miniDumpWriteFunc)GetProcAddress(dbghelp, "MiniDumpWriteDump");

	HANDLE file = CreateFileA("app.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file != INVALID_HANDLE_VALUE) 
	{
		MINIDUMP_EXCEPTION_INFORMATION stMDEI;
		stMDEI.ThreadId = GetCurrentThreadId();
		stMDEI.ExceptionPointers = _pEx;
		stMDEI.ClientPointers = TRUE;
		miniDumpWrite(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpNormal, &stMDEI, NULL, NULL);
		CloseHandle(file);
	}

	FreeLibrary(dbghelp);

	return EXCEPTION_CONTINUE_SEARCH;
}

void crashHandlerInit(void)
{
	SetUnhandledExceptionFilter(crashHandlerExceptionFilterGet);
}

void appNameSet(app_t* _app, const char* _name)
{
	const size_t capacity = appNameCapacityGet();
	strncpy(_app->m_name, _name, capacity);
}

int appInit(app_t* _app, const char* _name, int _width, int _height, windowOptions_t* _winOptions, framebufferOptions_t* _frameBufferOptions)
{
	crashHandlerInit();

	memset(_app, 0, sizeof(*_app));

	QueryPerformanceCounter(&_app->m_startCount);
	QueryPerformanceFrequency(&_app->m_frequency);

	appNameSet(_app, _name);

	return 0;
}

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nCmdShow)
{
	const int ret = kickstart(__argc, __argv);
	
	if (ret != 0)
    {
		__debugbreak();
	}

	return ret;
}

