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

void windowPaint(windowData_t* _window)
{
    const char* toolWindow = "Tool Window";
    if ((uintptr_t)_window->m_title == (uintptr_t)toolWindow)
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(_window->m_handle, &ps);
        {
            TextOut(hdc, 5, 5, "greeting", strlen("greeting"));
        }
        EndPaint(_window->m_handle, &ps);
    }
}

void appUpdate(app_t* _app, globalContext_t* _global)
{
    if (!WindowShouldClose() && _app->m_running)
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) { UpdateCamera(&_global->camera, CAMERA_CUSTOM); }
        if (IsKeyPressed('R')) _global->camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };

        // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        float cameraPos[3] = { _global->camera.position.x, _global->camera.position.y, _global->camera.position.z };
        SetShaderValue(_global->mainShader, _global->mainShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        // Check key inputs to enable/disable lights
        if (IsKeyPressed(KEY_Y)) { _global->lights[0].enabled = !_global->lights[0].enabled; }
        if (IsKeyPressed(KEY_R)) { _global->lights[1].enabled = !_global->lights[1].enabled; }
        if (IsKeyPressed(KEY_G)) { _global->lights[2].enabled = !_global->lights[2].enabled; }
        if (IsKeyPressed(KEY_B)) { _global->lights[3].enabled = !_global->lights[3].enabled; }

        // Update light values (actually, only enable/disable them)
        for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(_global->mainShader, _global->lights[i]);

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            BeginMode3D(_global->camera);
            {
                DrawModel(_global->meshGround, Vector3Zero(), 1.0f, WHITE);
                DrawCube(_global->cubePosition, 2.0f, 2.0f, 2.0f, (Color) { 230, 41, 55, 255 });
                DrawCubeWires(_global->cubePosition, 2.0f, 2.0f, 2.0f, MAROON);

                // Draw spheres to show where the lights are
                for (int i = 0; i < MAX_LIGHTS; i++)
                {
                    if (_global->lights[i].enabled) DrawSphereEx(_global->lights[i].position, 0.2f, 8, 8, _global->lights[i].color);
                    else DrawSphereWires(_global->lights[i].position, 0.2f, 8, 8, ColorAlpha(_global->lights[i].color, 0.3f));
                }

                DrawGrid(10, 1.0f);
            }
            EndMode3D();
            DrawFPS(10, 10);
        }
        EndDrawing();
    }
}

int appKickstart(int argc, char **argv)
{
    static app_t app;
    appInit(&app, "My App");
    
    static windowArray_t windows;
    windowArrayInit(&app, &windows);
    windowInit(&windows, 0, 1024, 768, "First Window", WINDOW_MODE_WINDOW, NULL);
    windowInit(&windows, 1, 600, 400, "Tool Window", WINDOW_MODE_TOOL, windows.m_data[0].m_handle);

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_BORDERLESS_WINDOWED_MODE | FLAG_WINDOW_UNDECORATED);
    InitWindow(800, 600, "Raylib Window");

    // Init game global context
    globalContext_t globalContext = { 0 };
    {
        globalContext.camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
        globalContext.camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
        globalContext.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
        globalContext.camera.fovy = 45.0f;                                // Camera field-of-view Y
        globalContext.camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

        globalContext.mainShader = LoadShader("./src/lightning.vs", "./src/lightning.fs");
        globalContext.mainShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(globalContext.mainShader, "viewPos");
        const int ambientLoc = GetShaderLocation(globalContext.mainShader, "ambient");
        SetShaderValue(globalContext.mainShader, ambientLoc, (float[4]) { 0.1f, 0.1f, 0.1f, 1.0f }, SHADER_UNIFORM_VEC4);

        globalContext.meshGround = LoadModelFromMesh(GenMeshPlane(10.0f, 10.0f, 3, 3));
        globalContext.meshGround.materials[0].shader = globalContext.mainShader;

        globalContext.lights[0] = CreateLight(LIGHT_POINT, (Vector3) { -2, 1, -2 }, Vector3Zero(), YELLOW, globalContext.mainShader);
        globalContext.lights[1] = CreateLight(LIGHT_POINT, (Vector3) { 2, 1, 2 }, Vector3Zero(), RED, globalContext.mainShader);
        globalContext.lights[2] = CreateLight(LIGHT_POINT, (Vector3) { -2, 1, 2 }, Vector3Zero(), GREEN, globalContext.mainShader);
        globalContext.lights[3] = CreateLight(LIGHT_POINT, (Vector3) { 2, 1, -2 }, Vector3Zero(), BLUE, globalContext.mainShader);
    }

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

    appStart(&app, &globalContext);

    // Fermer la fenêtre raylib
    RL_CloseWindow();

    return 0;
}