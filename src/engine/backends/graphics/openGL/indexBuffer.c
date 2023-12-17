#pragma once

#include <assert.h>
#include "glWrapper.c"
#include "../../../indexBuffer.c"

typedef union indexBufferData
{
    uint16_t m_data16Bits[64];
    uint32_t m_data32Bits[32];
} indexBufferData_t;

typedef struct indexBuffer
{
	indexBufferData_t m_data;
	uint32_t m_count;

	unsigned m_usage;
	int m_format;
	unsigned m_bufferId;
} indexBuffer_t;

void indexBufferInit(indexBuffer_t* _indexBuffer, const uint32_t _count, indexBufferFormat_t _format)
{
    memset(_indexBuffer, 0, sizeof(*_indexBuffer));

    _indexBuffer->m_count = _count;
    _indexBuffer->m_format = _format;
    _indexBuffer->m_usage = GL_STATIC_DRAW;

    glGenBuffers(1, &_indexBuffer->m_bufferId);
    assert(glDebugErrorCheck() == false);
}

indexBufferData_t* indexBufferLockAll(indexBuffer_t* _indexBuffer)
{
    indexBufferData_t* data = &_indexBuffer->m_data;
    return data;
};

void indexBufferUnlockAll(indexBuffer_t* _indexBuffer)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer->m_bufferId);
    assert(glDebugErrorCheck() == false);

    const GLsizeiptr size = _indexBuffer->m_format == INDEX_BUFFER_FORMAT_16BIT ? _indexBuffer->m_count * sizeof(uint16_t) : _indexBuffer->m_count * sizeof(uint32_t);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, &_indexBuffer->m_data, _indexBuffer->m_usage);
    assert(glDebugErrorCheck() == false);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	assert(glDebugErrorCheck() == false);
}
