#include <stdio.h>

#include "engine/backends/windowsOpengl.c"

// NOTE(Victor): build with mingw on windows:
// gcc src/main.c -g -lgdi32 -lopengl32 -lglu32 && ./cv2pdb64.exe a.exe
// devenv a.exe (to open visual studio debugger)

// NOTE(Victor): build with zig cc (clang) compiler on windows:
// zig cc src/main.c -g -lgdi32 -lopengl32 -lglu32
// devenv a.exe (to open visual studio debugger)

void windowResize(windowData_t* _window, const int _width, const int _height)
{
    int test = 0;
}

void appUpdate(app_t* _app)
{
}

int appKickstart(int argc, char **argv)
{
    printf("Inside kickstart 2\n");

    static app_t app;
    appInit(&app, "MultiWindow", 1024, 768);

    // Init graphic pipeline
    // {
    //     pipeline_t pipeline;
    //     pipelineInit
    // }

    appStart(&app);

    return 0;
}