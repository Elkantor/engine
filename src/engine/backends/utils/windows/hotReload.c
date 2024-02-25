#pragma once

#include <assert.h>
#include <stdio.h>
#include <string.h>
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "../../../utils/hotReload/hotReload.c"

void* hotReloadLibNew(const string32_t* _file)
{
	const char* file = string32DataGet(_file);

	SYSTEMTIME lt = { 0 };
	GetLocalTime(&lt);
	char hour[32] = { 0 };
	snprintf(hour, 32, "%04d%02d%02d%02d%02d%02d", lt.wYear, lt.wMonth, lt.wDay, lt.wHour, lt.wMinute, lt.wSecond);

	char output[48] = { 0 };
	snprintf(output, 48, "tmp_%s.dll", hour);

	int result = 0;
	{
		char buff[512] = { 0 };
		snprintf(buff, 512, "%s%s ", "zig cc -g -Wall -Werror -lwinmm -lgdi32 -lopengl32 -lraylib -L./libs/raylib -DTHREADED -shared -o ", output);
		strncat_s(buff, 512, file, _file->m_size);
		result = system(buff);
	}

	if (result != 0)
	{
		printf("[ERROR]: HotReload - Compilation failed\n");
		return NULL;
	}

	const HINSTANCE hDLL = LoadLibraryA(output);

	if (hDLL == NULL)
	{
		printf("[ERROR]: HotReload - Can't load dll\n");
		return NULL;
	}

	return hDLL;
}

void hotReloadLibDelete(const void* _lib)
{
	if (_lib != NULL)
	{
		FreeLibrary((HINSTANCE)_lib);
	}
}

void* hotReloadSymbolGet(const void* _lib, const string32_t* _symbol)
{
	assert(_lib != NULL);

	void* func = (void*)GetProcAddress((HINSTANCE)_lib, string32DataGet(_symbol));

	if (func == NULL)
	{
		printf("[ERROR]: HotReload - Can't retrieve symbol %s\n", string32DataGet(_symbol));
		return NULL;
	}

	return func;
}