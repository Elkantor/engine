#include <stdio.h>

#include "engine/utils/memory/memoryStack.c"
#include "appconfig.c"

static app_t app;
static globalContext_t globalContext = { 0 };

// NOTE(Victor): build with mingw on windows:
// gcc src/main.c -g -Wall -Werror -lmCtrl -lcomctl32 -lDwmapi -lwinmm -lgdi32 -lopengl32 -lraylib -L./libs/raylib -L./libs/mctrl
// devenv a.exe (to open visual studio debugger)

// NOTE(Victor): build with zig cc (clang) compiler on windows:
// zig cc src/main.c -g -Wall -Werror -lmCtrl -lcomctl32 -lDwmapi -lwinmm -lgdi32 -lopengl32 -lraylib -L./libs/raylib -L./libs/mctrl
// devenv a.exe (to open visual studio debugger)

void windowResize(windowData_t* _window, const int _width, const int _height)
{
    
}

void windowFocus(windowData_t* _window)
{
    const char* mainWindow = "First Window";
    if ((uintptr_t)_window->m_title == (uintptr_t)mainWindow)
    {
        SetFocus(GetWindowHandle());
    }
}

void windowNotify(windowData_t* _window, void* _data)
{
    NMHDR* hdr = (NMHDR*)_data;

    if (hdr->code == MC_HN_APPLINK)
    {
        MC_NMHTMLURLW* nmhtmlurl = (MC_NMHTMLURLW*)hdr;
        const string32_t value = string32Init((const char*)nmhtmlurl->pszUrl);
        const uint64_t hash = string32Hash(&value);

        static const uint64_t appHelloHash = string32HashConst("app:SayHello");
        static const uint64_t appHUDClickedHash = string32HashConst("app:HUDClicked");
        static const uint64_t appCopyClipboardAvatar = string32HashConst("app:CopyClipboardAvatar");

        switch (hash)
        {
            case appHelloHash:
            {
                MessageBoxA(_window->m_handle, "Button Clicked", "Button Clicked!", MB_OK);
                break;
            }
            case appHUDClickedHash:
            {
                MessageBoxA(_window->m_handle, "Button Clicked", "Button Clicked!", MB_OK);
                SetFocus(k_windows->m_data[0].m_handle);

                MC_HMCALLSCRIPTFUNC csfArgs;
                WCHAR pszRetVal[128];

                csfArgs.cbSize = sizeof(MC_HMCALLSCRIPTFUNC);
                csfArgs.pszRet = pszRetVal;
                csfArgs.iRet = sizeof(pszRetVal) / sizeof(pszRetVal[0]);
                csfArgs.cArgs = 0;
                windowData_t* window = &k_windows->m_data[2];
                SendMessageW(window->m_uiHandle, MC_HM_CALLSCRIPTFUNC, (WPARAM)L"looseFocus", (LPARAM)&csfArgs);

                break;
            }
            case appCopyClipboardAvatar:
            {
                MC_HMCALLSCRIPTFUNC csfArgs;
                WCHAR pszRetVal[128];

                csfArgs.cbSize = sizeof(MC_HMCALLSCRIPTFUNC);
                csfArgs.pszRet = pszRetVal;
                csfArgs.iRet = sizeof(pszRetVal) / sizeof(pszRetVal[0]);
                csfArgs.cArgs = 1;
                csfArgs.pszArg1 = L"avatar";
                windowData_t* window = &k_windows->m_data[1];
                SendMessageW(window->m_uiHandle, MC_HM_CALLSCRIPTFUNC, (WPARAM)L"getCopiedValue", (LPARAM)&csfArgs);

                appClipboardCopy(pszRetVal, wcsnlen_s(pszRetVal, 128));
                break;
            }
            default:
                break;
        }
    }
}

// Fonction pour extraire le vecteur de translation d'une matrice de transformation
Vector3 extractTranslation(const Matrix* transformMatrix)
{
    Vector3 translation;
    translation.x = transformMatrix->m12;
    translation.y = transformMatrix->m13;
    translation.z = transformMatrix->m14;
    return translation;
}

