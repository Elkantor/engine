#pragma once

#include <stdint.h>
#include <windef.h>
#include <stddef.h>

#include "../../../../graphics.c"
#include "../../openGL/glWrapper.c"
#include "../../openGL/renderTarget.c"

typedef struct
{
    HDC m_deviceContext;
	HGLRC m_glContext;
	int m_depthBufferBits;
	int m_framebuffer;
	unsigned m_vertexArray;
	renderTargetImpl_t m_renderTarget;
} graphic_t;

typedef struct graphicsArray
{
    graphic_t m_data[8];
    uint32_t m_size;
} graphicsArray_t;

uint32_t graphicsArrayCapacityGet(void)
{
    return offsetof(graphicsArray_t, m_size) / sizeof(graphic_t); 
}
