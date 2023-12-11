#pragma once

#include <Windows.h>
#include <DbgHelp.h>
#include <string.h>
#include <winnt.h>

#include "window.c"
#include "display.c"
#include "../../../window.c"
#include "../../../graphics.c"
#include "../../../system.c"

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
	const uint32_t capacity = appNameCapacityGet();
	strncpy(_app->m_name, _name, capacity);
}

bool internalMessagesHandle(void)
{
	MSG message;

	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return true;
}

bool internalFrame(app_t* _app)
{
	appUpdate(_app);
	internalMessagesHandle();

	return _app->m_running;
}

void appStop(app_t* _app)
{
    displayRestore(&_app->m_displays);
}

void appStart(app_t* _app)
{
	_app->m_running = true;

	while (internalFrame(_app)) 
	{
	}

	appStop(_app);
}

void appInit(app_t* _app, const char* _name, const int _width, const int _height)
{
	crashHandlerInit();

	memset(_app, 0, sizeof(*_app));

	appPtr = _app;

	QueryPerformanceCounter((LARGE_INTEGER*) &_app->m_startCount);
	QueryPerformanceFrequency((LARGE_INTEGER*)&_app->m_frequency);

	appNameSet(_app, _name);

	// Init the displays
	{
		_app->m_displays.m_size = 0;
    	displayInit(&_app->m_displays);
	}

	windowData_t* windowData = &_app->m_windows.m_data[_app->m_windows.m_size];

	// Init the windows
	{
		windowDataDefaultSet(windowData);
	
		windowData->m_width = _width;
		windowData->m_height = _height;
		windowData->m_title = _name;
		windowData->m_displayIndex = displayPrimaryGet(&_app->m_displays);
	}

	windowCreate(&_app->m_displays, windowData, false);
	_app->m_windows.m_size += 1;

	graphicsWindowInit(windowData);

	windowShow(windowData);
}

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nCmdShow)
{
	const int ret = appKickstart(__argc, __argv);
	
	if (ret != 0)
    {
		__debugbreak();
	}

	return ret;
}

