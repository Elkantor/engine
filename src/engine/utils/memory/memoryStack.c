#pragma once

#include <stdint.h>
#include "../../moduleHelper.c"

#undef module
#define module memoryStack

typedef struct memoryStackStruct
{
    char m_bytesStack[512 * 1024];
    uint32_t m_bytesAllocated;
} memoryStackStruct;

fn(void*, push, memoryStackStruct* _stack, const uint32_t _bytes)
{
    char* address = _stack->m_bytesStack + _stack->m_bytesAllocated;
    _stack->m_bytesAllocated += _bytes;
    return (void*)address;
}
end(push)

fn(void, pop, memoryStackStruct* _stack, void* _value)
{
    void* currentAddressAvailable = _stack->m_bytesStack + _stack->m_bytesAllocated;
    const uintptr_t numberDeallocated = (!!(uintptr_t)_value) * (uintptr_t)currentAddressAvailable - (uintptr_t)_value;
    _stack->m_bytesAllocated -= numberDeallocated;
}
end(pop)

fn(void, swap, memoryStackStruct* restrict * _stackA, memoryStackStruct* restrict * _stackB)
{
    memoryStackStruct* tmp = *_stackA;
    *_stackA = *_stackB;
    *_stackB = tmp;
}
end(swap)

module_export(
    push,
    pop,
    swap
)