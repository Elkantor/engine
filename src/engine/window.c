#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "display.c"

typedef enum
{
    WINDOW_FEATURE_RESIZEABLE = 1,
    WINDOW_FEATURE_MINIMIZABLE = 2,
    WINDOW_FEATURE_MAXIMIZABLE = 4,
    WINDOW_FEATURE_BORDERLESS = 8,
	WINDOW_FEATURE_ON_TOP = 16,
} windowFeature_t;

typedef enum
{
	WINDOW_MODE_WINDOW,
	WINDOW_MODE_FULLSCREEN,
	WINDOW_MODE_FULLSCREEN_EXCLUSIVE // Only relevant for Windows
} windowMode_t;

typedef struct
{
	int m_frequency;
	int m_colorBits;
	int m_depthBits;
	int m_stencilBits;
	int m_samplesPerPixel;
	bool m_verticalSync;
} framebufferOptions_t;

void framebufferOptionsDefaultSet(framebufferOptions_t* _frame)
{
	_frame->m_frequency = 60;
	_frame->m_verticalSync = true;
	_frame->m_colorBits = 32;
	_frame->m_depthBits = 16;
	_frame->m_stencilBits = 8;
	_frame->m_samplesPerPixel = 1;
}

typedef struct 
{
	framebufferOptions_t m_frameBufferOptions;
	void* m_handle;
	const char* m_title;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_displayIndex;
	int m_windowFeatures;
	windowMode_t m_mode;
	bool m_visible;
} windowData_t;

void windowDataDefaultSet(windowData_t* _win)
{
	framebufferOptionsDefaultSet(&_win->m_frameBufferOptions);
	_win->m_handle = NULL;
	_win->m_title = 0;
	_win->m_x = -1;
	_win->m_y = -1;
	_win->m_width = 800;
	_win->m_height = 600;
	_win->m_displayIndex = 0;
	_win->m_windowFeatures = WINDOW_FEATURE_RESIZEABLE | WINDOW_FEATURE_MINIMIZABLE | WINDOW_FEATURE_MAXIMIZABLE;
	_win->m_mode = WINDOW_MODE_WINDOW;
	_win->m_visible = true;
}

typedef struct
{
    windowData_t m_data[8]; // 8 windows max for an app
    uint32_t m_size;
} windowArray_t;

uint32_t windowArrayCapacityGet(void)
{
    return offsetof(windowArray_t, m_size) / sizeof(windowData_t);
}

void windowCreate(displayDataArray_t* _displays, windowData_t* _windowData, const bool _appInitialized);
void windowShow(windowData_t* _window);
void windowDestroy(void* _handle);
uint32_t windowIndexGet(void* _handle);