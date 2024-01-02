#pragma once

#include <stddef.h>
#include <Windows.h>
#include <WinUser.h>
#include <stdint.h>
#include <string.h>
#include <windef.h>
#include <DbgHelp.h>
#include <dwmapi.h>

#include "../../../window.c"
#include "../../../utils/recti32_t.c"
#include "../../../system.c"

void windowInit(windowArray_t* _windows, const uint32_t _index, const int _width, const int _height, const char* _name, const windowMode_t _mode, const windowFeature_t _features, void* _ownerHandle)
{
	windowData_t* windowData = &_windows->m_data[_windows->m_size];

	// Init the windows
	{
		windowDataDefaultSet(windowData);
	
		windowData->m_width = _width;
		windowData->m_height = _height;
		windowData->m_title = _name;
		windowData->m_displayIndex = displayPrimaryGet(&_windows->m_app->m_displays);
		windowData->m_mode = _mode;
		windowData->m_windowFeatures |= _features;
	}

	const bool appInitialized = _windows->m_size > 0;
	windowCreate(&_windows->m_app->m_displays, windowData, appInitialized, _ownerHandle);
	_windows->m_size += 1;
	
	windowShow(windowData);
}

uint32_t windowIndexGet(windowArray_t* _windows, void* _handle)
{
	for (uint32_t i = 0; i < _windows->m_size; ++i)
	{
		const windowData_t* current = &_windows->m_data[i];

		if (current->m_handle == _handle)
		{
			return i;
		}
	}

	return UINT32_MAX;
}

void windowDestroy(windowArray_t* _windows, void* _handle)
{
	const uint32_t windowIndex = windowIndexGet(_windows, _handle); 

	if (windowIndex == UINT32_MAX)
	{
		return;
	}

	if (_windows->m_data[windowIndex].m_uiHandle != NULL)
	{
		DestroyWindow(_handle);
	}

	DestroyWindow(_handle);
	memset(&_windows->m_data[windowIndex], 0, sizeof(_windows->m_data[0]));
	_windows->m_data[windowIndex] = _windows->m_data[--_windows->m_size];
}

void windowHTMLAdd(windowArray_t* _windows, const uint32_t _index, const string32_t* _path)
{
	wString256_t absolutePath;
	appPathAbsoluteGet(&absolutePath);

	char absolutePathUTF8[128] = { 0 };
	WideCharToMultiByte(CP_UTF8, 0, absolutePath.m_data, -1, absolutePathUTF8, 64 - 1, NULL, NULL);

	strncat_s(absolutePathUTF8, 128, _path->m_data, _path->m_size);

	windowData_t* windowData = &_windows->m_data[_index];

	windowData->m_uiHandle = CreateWindowA("mCtrl.html", absolutePathUTF8, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, 0, 0, 0, windowData->m_handle, NULL, GetModuleHandle(NULL), windowData->m_handle);
	
	RECT clientRect;
	GetWindowRect(windowData->m_handle, &clientRect);
	const uint32_t width = clientRect.right - clientRect.left;
	const uint32_t height = clientRect.bottom - clientRect.top;

	SendMessage(windowData->m_handle, WM_SIZE, SIZE_RESTORED, width << 16 | height);
}

LRESULT WINAPI windowsMessageProcedure(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	//int windowId;
	//DWORD pointerId;
	//POINTER_INFO pointerInfo = {0};
	//POINTER_PEN_INFO penInfo = {0};
	
	switch (_msg) 
	{
		case WM_NOTIFY:
		{
			if (k_windows->m_size > 0)
			{
				const uint32_t windowIndex = windowIndexGet(k_windows, _hWnd);
				if (windowIndex == UINT32_MAX) break;

				windowData_t* window = &k_windows->m_data[windowIndex];
				windowNotify(window, (NMHDR*)_lParam);
				return 0;
			}
		}
		case WM_SETFOCUS:
		{
			if (k_windows->m_size > 0)
			{
				const uint32_t windowIndex = windowIndexGet(k_windows, _hWnd);
				if (windowIndex == UINT32_MAX) break;

				windowData_t* window = &k_windows->m_data[windowIndex];

				if (window->m_uiHandle != NULL)
				{
					SetFocus(window->m_uiHandle);
				}
				windowFocus(window);
			}
			return 0;
		}
		case WM_NCCREATE:
		{
			break;
		}
		case WM_CREATE:
		{
			break;
		}
		case /*WM_DPICHANGED*/ 0x02E0: 
		{
			break;
		}
		case WM_CLOSE:
		{
			windowDestroy(k_windows, _hWnd);

			if (k_windows->m_size == 0)
			{
				appStop(k_windows->m_app);
				const wchar_t* windowClassName = L"Window";
				UnregisterClassW(windowClassName, GetModuleHandle(NULL));
			}
			
			return 0;
		}
		case WM_SIZE:
		{
			uint32_t windowIndex = windowIndexGet(k_windows, _hWnd);
			if (windowIndex == UINT32_MAX)
			{
				windowIndex = k_windows->m_size;
			}

			windowData_t* window = &k_windows->m_data[windowIndex];

			if (window->m_uiHandle != NULL)
			{
				RECT clientRect;
				GetWindowRect(window->m_handle, &clientRect);
				const uint32_t width = clientRect.right - clientRect.left;
				const uint32_t height = clientRect.bottom - clientRect.top;
				SetWindowPos(window->m_uiHandle, NULL, 0, 0, width, height, SWP_NOZORDER);
			}

			const int width = LOWORD(_lParam);
			const int height = HIWORD(_lParam);

			window->m_width = width;
			window->m_height = height;

			windowResize(window, width, height);
			break;
		}
		/*case WM_PAINT:
		{
			if (k_windows->m_size > 0)
			{
				const uint32_t windowIndex = windowIndexGet(k_windows, _hWnd);
				if (windowIndex == UINT32_MAX) return DefWindowProcW(_hWnd, _msg, _wParam, _lParam);;

				windowData_t* window = &k_windows->m_data[windowIndex];
				const int result = windowPaint(window);

				if (result == -1)
				{
					return DefWindowProcW(_hWnd, _msg, _wParam, _lParam);
				}
			}
			break;
		}*/
	}

	return DefWindowProcW(_hWnd, _msg, _wParam, _lParam);
}

