#pragma once

#include <stdint.h>

void convertUint16ToFloat3Array(uint16_t _input, float _output[3])
{
    _output[0] = (float)(_input & 0xFF);
    _output[1] = (float)((_input >> 8) & 0xFF);
    _output[2] = (float)((_input >> 4) & 0xFF);
}

uint16_t convertFloat3ArrayToUint16(const float _input[3])
{
    uint16_t result = 0;

    result |= ((uint16_t)(_input[0])) & 0xFF;
    result |= ((uint16_t)(_input[1]) << 8) & 0xFF00;
    result |= ((uint16_t)(_input[2]) << 4) & 0xF00;

    return result;
}