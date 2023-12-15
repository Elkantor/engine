#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef struct vertexBuffer vertexBuffer_t;

typedef enum
{
    VERTEX_DATA_NONE = 0,
	VERTEX_DATA_F32_1X = 1,
	VERTEX_DATA_F32_2X = 2,
	VERTEX_DATA_F32_3X = 3,
	VERTEX_DATA_F32_4X = 4,
	VERTEX_DATA_F32_4X4 = 5,
	VERTEX_DATA_I8_1X = 6,
	VERTEX_DATA_U8_1X = 7,
	VERTEX_DATA_I8_1X_NORMALIZED = 8,
	VERTEX_DATA_U8_1X_NORMALIZED = 9,
	VERTEX_DATA_I8_2X = 10,
	VERTEX_DATA_U8_2X = 11,
	VERTEX_DATA_I8_2X_NORMALIZED = 12,
	VERTEX_DATA_U8_2X_NORMALIZED = 13,
	VERTEX_DATA_I8_4X = 14,
	VERTEX_DATA_U8_4X = 15,
	VERTEX_DATA_I8_4X_NORMALIZED = 16,
	VERTEX_DATA_U8_4X_NORMALIZED = 17,
	VERTEX_DATA_I16_1X = 18,
	VERTEX_DATA_U16_1X = 19,
	VERTEX_DATA_I16_1X_NORMALIZED = 20,
	VERTEX_DATA_U16_1X_NORMALIZED = 21,
	VERTEX_DATA_I16_2X = 22,
	VERTEX_DATA_U16_2X = 23,
	VERTEX_DATA_I16_2X_NORMALIZED = 24,
	VERTEX_DATA_U16_2X_NORMALIZED = 25,
	VERTEX_DATA_I16_4X = 26,
	VERTEX_DATA_U16_4X = 27,
	VERTEX_DATA_I16_4X_NORMALIZED = 28,
	VERTEX_DATA_U16_4X_NORMALIZED = 29,
	VERTEX_DATA_I32_1X = 30,
	VERTEX_DATA_U32_1X = 31,
	VERTEX_DATA_I32_2X = 32,
	VERTEX_DATA_U32_2X = 33,
	VERTEX_DATA_I32_3X = 34,
	VERTEX_DATA_U32_3X = 35,
	VERTEX_DATA_I32_4X = 36,
	VERTEX_DATA_U32_4X = 37,
} vertexDataType_t;

uint32_t vertexDataTypeSizeGet(vertexDataType_t _vertexDataType)
{
	const uint32_t vertexDataTypeSizes[] =
	{
		[VERTEX_DATA_NONE] = 0,
		[VERTEX_DATA_F32_1X] = 4 * 1,
		[VERTEX_DATA_F32_2X] = 4 * 2,
		[VERTEX_DATA_F32_3X] = 4 * 3,
		[VERTEX_DATA_F32_4X] = 4 * 4,
		[VERTEX_DATA_F32_4X4] = 4 * 4 * 4,
		[VERTEX_DATA_I8_1X] = 1,
		[VERTEX_DATA_U8_1X] = 1,
		[VERTEX_DATA_I8_1X_NORMALIZED] = 1,
		[VERTEX_DATA_U8_1X_NORMALIZED] = 1,
		[VERTEX_DATA_I8_2X] = 2,
		[VERTEX_DATA_U8_2X] = 2,
		[VERTEX_DATA_I8_2X_NORMALIZED] = 2,
		[VERTEX_DATA_U8_2X_NORMALIZED] = 2,
		[VERTEX_DATA_I8_4X] = 4,
		[VERTEX_DATA_U8_4X] = 4,
		[VERTEX_DATA_I8_4X_NORMALIZED] = 4,
		[VERTEX_DATA_U8_4X_NORMALIZED] = 4,
		[VERTEX_DATA_I16_1X] = 2,
		[VERTEX_DATA_U16_1X] = 2,
		[VERTEX_DATA_I16_1X_NORMALIZED] = 2,
		[VERTEX_DATA_U16_1X_NORMALIZED] = 2,
		[VERTEX_DATA_I16_2X] = 2 * 2,
		[VERTEX_DATA_U16_2X] = 2 * 2,
		[VERTEX_DATA_I16_2X_NORMALIZED] = 2 * 2,
		[VERTEX_DATA_U16_2X_NORMALIZED] = 2 * 2,
		[VERTEX_DATA_I16_4X] = 2 * 4,
		[VERTEX_DATA_U16_4X] = 2 * 4,
		[VERTEX_DATA_I16_4X_NORMALIZED] = 2 * 4,
		[VERTEX_DATA_U16_4X_NORMALIZED] = 2 * 4,
		[VERTEX_DATA_I32_1X] = 4,
		[VERTEX_DATA_U32_1X] = 4,
		[VERTEX_DATA_I32_2X] = 4 * 2,
		[VERTEX_DATA_U32_2X] = 4 * 2,
		[VERTEX_DATA_I32_3X] = 4 * 3,
		[VERTEX_DATA_U32_3X] = 4 * 3,
		[VERTEX_DATA_I32_4X] = 4 * 4,
		[VERTEX_DATA_U32_4X] = 4 * 4,
	};

	return vertexDataTypeSizes[_vertexDataType];	
}

typedef struct
{
    const char* m_name;
    vertexDataType_t m_dataType;
} vertex_t;

typedef struct
{
    vertex_t m_data[16];
    uint32_t m_size;

    bool m_instanced;
} vertexArray_t;

uint32_t vertexArrayCapacityGet(void)
{
    return offsetof(vertexArray_t, m_size) / sizeof(vertex_t);
}

void vertexArrayInit(vertexArray_t* _vertexArray)
{
    memset(_vertexArray, 0, sizeof(*_vertexArray));
}

void vertexInit(vertex_t* _vertex, const char* _name, const vertexDataType_t _dataType) {
	_vertex->m_name = _name;
	_vertex->m_dataType = _dataType;
}

void vertexArrayAdd(vertexArray_t* _vertexArray, const char* _name, const vertexDataType_t _dataType)
{
	vertexInit(&_vertexArray->m_data[_vertexArray->m_size++], _name, _dataType);
}

void vertexBufferInit(vertexBuffer_t* _vertexBuffer, const uint32_t _count, const vertexArray_t* _vertexArray, const int _instanceDataStepRate);