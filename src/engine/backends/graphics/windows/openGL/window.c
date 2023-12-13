#pragma once

#include <windows.h>
#include "graphics.c"
#include "../../../../window.c"

void internalGLContextInit(HWND _hwnd, graphic_t* _graphicsData, const int _depthBufferBits, const int _stencilBufferBits)
{
    _graphicsData->m_depthBufferBits = _depthBufferBits;

    const HDC hdc = GetDC(_hwnd);
    _graphicsData->m_deviceContext = hdc;

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
        .cDepthBits = (BYTE)_depthBufferBits,
        .cStencilBits = (BYTE)_stencilBufferBits,
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

void windowGraphicsInit(windowArray_t* _windows, const uint32_t _windowIndex, graphicsArray_t* _graphics, const uint32_t _graphicIndex)
{
    windowData_t* window = &_windows->m_data[_windowIndex];

    const int depthBits = window->m_frameBufferOptions.m_depthBits;
    const int stencilBits = window->m_frameBufferOptions.m_stencilBits;
    const HWND handle = window->m_handle;
    graphic_t* graphic = &_graphics->m_data[_graphicIndex];

    internalGLContextInit(handle, graphic, depthBits, stencilBits);
}