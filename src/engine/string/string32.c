#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef struct
{
    union
    {
        char m_data[32];
        const char* m_span;
    };
    uint32_t m_size;
} string32_t;

uint32_t string32CapacityGet(void)
{
    return offsetof(string32_t, m_size) / sizeof(char);
}

string32_t string32Init(const char* _from)
{
    if (_from == NULL)
    {
        return (string32_t){ .m_data[0] = 0, .m_size = 0};
    }
    else
    {
        return (string32_t){ .m_span = _from, .m_size = strnlen_s(_from, 32) };
    }
}

uint64_t string32Hash(const string32_t* _string)
{
    const char* data;
    if (_string->m_data[0] == *_string->m_span)
    {
        data = (char*)_string->m_data;
    }
    else
    {
        data = _string->m_span;
    }

    uint64_t hash = 14695981039346656037UL;
    uint8_t  c = data[0];
    for (size_t i = 0; i < 64; i++) {
        if (i < _string->m_size)
        {
            c = data[i];
        }
        else
        {
            c = 0;
        }
        hash = (hash ^ c) * 1099511628211;
    }
    return hash;
}

#define _h64_H1(s,i,x)  (((x)^s[(i)<sizeof(s)?(i):sizeof(s)-1])*1099511628211UL)
#define _h64_H4(s,i,x)  _h64_H1 (s,i+3, _h64_H1 (s,i+2, _h64_H1 (s,i+1, _h64_H1 (s,i,x))))
#define _h64_H16(s,i,x) _h64_H4 (s,i+12,_h64_H4 (s,i+8, _h64_H4 (s,i+4, _h64_H4 (s,i,x))))
#define _h64_H64(s,i,x) _h64_H16(s,i+48,_h64_H16(s,i+32,_h64_H16(s,i+16,_h64_H16(s,i,x))))
#define string32HashConst(str) ((uint64_t)(_h64_H64(str,0,14695981039346656037UL)))