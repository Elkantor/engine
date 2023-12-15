#include <stdio.h>

#include "engine/utils/memory/memoryStack.c"
#include "appconfig.c"

// NOTE(Victor): build with mingw on windows:
// gcc src/main.c -g -lgdi32 -lopengl32 -lglu32 && ./cv2pdb64.exe a.exe
// devenv a.exe (to open visual studio debugger)

// NOTE(Victor): build with zig cc (clang) compiler on windows:
// zig cc src/main.c -g -lgdi32 -lopengl32 -lglu32
// devenv a.exe (to open visual studio debugger)

void windowResize(windowData_t* _window, const int _width, const int _height)
{
    printf("resized\n");
}

void appUpdate(app_t* _app)
{
}

int appKickstart(int argc, char **argv)
{
    printf("Inside kickstart 2\n");

    static app_t app;
    appInit(&app, "My App");

    static windowArray_t windows;
    windowArrayInit(&app, &windows);
    
	static graphicsArray_t graphics;
	graphicsArrayInit(&graphics);

    windowInit(&windows, 0, 1024, 768, "First Window");

    windowGraphicsInit(&windows, 0, &graphics, 0);

    const uint64_t stackSize = appStackSizeGet();
    appStart(&app);


    return 0;
}