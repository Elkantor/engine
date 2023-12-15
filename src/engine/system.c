#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdint.h>

#include "window.c"
#include "display.c"

typedef struct
{
    int m_data[256];
    uint32_t m_size;
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
	int64_t m_frequency;
	int64_t m_startCount;
	bool m_running;
} app_t;

uint32_t appNameCapacityGet(void)
{
	return offsetof(app_t, m_keysPressed) / sizeof(char);
}


int appKickstart(int _argc, char** _argv);
void appUpdate(app_t* _app);
void appStart(app_t* _app);
void appStop(app_t* _app);
void appInit(app_t* _app, const char* _name);

uint64_t appStackSizeGet(void);