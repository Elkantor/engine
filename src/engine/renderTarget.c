#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef struct
{
    int m_width;
    int m_height;
    int m_texWidth;
    int m_texHeight;
} renderTarget_t;

typedef struct
{
    renderTarget_t m_data[8];
    uint32_t m_size;
} renderTargetArray_t;

uint32_t renderTargetArrayCapacityGet(void)
{
    return offsetof(renderTargetArray_t, m_size) / sizeof(renderTarget_t);
}