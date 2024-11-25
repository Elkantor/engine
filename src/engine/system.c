#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdint.h>

#include "window.c"
#include "display.c"
#include "string/wString256.c"

typedef struct memoryStackStruct memoryStackStruct;

typedef struct
{
    i32 m_data[256];
    u32 m_size;
} keysPressedArray_t;

uint32_t keysPressedArrayCapacityGet(void)
{
    return offsetof(keysPressedArray_t, m_size) / sizeof(int);
}

typedef struct app
{
	char m_name[256];
	displayDataArray_t m_displays;
    keysPressedArray_t m_keysPressed;
	i64 m_frequency;
	i64 m_startCount;
	bool m_running;
} app_t;

uint32_t appNameCapacityGet(void)
{
	return offsetof(app_t, m_keysPressed) / sizeof(char);
}


int appKickstart(int _argc, char** _argv);
void appUpdate(app_t* _app, memoryStackStruct* restrict _previousStack, memoryStackStruct* _currentStack);
void appStart(app_t* _app, memoryStackStruct* restrict _previousStack, memoryStackStruct* restrict _currentStack);
void appStop(app_t* _app);
void appInit(app_t* _app, const char* _name);
void appPathAbsoluteGet(wString256_t* _outPath);
void appClipboardCopy(const wchar_t* _text, const size_t _len);

uint64_t appStackSizeGet(void);