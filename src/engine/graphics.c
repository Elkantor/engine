#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct windowData windowData_t;

typedef struct graphicsInternal graphicsInternal_t;
typedef struct graphicsInternalArray graphicsInternalArray_t;

uint32_t graphicsInternalArrayCapacityGet(void);
void graphicsWindowInit(windowData_t* _window);
void graphicsWindowResize(windowData_t* _window, const int _width, const int _height);