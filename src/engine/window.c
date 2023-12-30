#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "system.c"
#include "display.c"
#include "vertex.c"
#include "graphics.c"

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
	WINDOW_MODE_FULLSCREEN_EXCLUSIVE, // Only relevant for Windows
	WINDOW_MODE_TOOL, // Only relevant for windows
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

typedef struct windowData
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
	uint32_t m_graphicIndex;

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
	_win->m_graphicIndex = UINT32_MAX;
}

typedef struct
{
    windowData_t m_data[8]; // 8 windows max for an app
    uint32_t m_size;

	app_t* m_app; // Windows are attached to an app, without the app, windows make no sense
} windowArray_t;

uint32_t windowArrayCapacityGet(void)
{
    return offsetof(windowArray_t, m_size) / sizeof(windowData_t);
}

static windowArray_t* k_windows = NULL;

void windowArrayInit(app_t* _app, windowArray_t* _windows)
{
	memset(_windows, 0, sizeof(windowArray_t));

	for (uint32_t i = 0; i < windowArrayCapacityGet(); ++i)
	{
		_windows->m_data[i].m_graphicIndex = UINT32_MAX;
	}

	_windows->m_app = _app;
	k_windows = _windows;
}

void windowInit(windowArray_t* _windows, const uint32_t _index, const int _width, const int _height, const char* _name, const windowMode_t _mode, void* _ownerHandle);
void windowCreate(displayDataArray_t* _displays, windowData_t* _windowData, const bool _appInitialized, void* _ownerHandle);
void windowShow(windowData_t* _window);
void windowHide(windowData_t* _window);
void windowDestroy(windowArray_t* _windows, void* _handle);
uint32_t windowIndexGet(windowArray_t* _windows, void* _handle);

// Graphics implementations
void windowGraphicsInit(windowArray_t* _windows, const uint32_t _windowIndex, graphicsArray_t* _graphics, const uint32_t _graphicIndex);

// User implemenatations
void windowResize(windowData_t* _window, const int _width, const int _height);
void windowPaint(windowData_t* _window);