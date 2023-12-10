#pragma once

#include <stddef.h>
#include <Windows.h>
#include <WinUser.h>
#include <windef.h>
#include <DbgHelp.h>

#include "../../../utils/recti32_t.c"
#include "../../../window.c"
#include "display.c"

LRESULT WINAPI windowsMessageProcedure(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	//int windowId;
	//DWORD pointerId;
	//POINTER_INFO pointerInfo = {0};
	//POINTER_PEN_INFO penInfo = {0};
	
	switch (_msg) 
	{
		case WM_NCCREATE:
		{

			break;
		}
		case /*WM_DPICHANGED*/ 0x02E0: 
		{
			break;
		}
	}

	return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
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

void windowCreate(displayDataArray_t* _displays, windowData_t* _windowData, const bool _appInitialized)
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
		_windowData->m_handle = CreateWindowExA(dwExStyle, classNameUTF8, _windowData->m_title, dwStyle, finalRect.m_x, finalRect.m_y, finalRect.m_width, finalRect.m_height, NULL, NULL, inst, NULL);
	}
	
	SetCursor(LoadCursor(0, IDC_ARROW));
}

void windowShow(windowData_t* _window)
{
	ShowWindow((HWND) _window->m_handle, SW_SHOWDEFAULT);
	UpdateWindow((HWND) _window->m_handle);
}