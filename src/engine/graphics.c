#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct graphicsArray graphicsArray_t;

static graphicsArray_t* k_graphicsArray = NULL;

void internalGraphicsInit(graphicsArray_t* _graphicsArray);

void graphicsArrayInit(graphicsArray_t* _graphicsArray)
{
    internalGraphicsInit(_graphicsArray);
    k_graphicsArray = _graphicsArray;
}