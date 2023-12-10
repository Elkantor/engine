#include <stdio.h>

#include "engine/backends/graphics/opengl/graphics.c"
#include "engine/backends/system/windows/system.c"

// NOTE(Victor): build with mingw on windows:
// gcc src/main.c -g -lgdi32 && ./cv2pdb64.exe a.exe
// devenv a.exe (to open visual studio debugger)

// NOTE(Victor): build with zig cc (clang) compiler on windows:
// zig cc src/main.c -g -lgdi32
// devenv a.exe (to open visual studio debugger)

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