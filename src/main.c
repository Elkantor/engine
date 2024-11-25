#include <stdio.h>

#include "appconfig.c"

static memoryStack stack1;
static app_t app;
static globalContext_t* globalContext = (globalContext_t*) & stack1;
static void* handleLib = NULL;

// NOTE(Victor): build with zig cc (clang) compiler on windows:
// zig cc src/main.c -g -Wall -Werror -lmCtrl -lcomctl32 -lDwmapi -lwinmm -lgdi32 -lopengl32 -lraylib -L./libs/raylib -L./libs/mctrl
// devenv a.exe (to open visual studio debugger)

Vector3 extractTranslation(Matrix* _mat)
{
    return (Vector3) { _mat->m12, _mat->m13, _mat->m14 };
}

void windowResize(windowData_t* _window, const int _width, const int _height)
{
    if (_window->m_handle == k_windows->m_data[0].m_handle)
    {
        const int mainWindowWith = _width;
        
        // Set property window size
        if (k_windows->m_data[1].m_handle != NULL)
        {
            const int propertyWindowWidth = mainWindowWith * 0.3f;
            const int propertyWindowX = mainWindowWith * 0.7f;
            SetWindowPos(k_windows->m_data[1].m_handle, HWND_TOP, propertyWindowX, 0, propertyWindowWidth, _height, SWP_DRAWFRAME);
        }

        // Set raylib viewport size
        if (GetWindowHandle() != NULL)
        {
            const int raylibWindowWidth = mainWindowWith * 0.7f;
            SetWindowPos(GetWindowHandle(), HWND_TOP, 0, 0, raylibWindowWidth, _height, SWP_DRAWFRAME);
            SetWindowSize(raylibWindowWidth, _height);
        }

        // HUD always on top
        SetWindowPos(k_windows->m_data[2].m_handle, HWND_TOP, 100, 5, 30, 30, SWP_DRAWFRAME);
    }
}

void windowFocus(windowData_t* _window)
{
    const char* mainWindow = "First Window";
    const char* toolWindow = "Tool Window";
    
    if ((uPtr)_window->m_title == (uPtr)mainWindow)
    {
        if (GetWindowHandle() != NULL)
        {
            SetWindowFocused();
        }
    }
    else if ((uPtr)_window->m_title == (uPtr)toolWindow)
    {
        if (_window->m_uiHandle != NULL)
        {
            SetFocus(_window->m_uiHandle);
        }
    }

    SetWindowPos(k_windows->m_data[2].m_handle, HWND_TOP, 100, 5, 30, 30, SWP_DRAWFRAME);
}

