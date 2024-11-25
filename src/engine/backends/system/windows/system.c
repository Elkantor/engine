#pragma once

#include <Windows.h>
#include <DbgHelp.h>
#include <string.h>
#include <winnt.h>

#include "window.c"
#include "display.c"
#include "../../../window.c"
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
		if (IsDialogMessage(k_windows->m_data[0].m_handle, &message))
		{
			continue;
		}

		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return true;
}

bool internalFrame(app_t* _app, memoryStackStruct* restrict _previousFrame, memoryStackStruct* _currentFrame)
{
	appUpdate(_app, _previousFrame, _currentFrame);
	internalMessagesHandle();

	return _app->m_running;
}

void appStop(app_t* _app)
{
    displayRestore(&_app->m_displays);
	
	_app->m_running = false;
}

void appStart(app_t* _app, memoryStackStruct* restrict _previousFrame, memoryStackStruct* restrict _currentFrame)
{
	_app->m_running = true;

	while (internalFrame(_app, _previousFrame, _currentFrame)) 
	{
	}

	appStop(_app);
}

void appInit(app_t* _app, const char* _name)
{
	crashHandlerInit();

	memset(_app, 0, sizeof(*_app));

	QueryPerformanceCounter((LARGE_INTEGER*) &_app->m_startCount);
	QueryPerformanceFrequency((LARGE_INTEGER*)&_app->m_frequency);

	appNameSet(_app, _name);

	// Init the displays
	{
		_app->m_displays.m_size = 0;
    	displayInit(&_app->m_displays);
	}
}

u64 appStackSizeGet(void)
{
	u64 low, high;
	GetCurrentThreadStackLimits(&low, &high);
	const u64 remaining = (u64)(&low) - low;

	return remaining;
}

void appPathAbsoluteGet(wString256_t* _outPath)
{
	const HMODULE hModule = GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		GetModuleFileNameW(NULL, _outPath->m_data, wString256CapacityGet());

		size_t len = wcslen(_outPath->m_data);

		while (len > 0)
		{
			const wchar_t current = _outPath->m_data[len - 1];

			if (current != L'\\')
			{
				len -= 1;
			}
			else
			{
				_outPath->m_data[len - 1] = L'\0';
				_outPath->m_size = (u32)(len - 1);
				break;
			}
		}
	}
}

void appClipboardCopy(const wchar_t* _text, const size_t _len)
{
	if (OpenClipboard(NULL) == false) return;

	EmptyClipboard();

	// Allouer de la m�moire pour la cha�ne
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, sizeof(*_text) * (_len + 1));
	if (hMem == NULL) { CloseClipboard(); return; }

	wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
	wcsncpy_s(pMem, (_len + 1) * sizeof(*_text), _text, _len + 1);

	SetClipboardData(CF_UNICODETEXT, hMem);

	CloseClipboard();

	GlobalUnlock(hMem);
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

