#pragma once

#include <string.h>
#include "../../../shader.c"

typedef struct shader
{
	const char* m_source;
	size_t m_length;
	unsigned m_glId;
} shader_t;

void shaderInit(shader_t* _shader, const void* _data, const size_t _length, const shaderType_t _type)
{
    memset(_shader, 0, sizeof(*_shader));

    _shader->m_length = _length;
    _shader->m_source = _data;
}