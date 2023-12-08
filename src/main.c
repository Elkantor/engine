#include <stdio.h>

#include "engine/backends/system/windows/system.c"
#include "engine/backends/system/windows/display.c"
#include "engine/backends/system/windows/window.c"

// NOTE(Victor): build with mingw on windows:
// gcc src/main.c -g -lgdi32 && ./cv2pdb64.exe a.exe
// devenv a.exe (to open visual studio debugger)

int kickstart(int argc, char **argv)
{
    printf("Inside kickstart 2\n");

    displayDataArray_t displays;
    displayInit(&displays);

    windowOptions_t firstWindowOptions;
	windowOptionsSetDefaults(&firstWindowOptions);

    return 0;
}