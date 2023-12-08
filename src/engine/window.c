#pragma once

#include <stdbool.h>

typedef enum
{
    WINDOW_FEATURE_RESIZEABLE = 1,
    WINDOW_FEATURE_MINIMIZABLE = 2,
    WINDOW_FEATURE_MAXIMIZABLE = 4,
    WINDOW_FEATURE_BORDERLESS = 8,
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


typedef struct 
{
	const char* m_title;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_displayIndex;
	int m_windowFeatures;
	windowMode_t m_mode;
	bool m_visible;
} windowOptions_t;