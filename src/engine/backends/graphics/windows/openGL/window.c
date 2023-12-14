#pragma once

#include <windows.h>
#include <wingdi.h>
#include "../../openGL/glew.c"
#include "../../openGL/glDebug.c"
#include "graphics.c"
#include "../../../../window.c"

void internalGLContextInit(HWND _hwnd, graphic_t* _graphicsData, const int _depthBufferBits, const int _stencilBufferBits)
{
    _graphicsData->m_depthBufferBits = _depthBufferBits;

    const HDC hdc = GetDC(_hwnd);
    _graphicsData->m_deviceContext = hdc;

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
    wglMakeCurrent(hdc, tempGlContext);

    glewInit();

    if (wglewIsSupported("WGL_ARB_create_context") == 1)
    {
		int attributes[] = 
        {
            [0] = WGL_CONTEXT_MAJOR_VERSION_ARB,
            [1] = 4,
            [2] = WGL_CONTEXT_MINOR_VERSION_ARB,
            [3] = 2,
            [4] = WGL_CONTEXT_FLAGS_ARB,
            [5] = WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            [6] = WGL_CONTEXT_PROFILE_MASK_ARB,
            [7] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            [8] = 0
        };

         _graphicsData->m_glContext = wglCreateContextAttribsARB(hdc, _graphicsData->m_glContext, attributes);
     
        if (glDebugErrorCheck())
        {
            // NOTE(Victor): Handle the error
            return;
        }
		
        wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempGlContext);
		wglMakeCurrent(hdc, _graphicsData->m_glContext);
		
        if (glDebugErrorCheck())
        {
            // NOTE(Victor): Handle error
            return;
        }
    }

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