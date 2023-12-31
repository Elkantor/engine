#pragma once

// Raylib
#include "../libs/raylib/raylib.h"
#include "../libs/raylib/rlgl.h"
#include "../libs/raylib/raymath.h"

// Include global context struct
#include "globalContext.c"

// Include here the plateform specific implementations
#include "engine/backends/system/windows/window.c"
#include "engine/backends/system/windows/display.c"
#include "engine/backends/system/windows/system.c"

// Mctrl
#include <commctrl.h> // needs windows.h defined in system.c
#include <wchar.h>
#include <dwmapi.h>
#include "../libs/mctrl/mctrl.h"
#include "../libs/mctrl/mCtrl/html.h"