void appUpdate(app_t* _app, globalContext_t* _global)
{
    if (!WindowShouldClose() && _app->m_running)
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) 
        { 
            UpdateCamera(&_global->camera, CAMERA_CUSTOM);
            MC_HMCALLSCRIPTFUNC csfArgs;
            WCHAR pszRetVal[64];

            wchar_t posX[5] = { 0 };
            swprintf(posX, 5, L"%f", _global->camera.position.x);

            csfArgs.cbSize = sizeof(MC_HMCALLSCRIPTFUNC);
            csfArgs.pszRet = pszRetVal;
            csfArgs.iRet = sizeof(pszRetVal) / sizeof(pszRetVal[0]);
            csfArgs.pszArg1 = posX;
            csfArgs.cArgs = 1;
            windowData_t* window = &k_windows->m_data[1];
            SendMessageW(window->m_uiHandle, MC_HM_CALLSCRIPTFUNC, (WPARAM)L"updateSlider", (LPARAM)&csfArgs);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            
        }

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
            ClearBackground(BLUE);
            BeginMode3D(_global->camera);
            {
                DrawModel(_global->meshGround, Vector3Zero(), 1.f, WHITE);
                DrawModel(_global->meshCube, extractTranslation(&_global->meshCube.transform), 1.0f, WHITE);

                // Draw spheres to show where the lights are
                for (int i = 0; i < MAX_LIGHTS; i++)
                {
                    if (_global->lights[i].enabled) DrawSphereEx(_global->lights[i].position, 0.2f, 8, 8, _global->lights[i].color);
                    else DrawSphereWires(_global->lights[i].position, 0.2f, 8, 8, ColorAlpha(_global->lights[i].color, 0.3f));
                }

                DrawGrid(100, 1.0f);
            }
            EndMode3D();
            DrawFPS(10, 10);
        }
        EndDrawing();
    }
}

int appKickstart(int argc, char **argv)
{
    // Mctrl initializations
    mcHtml_Initialize();
    InitCommonControls();

    appInit(&app, "My App");
    
    static windowArray_t windows;
    windowArrayInit(&app, &windows);
    windowInit(&windows, 0, 1024, 768, "First Window", WINDOW_MODE_WINDOW, WINDOW_FEATURE_RESIZEABLE | WINDOW_FEATURE_MINIMIZABLE | WINDOW_FEATURE_MAXIMIZABLE, NULL);
    
    windowInit(&windows, 1, 600, 400, "Tool Window", WINDOW_MODE_WINDOW, WINDOW_FEATURE_HTML | WINDOW_FEATURE_RESIZEABLE | WINDOW_FEATURE_MINIMIZABLE | WINDOW_FEATURE_MAXIMIZABLE, NULL);
    const string32_t url = (string32_t){ .m_data = "\\res\\doc2.html", .m_size = 32 };
    windowHTMLAdd(&windows, 1, &url);

    windowInit(&windows, 2, 100, 50, "Tool Window 2", WINDOW_MODE_FULLSCREEN_EXCLUSIVE, WINDOW_FEATURE_HTML | WINDOW_FEATURE_BORDERLESS | WINDOW_FEATURE_ON_TOP, NULL);
    const string32_t url2 = (string32_t){ .m_data = "\\res\\HUD.html", .m_size = 32 };
    windowHTMLAdd(&windows, 2, &url2);

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_BORDERLESS_WINDOWED_MODE | FLAG_WINDOW_UNDECORATED);
    InitWindow(800, 600, "Raylib Window");

    {
        globalContext.camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
        globalContext.camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
        globalContext.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
        globalContext.camera.fovy = 60.0f;                                // Camera field-of-view Y
        globalContext.camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

        globalContext.mainShader = LoadShader("./src/lightning.vs", "./src/lightning.fs");
        globalContext.mainShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(globalContext.mainShader, "viewPos");
        const int ambientLoc = GetShaderLocation(globalContext.mainShader, "ambient");
        SetShaderValue(globalContext.mainShader, ambientLoc, (float[4]) { 2.f, 2.f, 2.f, 1.0f }, SHADER_UNIFORM_VEC4);

        globalContext.meshGround = LoadModelFromMesh(GenMeshCube(100.0f, 100.0f, 1.0f));
        globalContext.meshGround.materials[0].shader = globalContext.mainShader;
        const float angle90 = 90.f * DEG2RAD;
        globalContext.meshGround.transform = MatrixMultiply(globalContext.meshGround.transform, MatrixRotateX(angle90));
        globalContext.meshGround.transform.m13 = -0.5f; // set y

        globalContext.meshCube = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
        globalContext.meshCube.materials[0].shader = globalContext.mainShader;

        {
            Matrix matScale = MatrixScale(1.0f, 1.0f, 1.0f);
            Matrix matRotation = MatrixRotate((Vector3){0.f, 0.f, 0.f}, 0.f);
            Matrix matTranslation = MatrixTranslate(0.f, 0.25f, 0.0f);
            Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
            globalContext.meshCube.transform = matTransform;
        }

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

    SetParent(windows.m_data[2].m_handle, windows.m_data[0].m_handle);
    SetWindowPos(windows.m_data[2].m_handle, HWND_TOP, 100, 5, 30, 30, SWP_NOREPOSITION);
    MARGINS margins = { -10, -10, -10, -10 };
    DwmExtendFrameIntoClientArea(windows.m_data[2].m_handle, &margins);

    appStart(&app, &globalContext);

    RL_CloseWindow();
    mcHtml_Terminate();

    return 0;
}