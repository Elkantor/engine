#pragma once

#include <stdint.h>

/******************************************** VARIABLES ****************************************************/
static char internalMemoryStackBytesStack[512 * 1024];
static uint32_t internalMemoryStackBytesAllocated = 0;


/******************************************** PROCEDURES ***************************************************/
void* memoryStackAlloc(const uint32_t _bytes)
{
    char* pointedAddress = internalMemoryStackBytesStack + internalMemoryStackBytesAllocated;
    internalMemoryStackBytesAllocated += _bytes;
    return (void*) pointedAddress;
}

void memoryStackDealloc(void* _value)
{
    void* currentAddressAvailable = internalMemoryStackBytesStack + internalMemoryStackBytesAllocated;
    const uintptr_t numberDeallocated = (!!(uintptr_t)_value) * (uintptr_t)currentAddressAvailable - (uintptr_t)_value;
    internalMemoryStackBytesAllocated -= numberDeallocated;
}