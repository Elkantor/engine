#include "appconfig.c"

// Compile with: zig cc src/mainEditor.c -g -Wall -Werror -Wshadow -Wconversion -lmsvcrt -llibcmt -lOleAut32 -lOle32 -lDwmapi -lcomctl32 -lDwmapi -lwinmm -lgdi32 -lopengl32 -lUxTheme -lhsluv-c -lmd4c-utf16 -lmCtrl_static -lraylib -L./libs/raylib -L./libs/mctrl

memoryStackStruct stackScratch;
memoryStackStruct frameOdd;
memoryStackStruct frameEven;

void windowResize(windowData_t* _window, const u32 _width, const u32 _height)
{
    const char* mainWindow = "First Window";

    if ((uPtr)_window->m_title == (uPtr)mainWindow)
    {
        const u32 mainWindowWith = _width;

        const char* toolWindow = "Tool Window";

        // Set property window size
        if (FindWindowA(NULL, toolWindow))
        {
            const i32 propertyWindowWidth = (i32)((f32)mainWindowWith * 0.3f);
            const i32 propertyWindowX = (i32)((f32)mainWindowWith * 0.7f);
            SetWindowPos(k_windows->m_data[1].m_handle, HWND_TOP, propertyWindowX, 0, propertyWindowWidth, (i32)_height, SWP_DRAWFRAME);
        }

        // Set raylib viewport size
        if (GetWindowHandle() != NULL)
        {
            const i32 raylibWindowWidth = (i32)((f32)mainWindowWith * 0.7f);
            SetWindowPos(GetWindowHandle(), HWND_TOP, 0, 0, raylibWindowWidth, (i32)_height, SWP_DRAWFRAME);
            SetWindowSize(raylibWindowWidth, (i32)_height);
        }
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
            case string32HashConst("app:CreateCube"):
            {
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

void appUpdate(app_t* _app, memoryStackStruct* restrict _previousStack, memoryStackStruct* restrict _currentStack)
{
    //const memoryStackStruct* const previousFrame = _previousStack;

    if (!WindowShouldClose() && _app->m_running)
    {
        BeginDrawing();
            ClearBackground(ORANGE);
        EndDrawing();
    }

    memoryStack.swap(&_previousStack, &_currentStack);
}

int appKickstart(int argc, char** argv)
{
    // Mctrl initializations
    mcHtml_Initialize();
    InitCommonControls();

    memset(&stackScratch, 0, sizeof(stackScratch));
    memset(&frameEven, 0, sizeof(frameEven));
    memset(&frameOdd, 0, sizeof(frameOdd));

    app_t* app = memoryStack.push(&stackScratch, sizeof(app_t));
    appInit(app, "My App");

    windowArray_t* windows = memoryStack.push(&stackScratch, sizeof(windowArray_t));
    windowArrayInit(app, windows);
    windowInit(windows, 0, 1280, 720, "First Window", WINDOW_MODE_WINDOW, WINDOW_FEATURE_RESIZEABLE | WINDOW_FEATURE_MINIMIZABLE | WINDOW_FEATURE_MAXIMIZABLE, NULL);
    windowHTMLAdd(windows, 0, &(string32_t){.m_data = "\\res\\main.html", .m_size = 32 });

    // // Make 2nd window a child from 1st one
    // {
    //     SetParent(windows->m_data[1].m_handle, windows->m_data[0].m_handle);
    //     SetWindowPos(windows->m_data[1].m_handle, HWND_TOP, (i32)(1280.f * 0.7f), 0, (i32)(1280.f * 0.3f), 720, SWP_DRAWFRAME);
    // }

    // // Make 2nd window transparent 
    // {
    //     SetWindowLong(windows->m_data[0].m_handle, GWL_EXSTYLE, GetWindowLong(windows->m_data[0].m_handle, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_COMPOSITED);
    //     SetLayeredWindowAttributes(windows->m_data[0].m_handle, 0, (255 * 50) / 100, LWA_ALPHA);

    //     SetWindowLong(windows->m_data[1].m_handle, GWL_EXSTYLE, GetWindowLong(windows->m_data[1].m_handle, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    //     SetLayeredWindowAttributes(windows->m_data[1].m_handle, 0, (255 * 10) / 100, LWA_ALPHA);
    // }

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_BORDERLESS_WINDOWED_MODE | FLAG_WINDOW_UNDECORATED);
    InitWindow((i32)(1280.f * 0.7f), 720 - 50, "Raylib Window");

    // Make raylib window as child
    {
        SetParent(GetWindowHandle(), windows->m_data[0].m_handle);
        const LONG style = GetWindowLong(GetWindowHandle(), GWL_STYLE);
        const LONG AsPopup = (LONG)(~WS_POPUP);
        const LONG nNewStyle = (style & AsPopup) | WS_CHILDWINDOW | WS_CHILD;
        SetWindowLong(GetWindowHandle(), GWL_STYLE, nNewStyle);
        const ULONG_PTR cNewStyle = GetClassLongPtr(GetWindowHandle(), GCL_STYLE) | CS_DROPSHADOW | CS_PARENTDC;
        SetClassLongPtr(GetWindowHandle(), GCL_STYLE, cNewStyle);
        SetWindowPos(GetWindowHandle(), HWND_TOP, 0, 50, 1280, 720-50, SWP_DRAWFRAME);
    }

    const u64 stackSize = appStackSizeGet();
    printf("App stack size left: %zu\n", stackSize);

    appStart(app, &frameEven, &frameOdd);

    RL_CloseWindow();
    mcHtml_Terminate();

    return 0;
}