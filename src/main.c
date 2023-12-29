#include <stdio.h>

#include "engine/utils/memory/memoryStack.c"
#include "appconfig.c"

// NOTE(Victor): build with mingw on windows:
// gcc src/main.c -g -lDwmapi -lgdi32 -lopengl32 -lglu32 && ./cv2pdb64.exe a.exe
// devenv a.exe (to open visual studio debugger)

// NOTE(Victor): build with zig cc (clang) compiler on windows:
// zig cc src/main.c -g -lDwmapi -lgdi32 -lopengl32 -lglu32
// devenv a.exe (to open visual studio debugger)

void windowResize(windowData_t* _window, const int _width, const int _height)
{
    printf("resized %d, %d\n", _width, _height);
}

void appUpdate(app_t* _app)
{
}

int appKickstart(int argc, char **argv)
{
    static app_t app;
    appInit(&app, "My App");

    static windowArray_t windows;
    windowArrayInit(&app, &windows);

    windowInit(&windows, 0, 1024, 768, "First Window");
    windowInit(&windows, 0, 1024, 768, "Second Window");

    const uint64_t stackSize = appStackSizeGet();
    printf("App stack size left: %zu\n", stackSize);
    appStart(&app);


    return 0;
}