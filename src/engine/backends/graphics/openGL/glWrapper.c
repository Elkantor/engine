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

int glVersionGet(void)
{
    int major = -1;
    int minor = -1;

	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	if (major < 0 || minor < 0)
    {
		const GLubyte* version = glGetString(GL_VERSION);
		
        if (version != NULL)
        {
			major = version[0] - '0';
		}
		else
        {
			major = 2;
		}

		minor = 0;
	}

	const int glVersion = major * 100 + minor * 10;
    return glVersion;
}