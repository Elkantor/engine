#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    char m_data[32];
    uint32_t m_size;
} string32_t;

uint32_t string32CapacityGet(void)
{
    return offsetof(string32_t, m_size) / sizeof(char);
}