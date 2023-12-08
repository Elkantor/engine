#pragma once

#include <Windows.h>
#include <stdbool.h>
#include <stdint.h>

#include "../../../display.c"

typedef struct
{
	struct HMONITOR__* m_monitor;
	char m_name[32];
	int m_index;
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    int m_ppi;
    int m_frequency;
    int m_bpp;
	bool m_primary;
    bool m_available;
    bool m_modeChanged;
} displayData_t;

typedef struct
{
    displayData_t m_elems[8];
    size_t m_size;
} displayDataArray_t;

size_t displaysCapacityGet(void)
{
    return offsetof(displayDataArray_t, m_size) / sizeof(displayData_t);
}

typedef enum
{ 
    MDT_EFFECTIVE_DPI = 0, 
    MDT_ANGULAR_DPI = 1, 
    MDT_RAW_DPI = 2, 
    MDT_DEFAULT = MDT_EFFECTIVE_DPI 
} MONITOR_DPI_TYPE;

typedef HRESULT(WINAPI *GetDpiForMonitorType)(HMONITOR _hmonitor, MONITOR_DPI_TYPE _dpiType, UINT* _dpiX, UINT* _dpiY);

BOOL CALLBACK EnumerationCallback(HMONITOR _monitor, HDC _hdcUnused, LPRECT _rectUnused, LPARAM _lparam)
{
	MONITORINFOEXA info;
	memset(&info, 0, sizeof(MONITORINFOEXA));
	info.cbSize = sizeof(MONITORINFOEXA);

	if (GetMonitorInfoA(_monitor, (MONITORINFO *)&info) == false)
    {
		return false;
	}

    displayDataArray_t* displays = (displayDataArray_t*)_lparam;

    size_t freeSlot = displays->m_size;
	displayData_t* current = &displays->m_elems[freeSlot];

	strncpy(current->m_name, info.szDevice, 31);
	current->m_name[31] = 0;
	current->m_index = freeSlot;
	current->m_monitor = _monitor;
	current->m_primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
	current->m_available = true;
	current->m_x = info.rcMonitor.left;
	current->m_y = info.rcMonitor.top;
	current->m_width = info.rcMonitor.right - info.rcMonitor.left;
	current->m_height = info.rcMonitor.bottom - info.rcMonitor.top;

	HDC hdc = CreateDCA(NULL, current->m_name, NULL, NULL);
	current->m_ppi = GetDeviceCaps(hdc, LOGPIXELSX);
	int scale = GetDeviceCaps(hdc, SCALINGFACTORX);
	DeleteDC(hdc);

	// if (MyGetDpiForMonitor != NULL) 
    // {
	// 	unsigned dpiX, dpiY;
	// 	MyGetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
	// 	display->ppi = (int)dpiX;
	// }

	// memset(&original_modes[free_slot], 0, sizeof(DEVMODEA));
	// original_modes[free_slot].dmSize = sizeof(DEVMODEA);
	//EnumDisplaySettingsA(current->m_name, ENUM_CURRENT_SETTINGS, &original_modes[free_slot]);
	//display->frequency = original_modes[free_slot].dmDisplayFrequency;
	//display->bpp = original_modes[free_slot].dmBitsPerPel;

	displays->m_size += 1;
	return TRUE;
}

void displayInit(displayDataArray_t* _displays) 
{
	HMODULE shcore = LoadLibraryA("Shcore.dll");
	
    if (shcore != NULL) 
    {
		GetDpiForMonitorType MyGetDpiForMonitor = (GetDpiForMonitorType)GetProcAddress(shcore, "GetDpiForMonitor");
	}
	memset(_displays, 0, sizeof(*_displays));
	EnumDisplayMonitors(NULL, NULL, EnumerationCallback, (LPARAM)_displays);
}