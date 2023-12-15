#pragma once

#include <assert.h>
#include "../../../vertex.c"
#include "glWrapper.c"

typedef struct vertexBuffer
{
	vertexArray_t m_vertexArray;
    float m_data[64];
	int m_count;
	int m_stride;
	unsigned m_bufferId;
	int m_sectionStart;
	int m_sectionSize;
	int m_instanceDataStepRate;
} vertexBuffer_t;

void vertexBufferInit(vertexBuffer_t* _vertexBuffer, const uint32_t _count, const vertexArray_t* _vertexArray, const int _instanceDataStepRate)
{
    memset(_vertexBuffer, 0, sizeof(*_vertexBuffer));

    _vertexBuffer->m_count = _count;
    _vertexBuffer->m_instanceDataStepRate = _instanceDataStepRate;

    for (uint32_t i = 0; i < _vertexArray->m_size; ++i)
    {
        _vertexBuffer->m_stride += vertexDataTypeSizeGet(_vertexArray->m_data[i].m_dataType);
    }

    _vertexBuffer->m_vertexArray = *_vertexArray;

    glGenBuffers(1, &_vertexBuffer->m_bufferId);
	assert(glDebugErrorCheck() == false);

    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer->m_bufferId);
	assert(glDebugErrorCheck() == false);

	glBufferData(GL_ARRAY_BUFFER, _vertexBuffer->m_stride * _vertexBuffer->m_count, NULL, GL_STATIC_DRAW);
	assert(glDebugErrorCheck() == false);

    const size_t dataSize = _count * _vertexBuffer->m_stride;
    assert(dataSize < 64);
}

float* vertexBufferLockAll(vertexBuffer_t* _vertexBuffer)
{
    _vertexBuffer->m_sectionStart = 0;
	_vertexBuffer->m_sectionSize = _vertexBuffer->m_count * _vertexBuffer->m_stride;
	return _vertexBuffer->m_data;
}

void vertexBufferUnlockAll(vertexBuffer_t* _vertexBuffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer->m_bufferId);
	assert(glDebugErrorCheck() == false);

	uint8_t* u8data = (uint8_t*)_vertexBuffer->m_data;
	glBufferSubData(GL_ARRAY_BUFFER, _vertexBuffer->m_sectionStart, _vertexBuffer->m_sectionSize, u8data + _vertexBuffer->m_sectionStart);
	assert(glDebugErrorCheck() == false);
}