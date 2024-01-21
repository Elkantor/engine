#pragma once

#include <stdint.h>
#include "../../../libs/raylib/raylib.h"

void uiBinderF32(float* _value, const wchar_t* _func, void* _windowHandle);
void uiBinderI32(int32_t* _value, const wchar_t* _func, void* _windowHandle);
void uiBinderVector3(Vector3* _value, const wchar_t* _func, void* _windowHandle);