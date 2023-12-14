#pragma once

#include <assert.h>
#include <stdint.h>
#include <windows.h>
#include <wingdi.h>
#include "../../openGL/glWrapper.c"
#include "../../../../window.c"
#include "../../../../vertex.c"
#include "graphics.c"

void internalGLContextInit(windowArray_t* _windows, const uint32_t _windowIndex, graphicsArray_t* _graphics, const uint32_t _graphicIndex, const int _depthBufferBits, const int _stencilBufferBits)
{
    graphic_t* graphic = &_graphics->m_data[_graphicIndex];
    graphic->m_depthBufferBits = _depthBufferBits;

    windowData_t* window = &_windows->m_data[_windowIndex];
    const HDC hdc = GetDC(window->m_handle);
    graphic->m_deviceContext = hdc;

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
    assert(glDebugErrorCheck() == false);

    const windowData_t* firstWindow = &_windows->m_data[0];
    const graphic_t* firstGraphic = &_graphics->m_data[0];

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

        graphic->m_glContext = wglCreateContextAttribsARB(hdc, firstGraphic->m_glContext, attributes);
        assert(glDebugErrorCheck() == false);
		
        wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempGlContext);
		
		wglMakeCurrent(hdc, graphic->m_glContext);
        assert(glDebugErrorCheck() == false);
    }
    else
    {
        graphic->m_glContext = tempGlContext;
    }

    const HDC firstWindowHDC = GetDC(firstWindow->m_handle);
    
    if (_graphicIndex != 0 && _windowIndex != 0)
    {
        wglShareLists(firstGraphic->m_glContext, graphic->m_glContext);

        wglMakeCurrent(firstWindowHDC, firstGraphic->m_glContext);
        assert(glDebugErrorCheck() == false);

        renderTargetInit(&graphic->m_renderTarget, window->m_width, window->m_height, _depthBufferBits, _stencilBufferBits, 1, RENDER_TARGET_FORMAT_32BIT);
    
        wglMakeCurrent(hdc, graphic->m_glContext);
        assert(glDebugErrorCheck() == false);

        vertexArray_t vertexArray;
        vertexArrayInit(&vertexArray);
        vertexArrayAdd(&vertexArray, "pos", VERTEX_DATA_F32_2X);
    }

    wglMakeCurrent(hdc, graphic->m_glContext);
    assert(glDebugErrorCheck() == false);
    
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &graphic->m_framebuffer);

	glGenVertexArrays(1, &graphic->m_vertexArray);
    assert(glDebugErrorCheck() == false);

	wglMakeCurrent(firstWindowHDC, firstGraphic->m_glContext);
    assert(glDebugErrorCheck() == false);
    
	glBindVertexArray(firstGraphic->m_vertexArray);
	assert(glDebugErrorCheck() == false);
}

void windowGraphicsInit(windowArray_t* _windows, const uint32_t _windowIndex, graphicsArray_t* _graphics, const uint32_t _graphicIndex)
{
    assert(_windows->m_data[_windowIndex].m_graphicIndex == UINT32_MAX);
    assert(_windows->m_data[_windowIndex].m_handle != NULL);
    
    windowData_t* window = &_windows->m_data[_windowIndex];

    const int depthBits = window->m_frameBufferOptions.m_depthBits;
    const int stencilBits = window->m_frameBufferOptions.m_stencilBits;

    internalGLContextInit(_windows, _windowIndex, _graphics, _graphicIndex, depthBits, stencilBits);

    if (_windows->m_size == 1)
    {
        if (wglSwapIntervalEXT != NULL)
        {
            wglSwapIntervalEXT(_windows->m_data[_windowIndex].m_frameBufferOptions.m_verticalSync);
        }
    }

    int maxColorAttachments = 8;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);

    //const int openGLVersion = glVersionGet();
}