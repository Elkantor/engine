#pragma once

#include <stdint.h>
#include <windows.h>
#include <stddef.h>
#include <wingdi.h>

#include "../../../graphics.c"
#include "../../../window.c"

typedef struct graphicsInternal
{
    HDC m_deviceContext;
	HGLRC m_glContext;
	int m_depthBufferBits;
	int m_framebuffer;
	unsigned m_vertexArray;
	//struct kinc_g4_render_target renderTarget;
} graphicsInternal_t;

typedef struct graphicsInternalArray
{
    graphicsInternal_t m_data[8];
    uint32_t m_size;
} graphicsInternalArray_t;

uint32_t graphicsInternalArrayCapacityGet(void)
{
    return offsetof(graphicsInternalArray_t, m_size) / sizeof(graphicsInternal_t); 
}

void internalGLContextInit(windowData_t* _window)
{
    _window->m_graphicsWindow.m_depthBufferBits = _window->m_frameBufferOptions.m_depthBits;

    const HDC hdc = GetDC((HWND)_window->m_handle);
    _window->m_graphicsWindow.m_deviceContext = hdc;

    DWORD dwDamageMask;

    PIXELFORMATDESCRIPTOR pfd = 
    {
        .nSize = sizeof(PIXELFORMATDESCRIPTOR),
	    .nVersion = 1,
	    .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
	    .iPixelType = PFD_TYPE_RGBA,
	    .cColorBits = 32,
        .cRedBits = 0,
        .cRedShift = 0,
        .cGreenBits = 0,
        .cGreenShift = 0,
        .cBlueBits = 0,
        .cBlueShift = 0,
        .cAlphaBits = 0,
        .cAlphaShift = 0,
        .cAccumBits = 0,
        .cAccumRedBits = 0,
        .cAccumGreenBits = 0,
        .cAccumBlueBits = 0,
        .cAccumAlphaBits = 0,
        .cDepthBits = (BYTE)_window->m_frameBufferOptions.m_depthBits,
        .cStencilBits = (BYTE)_window->m_frameBufferOptions.m_stencilBits,
        .cAuxBuffers = 0,
        .iLayerType = PFD_MAIN_PLANE,
        .bReserved = 0,
        .dwLayerMask =0,
        .dwVisibleMask = 0,
        .dwDamageMask = 0
    };

    const uint32_t pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);
    HGLRC tempGlContext = wglCreateContext(hdc);
}

void graphicsWindowInit(windowData_t* _window)
{
    internalGLContextInit(_window);

}

void graphicsWindowResize(windowData_t* _window, const int _width, const int _height)
{

}