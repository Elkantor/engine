#include <stdio.h>

#include "engine/utils/memory/memoryStack.c"
#include "appconfig.c"

// NOTE(Victor): build with mingw on windows:
// gcc src/main.c -g -Wall -Werror -lDwmapi -lwinmm -lgdi32 -lopengl32 -lraylib -L./libs/raylib && ./cv2pdb64.exe a.exe
// devenv a.exe (to open visual studio debugger)

// NOTE(Victor): build with zig cc (clang) compiler on windows:
// zig cc src/main.c -g -Wall -Werror -lDwmapi -lwinmm -lgdi32 -lopengl32 -lraylib -L./libs/raylib
// devenv a.exe (to open visual studio debugger)

void windowResize(windowData_t* _window, const int _width, const int _height)
{
    printf("resized %d, %d\n", _width, _height);
}


void appUpdate(app_t* _app)
{
    while (!WindowShouldClose() && _app->m_running) {
        BeginDrawing();
        ClearBackground((Color) { 245, 245, 245, 255 });
        DrawRectangle(0, 0, 900, 900, (Color) { 0, 255, 0, 255 });
        DrawFPS(100, 100);
        EndDrawing();
    }
}

int appKickstart(int argc, char **argv)
{
    static app_t app;
    appInit(&app, "My App");
    
    static windowArray_t windows;
    windowArrayInit(&app, &windows);
    windowInit(&windows, 0, 1024, 768, "First Window");

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_BORDERLESS_WINDOWED_MODE | FLAG_WINDOW_UNDECORATED);
    InitWindow(800, 600, "Raylib Window");

    // Make raylib window as child
    {
        SetParent(GetWindowHandle(), windows.m_data[0].m_handle);
        const LONG nNewStyle = (GetWindowLong(GetWindowHandle(), GWL_STYLE) & ~WS_POPUP) | WS_CHILDWINDOW;
        SetWindowLong(GetWindowHandle(), GWL_STYLE, nNewStyle);
        const ULONG_PTR cNewStyle = GetClassLongPtr(GetWindowHandle(), GCL_STYLE) | CS_DBLCLKS;
        SetClassLongPtr(GetWindowHandle(), GCL_STYLE, cNewStyle);
        SetWindowPos(GetWindowHandle(), HWND_TOP, 0, 0, 800, 600, SWP_DRAWFRAME);
    }

    const uint64_t stackSize = appStackSizeGet();
    printf("App stack size left: %zu\n", stackSize);

    appStart(&app);


    // Fermer la fenêtre raylib
    RL_CloseWindow();

    return 0;
}