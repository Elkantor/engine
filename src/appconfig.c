#pragma once

#include <stdint.h>
#include <stdio.h>


// Raylib
#include "../libs/raylib/raylib.h"
#include "../libs/raylib/rlgl.h"
#include "../libs/raylib/raymath.h"

#include "engine/moduleHelper.c"
#include "engine/utils/types.c"
#include "engine/utils/memory/memoryStack.c"
#include "engine/utils/convert.c"
#include "engine/gui/gui.c"


// Include here the plateform specific implementations
#include "engine/backends/system/windows/window.c"
#include "engine/backends/system/windows/display.c"
#include "engine/backends/system/windows/system.c"
#include "engine/backends/utils/windows/hotReload.c"

// Mctrl
void __stdio_common_vswprintf() {}
#include <guiddef.h>
#include <commctrl.h> // needs windows.h defined in system.c
#include <wchar.h>
#include <dwmapi.h>
//#define COM_NO_WINDOWS_H
//#include "../libs/mctrl/wincodec.h"
#include "../libs/mctrl/mctrl.h"
#include "../libs/mctrl/mCtrl/html.h"

// UI
#include "engine/backends/ui/windows/uiBinder.c"



