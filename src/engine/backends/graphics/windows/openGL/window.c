#pragma once

#include <assert.h>
#include <stdint.h>
#include <windows.h>
#include <wingdi.h>
#include "../../openGL/glWrapper.c"
#include "../../../../window.c"
#include "../../../../vertex.c"
#include "../../../../shader.c"
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

	// NOTE(Victor): Glew is initialized if whatever gl function ptr is not null anymore
	const bool openGLInitialized = (glGenVertexArrays != NULL);

	if (openGLInitialized == false)
	{
		glewInit();
	}

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
    
    if (_windowIndex != 0)
    {
        wglShareLists(firstGraphic->m_glContext, graphic->m_glContext);

        wglMakeCurrent(firstWindowHDC, firstGraphic->m_glContext);
        assert(glDebugErrorCheck() == false);

        renderTargetInit(&graphic->m_renderTarget, window->m_width, window->m_height, _depthBufferBits, _stencilBufferBits, 1, RENDER_TARGET_FORMAT_32BIT);
    
        if (openGLInitialized == false)
        {
            wglMakeCurrent(hdc, graphic->m_glContext);
            assert(glDebugErrorCheck() == false);

            vertexArray_t vertexArray;
            vertexArrayInit(&vertexArray);
            vertexArrayAdd(&vertexArray, "pos", VERTEX_DATA_F32_2X);

            vertexBufferInit(&_graphics->m_vertexBuffer, 4, &vertexArray, 0);
            float* vertices = vertexBufferLockAll(&_graphics->m_vertexBuffer);
            {
                vertices[0] = -1.0f;
                vertices[1] = -1.0f;
                vertices[2] = -1.0f;
                vertices[3] = 1.0f;
                vertices[4] = 1.0f;
                vertices[5] = 1.0f;
                vertices[6] = 1.0f;
                vertices[7] = -1.0f;
            }
            vertexBufferUnlockAll(&_graphics->m_vertexBuffer);
            
            indexBufferInit(&_graphics->m_indexBuffer, 6, INDEX_BUFFER_FORMAT_32BIT);
            indexBufferData_t* indices = indexBufferLockAll(&_graphics->m_indexBuffer);
            {
                indices->m_data32Bits[0] = 0;
                indices->m_data32Bits[1] = 1;
                indices->m_data32Bits[2] = 2;
                indices->m_data32Bits[3] = 0;
                indices->m_data32Bits[4] = 2;
                indices->m_data32Bits[5] = 3;
            }
            indexBufferUnlockAll(&_graphics->m_indexBuffer);

            const char* vertexShader = "#version 450\n"
                "in vec2 pos;\n"
                "out vec2 texCoord;\n"
                "void main() {\n"
                    "gl_Position = vec4(pos, 0.5, 1.0);\n"
                    "texCoord = (pos + 1.0) / 2.0;\n"
                "}\n";

            shaderInit(&_graphics->m_vertexShader, vertexShader, strlen(vertexShader), SHADER_TYPE_VERTEX);

            const char* fragmentShader = "#version 450\n"
                "uniform sampler2D tex;\n"
                "in vec2 texCoord;\n"
                "out vec4 frag;\n"
                "void main() {\n"
                    "frag = texture(tex, texCoord);\n"
                "}\n";

            shaderInit(&_graphics->m_fragmentShader, fragmentShader, strlen(fragmentShader), SHADER_TYPE_FRAGMENT);
            // TODO(Victor): finish to create the default vertex and fragment shader for the window

        }

    }

    wglMakeCurrent(hdc, graphic->m_glContext);
    assert(glDebugErrorCheck() == false);
    
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &graphic->m_framebuffer);
    assert(glDebugErrorCheck() == false);

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

    if (wglSwapIntervalEXT != NULL)
    {
        wglSwapIntervalEXT(_windows->m_data[_windowIndex].m_frameBufferOptions.m_verticalSync);
    }

    int maxColorAttachments = 8;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
}