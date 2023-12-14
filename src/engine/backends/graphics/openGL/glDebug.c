#pragma once

#include <stdbool.h>

typedef unsigned int GLenum;
#define GL_NO_ERROR 0

GLenum glGetError (void);


bool glDebugErrorCheck(void)
{
    const GLenum code = glGetError();
    return code != GL_NO_ERROR;
}