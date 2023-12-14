#pragma once

#include <stdbool.h>
#include "glew.c"

typedef unsigned int GLenum;
#define GL_NO_ERROR 0

bool glDebugErrorCheck(void)
{
    const GLenum code = glGetError();
    return code != GL_NO_ERROR;
}