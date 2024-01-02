#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    wchar_t m_data[256];
    uint32_t m_size;
} wString256_t;

uint32_t wString256CapacityGet(void)
{
    return offsetof(wString256_t, m_size) / sizeof(wchar_t);
}