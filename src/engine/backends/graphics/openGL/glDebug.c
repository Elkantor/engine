#pragma once

#include <stdbool.h>
#include "glew.c"

typedef unsigned int GLenum;

#ifndef GL_R16F_EXT
    #define GL_R16F_EXT 0x822D
#endif

#ifndef GL_R32F_EXT
    #define GL_R32F_EXT 0x822E
#endif

bool glDebugErrorCheck(void)
{
    const GLenum code = glGetError();
    return code != 0;
}