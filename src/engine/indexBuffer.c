#pragma once

#include <stdint.h>

typedef enum
{
    INDEX_BUFFER_FORMAT_16BIT = 0,
    INDEX_BUFFER_FORMAT_32BIT,
} indexBufferFormat_t;

typedef union indexBufferData indexBufferData_t;
typedef struct indexBuffer indexBuffer_t;

void indexBufferInit(indexBuffer_t* _indexBuffer, const uint32_t _count, indexBufferFormat_t _format);
indexBufferData_t* indexBufferLockAll(indexBuffer_t* _indexBuffer);
void indexBufferUnlockAll(indexBuffer_t* _indexBuffer);
uint32_t indexBufferStrideGet(indexBuffer_t* _indexBuffer);