void RegisterWindowClass(HINSTANCE _hInstance, const wchar_t* _className)
{
	WNDCLASSEXW wc = {
		sizeof(WNDCLASSEXA),
		CS_OWNDC /*CS_CLASSDC*/,
		windowsMessageProcedure,
		0L,
		0L,
		_hInstance,
		LoadIcon(_hInstance, MAKEINTRESOURCE(107)),
		LoadCursor(NULL, IDC_ARROW),
		0,
		0,
		_className,
		0
	};
	RegisterClassExW(&wc);
}

bool windowDisplayModeSet(displayDataArray_t* _displays, const uint32_t _displayIndex, const windowData_t* _windowData)
{
	if (_displayIndex >= _displays->m_size)
	{
		return false;
	}

	displayData_t* display = &_displays->m_data[_displayIndex];
	display->m_modeChanged = true;

	DEVMODEA mode = {0};
	mode.dmSize = sizeof(mode);
	strncpy((char *)mode.dmDeviceName, display->m_name.m_data, string32CapacityGet());
	mode.dmPelsWidth = _windowData->m_width;
	mode.dmPelsHeight = _windowData->m_height;
	mode.dmBitsPerPel = _windowData->m_frameBufferOptions.m_colorBits;
	mode.dmDisplayFrequency = _windowData->m_frameBufferOptions.m_frequency;
	mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

	bool success = ChangeDisplaySettingsA(&mode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;

	if (success == false)
	{
		mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		success = ChangeDisplaySettingsA(&mode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
	}

	return success;
}

DWORD styleGet(const int _features)
{
	DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;

	if ((_features & WINDOW_FEATURE_RESIZEABLE) && ((_features & WINDOW_FEATURE_BORDERLESS) == 0))
	{
		style |= WS_SIZEBOX;
	}

	if (_features & WINDOW_FEATURE_MAXIMIZABLE)
	{
		style |= WS_MAXIMIZEBOX;
	}

	if (_features & WINDOW_FEATURE_MINIMIZABLE)
	{
		style |= WS_MINIMIZEBOX;
	}

	if ((_features & WINDOW_FEATURE_BORDERLESS) == 0)
	{
		style |= WS_CAPTION | WS_SYSMENU;
	}

	return style;
}

DWORD dwStyleGet(const windowMode_t _mode, const int _features)
{
	switch (_mode)
	{
		case WINDOW_MODE_FULLSCREEN_EXCLUSIVE:
		{
			return WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
		}
		case WINDOW_MODE_FULLSCREEN:
		{
			return WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
		}
		case WINDOW_MODE_TOOL:
		{
			return WS_POPUP | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_VISIBLE;
		}
		case WINDOW_MODE_WINDOW:
		{
			return styleGet(_features);
		}
		default:
		{
			return styleGet(_features);
		}
	}
}

DWORD styleExGet(const int _features)
{
	DWORD exStyle = WS_EX_APPWINDOW;

	if ((_features & WINDOW_FEATURE_BORDERLESS) == 0)
	{
		exStyle |= WS_EX_WINDOWEDGE;
	}

	if (_features & WINDOW_FEATURE_ON_TOP)
	{
		exStyle |= WS_EX_TOPMOST;
	}

	return exStyle;
}

DWORD dwStyleExGet(const windowMode_t _mode, const int _features)
{
	switch (_mode)
	{
		case WINDOW_MODE_FULLSCREEN_EXCLUSIVE:
		{
			return WS_EX_APPWINDOW;
		}
		case WINDOW_MODE_FULLSCREEN:
		{
			return WS_EX_APPWINDOW;
		}
		case WINDOW_MODE_WINDOW:
		{
			return styleExGet(_features);
		}
		case WINDOW_MODE_TOOL:
		{
			return WS_EX_TOOLWINDOW;
		}
		default:
		{
			return styleExGet(_features);
		}
	}
}

recti32_t windowRectCalculate(const windowData_t* _windowData, const displayData_t* _display, const RECT* _windowRect)
{
	if (_windowData->m_mode == WINDOW_MODE_WINDOW)
	{
		// NOTE(Victor): Check if outside of the screen if app is in windowed mode, to place it correclty inside the screen borders
		const int dstx = _display->m_x + _windowData->m_x < 0 ? (_display->m_width - _windowData->m_width) / 2 : _windowData->m_x;
		const int dsty = _display->m_y + _windowData->m_y < 0 ? (_display->m_height - _windowData->m_height) / 2 : _windowData->m_y;
		const int dstw = _windowRect->right - _windowRect->left;
		const int dsth = _windowRect->bottom - _windowRect->top;

		return (recti32_t)
		{
			.m_x = dstx,
			.m_y = dsty,
			.m_width = dstw,
			.m_height = dsth,
		};
	}
	else if (_windowData->m_mode == WINDOW_MODE_FULLSCREEN)
	{
		const int dstx = _display->m_x;
		const int dsty = _display->m_y;
		const int dstw = _display->m_width;
		const int dsth = _display->m_height;

		return (recti32_t)
		{
			.m_x = dstx,
			.m_y = dsty,
			.m_width = dstw,
			.m_height = dsth,
		};
	}
	else
	{
		const int dstx = _display->m_x;
		const int dsty = _display->m_y;
		const int dstw = _windowData->m_width;
		const int dsth = _windowData->m_height;

		return (recti32_t)
		{
			.m_x = dstx,
			.m_y = dsty,
			.m_width = dstw,
			.m_height = dsth,
		};
	}
}

void windowCreate(displayDataArray_t* _displays, windowData_t* _windowData, const bool _appInitialized, void* _ownerHandle)
{
	const wchar_t* windowClassName = L"Window";
	HINSTANCE inst = GetModuleHandle(NULL);

	// Register the window class on first call
	if (_appInitialized == false)
	{
		RegisterWindowClass(inst, windowClassName);
	}

	const int displayIndex = _windowData->m_displayIndex;

	if (_windowData->m_mode == WINDOW_MODE_FULLSCREEN_EXCLUSIVE) 
	{
		windowDisplayModeSet(_displays, displayIndex, _windowData);
	}

	const DWORD dwStyle = dwStyleGet(_windowData->m_mode, _windowData->m_windowFeatures);
	const DWORD dwExStyle = dwStyleExGet(_windowData->m_mode, _windowData->m_windowFeatures);

	displayData_t* display = &_displays->m_data[displayIndex];

	RECT windowRect;
	windowRect.left = 0;
	windowRect.right = _windowData->m_width;
	windowRect.top = 0;
	windowRect.bottom = _windowData->m_height;
	AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);
	
	const recti32_t finalRect = windowRectCalculate(_windowData, display, &windowRect);

	_windowData->m_x = finalRect.m_x;
	_windowData->m_y = finalRect.m_y;
	_windowData->m_width = finalRect.m_width;
	_windowData->m_height = finalRect.m_height;

	// Create the window
	{
		char classNameUTF8[256] = { 0 };
		WideCharToMultiByte(CP_UTF8, 0, windowClassName, -1, classNameUTF8, 256 - 1, NULL, NULL);

		_windowData->m_handle = CreateWindowExA(dwExStyle, classNameUTF8, _windowData->m_title, dwStyle, finalRect.m_x, finalRect.m_y, finalRect.m_width, finalRect.m_height, _ownerHandle, NULL, inst, NULL);
	}
	
	SetCursor(LoadCursor(0, IDC_ARROW));

	// Top bar in dark mode
	{
		DWORD value = 1;
		DwmSetWindowAttribute(_windowData->m_handle, 20, &value, sizeof(value));
	}
}

void windowShow(windowData_t* _window)
{
	ShowWindow((HWND) _window->m_handle, SW_SHOWDEFAULT);
	UpdateWindow((HWND) _window->m_handle);
}

void windowHide(windowData_t* _window)
{
	ShowWindow(_window->m_handle, SW_HIDE);
	UpdateWindow(_window->m_handle);
}