void windowNotify(windowData_t* _window, void* _data)
{
    NMHDR* hdr = (NMHDR*)_data;

    if (hdr->code == MC_HN_APPLINK)
    {
        MC_NMHTMLURLW* nmhtmlurl = (MC_NMHTMLURLW*)hdr;
        const string32_t value = string32Init((const char*)nmhtmlurl->pszUrl);
        const u64 hash = string32Hash(&value);

        switch (hash)
        {
            case string32HashConst("app:focusIn"):
            {
                windowFocus(_window);
                break;
            }
            case string32HashConst("app:SayHello"):
            {
                MessageBoxA(_window->m_handle, "Button Clicked", "Button Clicked!", MB_OK);
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

void appUpdate(app_t* _app, globalContext_t* _global)
{
    if (!WindowShouldClose() && _app->m_running)
    {
        // Get back focus on raylib window when window is overring
        if (CheckCollisionPointRec(GetMousePosition(), (RL_Rectangle) { 0, 0, GetScreenWidth(), GetScreenHeight() }))
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
            {
                windowFocus(&k_windows->m_data[0]);
            }
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) 
        { 
            UpdateCamera(&_global->camera, CAMERA_FREE);
        }

        if (IsKeyPressed(KEY_F5)) { _global->camera.target = (Vector3){ 0.0f, 0.0f, 0.0f }; }

        // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        const float cameraPosf[3] = { _global->camera.position.x, _global->camera.position.y, _global->camera.position.z };
        SetShaderValue(_global->mainShader, _global->mainShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPosf, SHADER_UNIFORM_VEC3);

        // Check key inputs to enable/disable lights
        if (IsKeyPressed(KEY_Y)) { _global->lights[0].enabled = !_global->lights[0].enabled; }
        if (IsKeyPressed(KEY_R)) { _global->lights[1].enabled = !_global->lights[1].enabled; }
        if (IsKeyPressed(KEY_G)) { _global->lights[2].enabled = !_global->lights[2].enabled; }
        if (IsKeyPressed(KEY_B)) { _global->lights[3].enabled = !_global->lights[3].enabled; }

        // Gizmo translation
        if (_global->meshSelected != NULL)
        {
            const float axisX = -IsKeyDown(KEY_LEFT) + IsKeyDown(KEY_RIGHT);
            const float axisZ = IsKeyDown(KEY_UP) - IsKeyDown(KEY_DOWN);

            if (axisX != 0.f || axisZ != 0.f)
            {
                _global->meshSelected->transform.m12 += 0.01f * axisX; 
                _global->meshSelected->transform.m14 += 0.01f * axisZ;
            }

            const Vector3 previousPos = extractTranslation(&_global->meshSelected->transform);
            const bool gizmoX = (_global->m_gizmoSelected == GIZMO_TRANSFORM_X && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
            const bool gizmoY = (_global->m_gizmoSelected == GIZMO_TRANSFORM_Y && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
            const bool gizmoZ = (_global->m_gizmoSelected == GIZMO_TRANSFORM_Z && IsMouseButtonDown(MOUSE_BUTTON_LEFT));

            if (gizmoX)
            {
                const Ray ray = GetMouseRay(GetMousePosition(), _global->camera);
                const Vector3 center = Vector3Divide(Vector3Add(_global->bbxSelected.min, _global->bbxSelected.max), (Vector3) { 2.0f, 2.0f, 2.0f });
                const Vector3 pos = Vector3Add(center, (Vector3) { 0.25, 0.f, 0.f });
                const BoundingBox bbx = (BoundingBox)
                {
                    .min = (Vector3)
                    {
                        .x = pos.x - 0.75f / 2.f,
                        .y = pos.y - 0.1f / 2.f,
                        .z = pos.z - 0.1f / 2.f,
                    },
                    .max = (Vector3)
                    {
                        .x = pos.x + 0.75f / 2.f,
                        .y = pos.y + 0.1f / 2.f,
                        .z = pos.z + 0.1f / 2.f,
                    }
                };
                const RayCollision rayColl = GetRayCollisionBox(ray, bbx);
                const Vector2 mouseDelta = GetMouseDelta();

                if (mouseDelta.x != 0.f && mouseDelta.y != 0.f)
                {
                    const Vector2 previousMousePos = Vector2Subtract(GetMousePosition(), mouseDelta);
                    const Ray prevRay = GetMouseRay(previousMousePos, _global->camera);
                    const RayCollision prevRayColl = GetRayCollisionBox(prevRay, bbx);
                    
                    const Vector3 diff = Vector3Subtract(rayColl.point, prevRayColl.point);
                    printf("Diff: %f %f %f\n", diff.x, diff.y, diff.z);
                    
                }

                //_global->meshSelected->transform.m12 += 0.01f * GetMouseDelta().x;
            }
            else if (gizmoY)
            {
                _global->meshSelected->transform.m13 += -0.01f * GetMouseDelta().y;
            }
            else if (gizmoZ)
            {
                _global->meshSelected->transform.m14 += -0.01f * GetMouseDelta().x;
            }

            const Vector3 afterPos = extractTranslation(&_global->meshSelected->transform);
            const Vector3 delta = Vector3Subtract(previousPos, afterPos);

            if (delta.x != 0.f || delta.y != 0.f || delta.z != 0.f)
            {
                const BoundingBox selected = GetModelBoundingBox(*_global->meshSelected);
                Vector3 pos = extractTranslation(&_global->meshSelected->transform);
                const BoundingBox afterPos =
                {
                    .min = (Vector3){ selected.min.x + pos.x, selected.min.y + pos.y, selected.min.z + pos.z },
                    .max = (Vector3){ selected.max.x + pos.x, selected.max.y + pos.y, selected.max.z + pos.z },
                };
                _global->bbxSelected = afterPos;

                uiBinderVector3(&pos, L"transformSet", k_windows->m_data[1].m_uiHandle);
            }
        }

        // Update light values (actually, only enable/disable them)
        for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(_global->mainShader, _global->lights[i]);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (_global->meshSelected != NULL)
            {
                Image image = LoadImageFromTexture(_global->m_renderTextureForeground.texture);
                image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

                const Color colorPicked = GetImageColor(image, GetMouseX(), image.height - GetMouseY()); // Need to reverse image on Y axis
                const int colorValue = ColorToInt(colorPicked);
                static const int colorGizmoTransformX = ColorToIntConst(RED);
                static const int colorGizmoTransformY = ColorToIntConst(BLUE);
                static const int colorGizmoTransformZ = ColorToIntConst(GREEN);

                switch (colorValue)
                {
                    case colorGizmoTransformX:
                    {
                        _global->m_gizmoSelected = GIZMO_TRANSFORM_X;
                        break;
                    }
                    case colorGizmoTransformY:
                    {
                        _global->m_gizmoSelected = GIZMO_TRANSFORM_Y;
                        break;
                    }
                    case colorGizmoTransformZ:
                    {
                        _global->m_gizmoSelected = GIZMO_TRANSFORM_Z;
                        break;
                    }
                    default:
                    {
                        _global->m_gizmoSelected = GIZMO_NONE;
                        break;
                    }
                }
            }

            if (_global->m_gizmoSelected == GIZMO_NONE)
            {
                Image image = LoadImageFromTexture(_global->renderTexture.texture);
                image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

                const Color colorPicked = GetImageColor(image, GetMouseX(), image.height - GetMouseY()); // Need to reverse image on Y axis
                const float pickingColor[4] = { (float)colorPicked.r, (float)colorPicked.g, (float)colorPicked.b, (float)colorPicked.a };
                const uint16_t id = convertFloat3ArrayToUint16(pickingColor);

                const float pickingColorNorm[4] = { (float)colorPicked.r / 255.f, (float)colorPicked.g / 255.f, (float)colorPicked.b / 255.f, (float)colorPicked.a / 255.f };
                SetShaderValue(_global->outlineShader, GetShaderLocation(_global->outlineShader, "pickingColor"), &pickingColorNorm, SHADER_UNIFORM_VEC4);
            
                if (colorPicked.r != WHITE.r && colorPicked.g != WHITE.g && colorPicked.b != WHITE.b)
                {
                    const uintptr_t addressMesh = (uintptr_t)&globalContext + id;
                    _global->meshSelected = (Model*)(addressMesh);

                    const BoundingBox selected = GetModelBoundingBox(*_global->meshSelected);
                    Vector3 pos = extractTranslation(&_global->meshSelected->transform);
                    const BoundingBox afterPos =
                    {
                        .min = (Vector3) { selected.min.x + pos.x, selected.min.y + pos.y, selected.min.z + pos.z },
                        .max = (Vector3) { selected.max.x + pos.x, selected.max.y + pos.y, selected.max.z + pos.z },
                    };
                    _global->bbxSelected = afterPos;
                    uiBinderVector3(&pos, L"transformSet", k_windows->m_data[1].m_uiHandle);

                    _global->m_bbxChecked = false;
                    int bbx = _global->m_bbxChecked;
                    uiBinderI32(&bbx, L"boundingBoxSet", k_windows->m_data[1].m_uiHandle);
                }
                else
                {
                    _global->meshSelected = NULL;
                    _global->bbxSelected.min = Vector3Zero();
                    _global->bbxSelected.max = Vector3Zero();
                    _global->m_bbxChecked = false;
                }
            }
        }

        BeginDrawing();
        {
            ClearBackground(DARKGRAY);
            BeginMode3D(_global->camera);
            {
                DrawModel(_global->meshGround, Vector3Zero(), 1.f, (Color) { 20, 20, 20, 255 });

                // // Church
                // {
                //     const Vector3 pos = extractTranslation(&_global->churchMesh.transform);
                //     DrawModel(_global->churchMesh, pos, 1.0f, WHITE);
                // }

                // // Graveyard Dead Tree
                // {
                //     const Vector3 pos = extractTranslation(&_global->m_graveyardTreeDecoratedMesh.transform);
                //     DrawModel(_global->m_graveyardTreeDecoratedMesh, pos, 1.0f, WHITE);
                // }

                // // Graveyard Pine Orange Large
                // {
                //     const Vector3 pos = extractTranslation(&_global->m_graveyardPineLargeMesh.transform);
                //     DrawModel(_global->m_graveyardPineLargeMesh, pos, 1.0f, WHITE);
                // }

                // // Graveyard Pine Orange Large
                // {
                //     const Vector3 pos = extractTranslation(&_global->m_graveyardArchGateMesh.transform);
                //     DrawModel(_global->m_graveyardArchGateMesh, pos, 1.0f, WHITE);
                // }

                // Cube
                {
                    const Vector3 pos = extractTranslation(&_global->meshCube.transform);
                    DrawModel(_global->meshCube, pos, 1.0f, WHITE);
                }

                // Draw spheres to show where the lights are
                for (int i = 0; i < MAX_LIGHTS; i++)
                {
                    if (_global->lights[i].enabled) DrawSphereEx(_global->lights[i].position, 0.2f, 8, 8, _global->lights[i].color);
                    else DrawSphereWires(_global->lights[i].position, 0.2f, 8, 8, ColorAlpha(_global->lights[i].color, 0.3f));
                }

                if (_global->meshSelected != NULL && _global->m_bbxChecked)
                {
                    DrawBoundingBox(_global->bbxSelected, WHITE);
                }
            }
            EndMode3D();

            // Draw Gizmo
            if (_global->meshSelected != NULL)
            {
                BeginTextureMode(_global->m_renderTextureForeground);
                    ClearBackground((Color) { 0, 0, 0, 0 });
                    BeginMode3D(_global->camera);
                    {
                        // Gizmo Arrow
                        {
                            const Vector3 center = Vector3Divide(Vector3Add(_global->bbxSelected.min, _global->bbxSelected.max), (Vector3) { 2.0f, 2.0f, 2.0f });
                            DrawSphere(center, 0.15f, (Color) { 230, 230, 230, 255 });

                            DrawCube(Vector3Add(center, (Vector3) { 0.25, 0.f, 0.f }), 0.75f, 0.1f, 0.1f, RED);

                            if (_global->m_gizmoSelected == GIZMO_TRANSFORM_X)
                            {
                                DrawCubeV(Vector3Add(center, (Vector3) { -10.f, 0.f, 0.f }), (Vector3) { 30.f, .1f, .1f }, RED);
                            }

                            DrawCube(Vector3Add(center, (Vector3) { 0.f, 0.25f, 0.f }), 0.1f, 0.75f, 0.1f, BLUE);
                            DrawCube(Vector3Add(center, (Vector3) { 0.f, 0.0f, 0.25f }), 0.1f, 0.1f, 0.75f, GREEN);
                        }
                    }
                    EndMode3D();
                EndTextureMode();
            }

            // Draw color picking id silhouettes to texture
            //----------------------------------------------------------------------------------
            BeginTextureMode(_global->renderTexture);
                ClearBackground(WHITE);
                BeginMode3D(_global->camera);

                // // Church mesh
                // {
                //     static const uint16_t churchID = offsetof(globalContext_t, churchMesh);
                //     static float color[4] = { 0,[3] = 1.f };
                //     convertUint16ToFloat3Array(churchID, color);
                //     color[0] /= 255.f; color[1] /= 255.f; color[2] /= 255.f;
                //     SetShaderValue(_global->materialFlatColor.shader, _global->shaderFlatColorLoc, &color, SHADER_UNIFORM_VEC4);
                //     const Vector3 pos = extractTranslation(&_global->churchMesh.transform);
                //     DrawModelMat(_global->churchMesh, pos, 1.0f, WHITE, &_global->materialFlatColor);
                // }

                // // Graveyard Tree mesh
                // {
                //     static const uint16_t meshID = offsetof(globalContext_t, m_graveyardTreeDecoratedMesh);
                //     static float color[4] = { 0,[3] = 1.f };
                //     convertUint16ToFloat3Array(meshID, color);
                //     color[0] /= 255.f; color[1] /= 255.f; color[2] /= 255.f;
                //     SetShaderValue(_global->materialFlatColor.shader, _global->shaderFlatColorLoc, &color, SHADER_UNIFORM_VEC4);
                //     const Vector3 pos = extractTranslation(&_global->m_graveyardTreeDecoratedMesh.transform);
                //     DrawModelMat(_global->m_graveyardTreeDecoratedMesh, pos, 1.0f, WHITE, &_global->materialFlatColor);
                // }

                // // Graveyard Pine Orange Large
                // {
                //     static const uint16_t meshID = offsetof(globalContext_t, m_graveyardPineLargeMesh);
                //     static float color[4] = { 0,[3] = 1.f };
                //     convertUint16ToFloat3Array(meshID, color);
                //     color[0] /= 255.f; color[1] /= 255.f; color[2] /= 255.f;
                //     SetShaderValue(_global->materialFlatColor.shader, _global->shaderFlatColorLoc, &color, SHADER_UNIFORM_VEC4);
                //     const Vector3 pos = extractTranslation(&_global->m_graveyardPineLargeMesh.transform);
                //     DrawModelMat(_global->m_graveyardPineLargeMesh, pos, 1.0f, WHITE, &_global->materialFlatColor);
                // }

                // // Graveyard Arch Gate
                // {
                //     static const uint16_t meshID = offsetof(globalContext_t, m_graveyardArchGateMesh);
                //     static float color[4] = { 0,[3] = 1.f };
                //     convertUint16ToFloat3Array(meshID, color);
                //     color[0] /= 255.f; color[1] /= 255.f; color[2] /= 255.f;
                //     SetShaderValue(_global->materialFlatColor.shader, _global->shaderFlatColorLoc, &color, SHADER_UNIFORM_VEC4);
                //     const Vector3 pos = extractTranslation(&_global->m_graveyardArchGateMesh.transform);
                //     DrawModelMat(_global->m_graveyardArchGateMesh, pos, 1.0f, WHITE, &_global->materialFlatColor);
                // }

                // Cube mesh
                {
                    static const uint16_t cubeID = offsetof(globalContext_t, meshCube);
                    static float color[4] = { 0,[3] = 1.f };
                    convertUint16ToFloat3Array(cubeID, color);
                    color[0] /= 255.f; color[1] /= 255.f; color[2] /= 255.f;
                    SetShaderValue(_global->materialFlatColor.shader, _global->shaderFlatColorLoc, &color, SHADER_UNIFORM_VEC4);
                    const Vector3 pos = extractTranslation(&_global->meshCube.transform);
                    DrawModelMat(_global->meshCube, pos, 1.0f, WHITE, &_global->materialFlatColor);
                }
                EndMode3D();
            EndTextureMode();

            // Draw outline
            //----------------------------------------------------------------------------------
            BeginShaderMode(_global->outlineShader);
            {
                const RL_Rectangle rec = { 0, 0, (float)_global->renderTexture.texture.width, (float)-_global->renderTexture.texture.height };
                DrawTextureRec(_global->renderTexture.texture, rec, (Vector2) { 0, 0 }, WHITE);
            }
            EndShaderMode();

            if (_global->meshSelected != NULL)
            {
                BeginShaderMode(_global->gizmoShader);
                {
                    const RL_Rectangle rec = { 0, 0, (float)_global->m_renderTextureForeground.texture.width, (float)-_global->m_renderTextureForeground.texture.height };
                    DrawTextureRec(_global->m_renderTextureForeground.texture, rec, (Vector2) { 0, 0 }, WHITE);
                }
                EndShaderMode();
            }

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

    globalContext_t* globalContext = MemoryStack.alloc(&stack1, sizeof(globalContext));

    appInit(&app, "My App");
    
    static windowArray_t windows;
    windowArrayInit(&app, &windows);
    windowInit(&windows, 0, 1280, 720, "First Window", WINDOW_MODE_WINDOW, WINDOW_FEATURE_RESIZEABLE | WINDOW_FEATURE_MINIMIZABLE | WINDOW_FEATURE_MAXIMIZABLE, NULL);
    
    windowInit(&windows, 1, 1280.f * 0.3f, 720, "Tool Window", WINDOW_MODE_WINDOW, WINDOW_FEATURE_HTML | WINDOW_FEATURE_RESIZEABLE | WINDOW_FEATURE_MINIMIZABLE | WINDOW_FEATURE_MAXIMIZABLE | WINDOW_FEATURE_BORDERLESS, NULL);
    const string32_t url = (string32_t){ .m_data = "\\res\\selection.html", .m_size = 32 };
    windowHTMLAdd(&windows, 1, &url);

    // Make 2nd window a child from 1st one
    {
        SetParent(windows.m_data[1].m_handle, windows.m_data[0].m_handle);
        const LONG nNewStyle = (GetWindowLong(windows.m_data[1].m_handle, GWL_STYLE) & ~WS_POPUP) | WS_CHILDWINDOW | WS_CHILD;
        SetWindowLong(windows.m_data[1].m_handle, GWL_STYLE, nNewStyle);
        const ULONG_PTR cNewStyle = GetClassLongPtr(windows.m_data[1].m_handle, GCL_STYLE) | CS_DBLCLKS;
        SetClassLongPtr(windows.m_data[1].m_handle, GCL_STYLE, cNewStyle);
        SetWindowPos(windows.m_data[1].m_handle, HWND_TOP, 1280.f * 0.7f, 0, 1280.f * 0.3f, 720, SWP_DRAWFRAME);
    }

    windowInit(&windows, 2, 100, 50, "Tool Window 2", WINDOW_MODE_FULLSCREEN_EXCLUSIVE, WINDOW_FEATURE_HTML | WINDOW_FEATURE_BORDERLESS | WINDOW_FEATURE_ON_TOP, NULL);
    const string32_t url2 = (string32_t){ .m_data = "\\res\\HUD.html", .m_size = 32 };
    windowHTMLAdd(&windows, 2, &url2);

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_BORDERLESS_WINDOWED_MODE | FLAG_WINDOW_UNDECORATED);
    InitWindow(1280.f * 0.7f, 720, "Raylib Window");

    // Initialize global context
    {
        globalContext->camera.position = (Vector3){ 5.0f, 5.0f, 5.0f }; // Camera position
        globalContext->camera.target = (Vector3){ 0.0f, 0.25f, 0.0f };      // Camera looking at point
        globalContext->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
        globalContext->camera.fovy = 60.0f;                                // Camera field-of-view Y
        globalContext->camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

        globalContext->mainShader = LoadShader("./src/lightning.vs", "./src/lightning.fs");
        globalContext->mainShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(globalContext->mainShader, "viewPos");

        globalContext->m_outlineSize = 2;
        globalContext->outlineShader = LoadShader(0, "./src/outline.fs");
        const int screenWidth = GetScreenWidth();
        const int screenHeight = GetScreenHeight();
        const float outlineColor[4] = { 1, 0.63, 0, 1 }; // Orange
        const float pickingColor[4] = { WHITE.r/255, WHITE.g/255, WHITE.b/255, WHITE.a/255 };
        SetShaderValue(globalContext->outlineShader, GetShaderLocation(globalContext->outlineShader, "width"), &screenWidth, SHADER_UNIFORM_INT);
        SetShaderValue(globalContext->outlineShader, GetShaderLocation(globalContext->outlineShader, "height"), &screenHeight, SHADER_UNIFORM_INT);
        SetShaderValue(globalContext->outlineShader, GetShaderLocation(globalContext->outlineShader, "color"), outlineColor, SHADER_UNIFORM_VEC4);
        SetShaderValue(globalContext->outlineShader, GetShaderLocation(globalContext->outlineShader, "pickingColor"), pickingColor, SHADER_UNIFORM_VEC4);
        SetShaderValue(globalContext->outlineShader, GetShaderLocation(globalContext->outlineShader, "size"), &globalContext->m_outlineSize, SHADER_UNIFORM_INT);

        globalContext->materialFlatColor = LoadMaterialDefault();
        UnloadShader(globalContext->materialFlatColor.shader);
        globalContext->materialFlatColor.shader = LoadShader(0, "./src/flatColor.fs");
        globalContext->shaderFlatColorLoc = GetShaderLocation(globalContext->materialFlatColor.shader, "color");

        globalContext->renderTexture = LoadRenderTexture(screenWidth, screenHeight);
        globalContext->m_renderTextureForeground = LoadRenderTexture(screenWidth, screenHeight);

        globalContext->gizmoShader = LoadShader(0, "./src/gizmo.fs");
        SetShaderValue(globalContext->gizmoShader, GetShaderLocation(globalContext->gizmoShader, "width"), &screenWidth, SHADER_UNIFORM_INT);
        SetShaderValue(globalContext->gizmoShader, GetShaderLocation(globalContext->gizmoShader, "height"), &screenHeight, SHADER_UNIFORM_INT);
        const float gizmoOutlineColor[] = {0.0f, 0.0f, 0.0f, 0.4f};
        SetShaderValue(globalContext->gizmoShader, GetShaderLocation(globalContext->gizmoShader, "outlineColor"), &gizmoOutlineColor, SHADER_UNIFORM_VEC4);

        globalContext->m_ambient = 3.0f;
        globalContext->m_ambientLoc = GetShaderLocation(globalContext->mainShader, "ambient");
        SetShaderValue(globalContext->mainShader, globalContext->m_ambientLoc, (float[4]) { globalContext->m_ambient, globalContext->m_ambient, globalContext->m_ambient, 1.f }, SHADER_UNIFORM_VEC4);

        globalContext->m_gamma = 2.2f;
        globalContext->m_gammaLoc = GetShaderLocation(globalContext->mainShader, "gamma");
        SetShaderValue(globalContext->mainShader, globalContext->m_gammaLoc, &globalContext->m_gamma, SHADER_UNIFORM_FLOAT);

        globalContext->meshGround = LoadModelFromMesh(GenMeshCube(100.0f, 100.0f, 1.0f));
        globalContext->meshGround.materials[0].shader = globalContext->mainShader;
        const float angle90 = 90.f * DEG2RAD;
        globalContext->meshGround.transform = MatrixMultiply(globalContext->meshGround.transform, MatrixRotateX(angle90));
        globalContext->meshGround.transform.m13 = -0.5f; // set y

        globalContext->meshCube = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
        globalContext->meshCube.materials[0].shader = globalContext->mainShader;
        {
            Matrix matScale = MatrixScale(1.0f, 1.0f, 1.0f);
            Matrix matRotation = MatrixRotate((Vector3){0.f, 0.f, 0.f}, 0.f);
            Matrix matTranslation = MatrixTranslate(0.f, 0.25f, 0.0f);
            Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
            globalContext->meshCube.transform = matTransform;
        }

        // globalContext->churchMesh = LoadModel("./res/models/church/churchMesh.obj");
        // globalContext->churchMesh.materials[0].shader = globalContext->mainShader;
        // globalContext->churchMesh.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("./res/models/church/churchTextureDiffuse.png");
        // {
        //     Matrix matScale = MatrixScale(0.1f, 0.1f, 0.1f);
        //     Matrix matRotation = MatrixRotate((Vector3) { 0.f, 0.f, 0.f }, 0.f);
        //     Matrix matTranslation = MatrixTranslate(0.0f, 0.f, 0.f);
        //     Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
        //     globalContext->churchMesh.transform = matTransform;

        //     BoundingBox bbx = GetModelBoundingBox(globalContext->churchMesh);
        //     Vector3 originalAnchor = Vector3Divide(Vector3Add(bbx.min, bbx.max), (Vector3) { 2.0f, 2.0f, 2.0f });
        //     globalContext->churchMesh.transform.m12 += originalAnchor.x;
        //     globalContext->churchMesh.transform.m13 += 0.f;
        //     globalContext->churchMesh.transform.m14 += originalAnchor.z; 
        // }

        // // Graveyard assets
        // {
        //     const Texture2D graveYardTexture2D = LoadTexture("./res/models/graveyard/halloweenbits_texture.png");

        //     // Tree Decorated
        //     {
        //         globalContext->m_graveyardTreeDecoratedMesh = LoadModel("./res/models/graveyard/deadTreeDecorated/tree_dead_large_decorated.obj");
        //         globalContext->m_graveyardTreeDecoratedMesh.materials[0].shader = globalContext->mainShader;
        //         globalContext->m_graveyardTreeDecoratedMesh.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = graveYardTexture2D;
        //         {
        //             Matrix matScale = MatrixScale(1.f, 1.f, 1.f);
        //             Matrix matRotation = MatrixRotate((Vector3) { 0.f, 0.f, 0.f }, 0.f);
        //             Matrix matTranslation = MatrixTranslate(2.0f, 0.f, 0.f);
        //             Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
        //             globalContext->m_graveyardTreeDecoratedMesh.transform = matTransform;

        //             BoundingBox bbx = GetModelBoundingBox(globalContext->m_graveyardTreeDecoratedMesh);
        //             Vector3 originalAnchor = Vector3Divide(Vector3Add(bbx.min, bbx.max), (Vector3) { 2.0f, 2.0f, 2.0f });
        //             globalContext->m_graveyardTreeDecoratedMesh.transform.m12 += originalAnchor.x;
        //             globalContext->m_graveyardTreeDecoratedMesh.transform.m13 += 0.f;
        //             globalContext->m_graveyardTreeDecoratedMesh.transform.m14 += originalAnchor.z;
        //         }
        //     }

        //     // Pine Orange Large
        //     {
        //         globalContext->m_graveyardPineLargeMesh = LoadModel("./res/models/graveyard/deadTreeDecorated/tree_pine_orange_large.obj");
        //         globalContext->m_graveyardPineLargeMesh.materials[0].shader = globalContext->mainShader;
        //         globalContext->m_graveyardPineLargeMesh.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = graveYardTexture2D;
        //         {
        //             Matrix matScale = MatrixScale(1.f, 1.f, 1.f);
        //             Matrix matRotation = MatrixRotate((Vector3) { 0.f, 0.f, 0.f }, 0.f);
        //             Matrix matTranslation = MatrixTranslate(2.0f, 0.f, 0.f);
        //             Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
        //             globalContext->m_graveyardPineLargeMesh.transform = matTransform;

        //             BoundingBox bbx = GetModelBoundingBox(globalContext->m_graveyardPineLargeMesh);
        //             Vector3 originalAnchor = Vector3Divide(Vector3Add(bbx.min, bbx.max), (Vector3) { 2.0f, 2.0f, 2.0f });
        //             globalContext->m_graveyardPineLargeMesh.transform.m12 += originalAnchor.x;
        //             globalContext->m_graveyardPineLargeMesh.transform.m13 += 0.f;
        //             globalContext->m_graveyardPineLargeMesh.transform.m14 += originalAnchor.z;
        //         }
        //     }

        //     // Arch Gate
        //     {
        //         globalContext->m_graveyardArchGateMesh = LoadModel("./res/models/graveyard/deadTreeDecorated/arch_gate.obj");
        //         globalContext->m_graveyardArchGateMesh.materials[0].shader = globalContext->mainShader;
        //         globalContext->m_graveyardArchGateMesh.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = graveYardTexture2D;
        //         {
        //             Matrix matScale = MatrixScale(1.f, 1.f, 1.f);
        //             Matrix matRotation = MatrixRotate((Vector3) { 0.f, 0.f, 0.f }, 0.f);
        //             Matrix matTranslation = MatrixTranslate(1.0f, 0.f, 2.f);
        //             Matrix matTransform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
        //             globalContext->m_graveyardArchGateMesh.transform = matTransform;

        //             BoundingBox bbx = GetModelBoundingBox(globalContext->m_graveyardArchGateMesh);
        //             Vector3 originalAnchor = Vector3Divide(Vector3Add(bbx.min, bbx.max), (Vector3) { 2.0f, 2.0f, 2.0f });
        //             globalContext->m_graveyardArchGateMesh.transform.m12 += originalAnchor.x;
        //             globalContext->m_graveyardArchGateMesh.transform.m13 += 0.f;
        //             globalContext->m_graveyardArchGateMesh.transform.m14 += originalAnchor.z;
        //         }
        //     }
        // }

        globalContext->lights[0] = CreateLight(LIGHT_POINT, (Vector3) { -2, 1, -2 }, Vector3Zero(), YELLOW, globalContext->mainShader);
        globalContext->lights[1] = CreateLight(LIGHT_POINT, (Vector3) { 2, 1, 2 }, Vector3Zero(), RED, globalContext->mainShader);
        globalContext->lights[2] = CreateLight(LIGHT_POINT, (Vector3) { -2, 1, 2 }, Vector3Zero(), GREEN, globalContext->mainShader);
        globalContext->lights[3] = CreateLight(LIGHT_POINT, (Vector3) { 2, 1, -2 }, Vector3Zero(), BLUE, globalContext->mainShader);
    }

    // Make raylib window as child
    {
        SetParent(GetWindowHandle(), windows.m_data[0].m_handle);
        const LONG nNewStyle = (GetWindowLong(GetWindowHandle(), GWL_STYLE) & ~WS_POPUP) | WS_CHILDWINDOW | WS_CHILD;
        SetWindowLong(GetWindowHandle(), GWL_STYLE, nNewStyle);
        const ULONG_PTR cNewStyle = GetClassLongPtr(GetWindowHandle(), GCL_STYLE) | CS_DBLCLKS;
        SetClassLongPtr(GetWindowHandle(), GCL_STYLE, cNewStyle);
        SetWindowPosition(0, 0);
    }

    // Make window 3 a child from first one, and on top of raylib
    {
        SetParent(windows.m_data[2].m_handle, windows.m_data[0].m_handle);
        const LONG nNewStyle = (GetWindowLong(windows.m_data[2].m_handle, GWL_STYLE) & ~WS_POPUP) | WS_CHILDWINDOW | WS_CHILD;
        SetWindowLong(windows.m_data[2].m_handle, GWL_STYLE, nNewStyle);
        const ULONG_PTR cNewStyle = GetClassLongPtr(windows.m_data[2].m_handle, GCL_STYLE) | CS_DBLCLKS;
        SetClassLongPtr(windows.m_data[2].m_handle, GCL_STYLE, cNewStyle);

        SetWindowPos(windows.m_data[2].m_handle, HWND_TOP, 100, 5, 30, 30, SWP_DRAWFRAME);
        MARGINS margins = { -10, -10, -10, -10 };
        DwmExtendFrameIntoClientArea(windows.m_data[2].m_handle, &margins);
    }

    const uint64_t stackSize = appStackSizeGet();
    printf("App stack size left: %zu\n", stackSize);

    appStart(&app, globalContext);

    RL_CloseWindow();
    mcHtml_Terminate();

    if (handleLib != NULL) { hotReloadLibDelete(handleLib); }
    system("rm -rf tmp.lib tmp.pdb *tmp*.lib");

    return 0;
}