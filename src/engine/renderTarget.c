#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef struct renderTargetImpl renderTargetImpl_t;

typedef enum
{
	RENDER_TARGET_FORMAT_32BIT,
	RENDER_TARGET_FORMAT_64BIT_FLOAT,
	RENDER_TARGET_FORMAT_32BIT_RED_FLOAT,
	RENDER_TARGET_FORMAT_128BIT_FLOAT,
	RENDER_TARGET_FORMAT_16BIT_DEPTH,
	RENDER_TARGET_FORMAT_8BIT_RED,
	RENDER_TARGET_FORMAT_16BIT_RED_FLOAT
} renderTargetFormat_t;

typedef struct
{
    int m_width;
    int m_height;
    int m_texWidth;
    int m_texHeight;
} renderTarget_t;

typedef struct
{
    renderTarget_t m_data[8];
    uint32_t m_size;
} renderTargetArray_t;

uint32_t renderTargetArrayCapacityGet(void)
{
    return offsetof(renderTargetArray_t, m_size) / sizeof(renderTarget_t);
}

void renderTargetInit(renderTargetImpl_t* _renderTarget, const int _width, const int _height, const int _depthBits, const int _stencilBits, const int _samplesPerPixel, const renderTargetFormat_t _format);