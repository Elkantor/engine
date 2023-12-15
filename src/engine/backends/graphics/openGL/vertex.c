#pragma once

#include "../../../vertex.c"

typedef struct vertexBuffer
{
	vertexArray_t m_vertexArray;
    float* m_data;
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
        _vertexBuffer->m_stride
    }
}