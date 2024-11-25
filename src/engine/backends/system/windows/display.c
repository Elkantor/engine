#pragma once

#include <Windows.h>
#include <libloaderapi.h>
#include <stdbool.h>
#include <wingdi.h>
#include <stddef.h>

#include "../../../display.c"

uint32_t displayPrimaryGet(displayDataArray_t* _displays)
{
	for (uint32_t i = 0; i < _displays->m_size; ++i)
    {
		displayData_t* display = &_displays->m_data[i];

		if (display->m_available && display->m_primary)
        {
			return i;
		}
	}

	return UINT32_MAX;
}

typedef enum
{ 
    MDT_EFFECTIVE_DPI = 0, 
    MDT_ANGULAR_DPI = 1, 
    MDT_RAW_DPI = 2, 
    MDT_DEFAULT = MDT_EFFECTIVE_DPI 
} MONITOR_DPI_TYPE;

typedef HRESULT(WINAPI *getDpiForMonitor_t)(HMONITOR _hmonitor, MONITOR_DPI_TYPE _dpiType, UINT* _dpiX, UINT* _dpiY);

typedef struct
{
    displayDataArray_t* m_displaysPtr;
    getDpiForMonitor_t m_getDpiForMonitorPtr;
} EnumDisplayMonitorsCallbackParams_t;

BOOL CALLBACK EnumerationCallback(HMONITOR _monitor, HDC _hdcUnused, LPRECT _rectUnused, LPARAM _lparam)
{
	MONITORINFOEXA info;
	memset(&info, 0, sizeof(MONITORINFOEXA));
	info.cbSize = sizeof(MONITORINFOEXA);

	if (GetMonitorInfoA(_monitor, (MONITORINFO *)&info) == false)
    {
		return false;
	}

    EnumDisplayMonitorsCallbackParams_t* params = (EnumDisplayMonitorsCallbackParams_t*)_lparam;
    displayDataArray_t* displays = params->m_displaysPtr;

    const uint32_t freeSlot = displays->m_size;
	displayData_t* current = &displays->m_data[freeSlot];

    // Get common display data
    {
        const uint32_t string32Capacity = string32CapacityGet();
        strncpy(current->m_name.m_data, info.szDevice, string32Capacity);
        current->m_name.m_data[string32Capacity - 1] = 0;
        current->m_index = freeSlot;
        current->m_monitorHandle = _monitor;
        current->m_primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
        current->m_available = true;
        current->m_x = info.rcMonitor.left;
        current->m_y = info.rcMonitor.top;
        current->m_width = (u32)(info.rcMonitor.right - info.rcMonitor.left);
        current->m_height = (u32)(info.rcMonitor.bottom - info.rcMonitor.top);

        HDC hdc = CreateDCA(NULL, current->m_name.m_data, NULL, NULL);
        current->m_ppi = GetDeviceCaps(hdc, LOGPIXELSX);
        DeleteDC(hdc);
    }

    // Get dpi for this monitor
    {
        getDpiForMonitor_t getDpiForMonitor = params->m_getDpiForMonitorPtr;
        
        if (getDpiForMonitor != NULL) 
        {
            unsigned dpiX, dpiY;
            getDpiForMonitor(_monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
            current->m_ppi = (int)dpiX;
        }
    }

    // Get bpp and frequency for this monitor
    {
        DEVMODEA devmodeA;
        memset(&devmodeA, 0, sizeof(DEVMODEA));
	    
        devmodeA.dmSize = sizeof(DEVMODEA);
	    EnumDisplaySettingsA(current->m_name.m_data, ENUM_CURRENT_SETTINGS, &devmodeA);
	    current->m_frequency = devmodeA.dmDisplayFrequency;
	    current->m_bpp = devmodeA.dmBitsPerPel;
    }

	displays->m_size += 1;
	return TRUE;
}

void displayInit(displayDataArray_t* _displays)
{
	HMODULE shcore = LoadLibraryA("Shcore.dll");
	getDpiForMonitor_t getDpiForMonitorPtr = NULL;

    if (shcore != NULL) 
    {
		getDpiForMonitorPtr = (getDpiForMonitor_t)GetProcAddress(shcore, "GetDpiForMonitor");
	}

	memset(_displays, 0, sizeof(*_displays));

    EnumDisplayMonitorsCallbackParams_t params;
    params.m_displaysPtr = _displays;
    params.m_getDpiForMonitorPtr = getDpiForMonitorPtr;

	EnumDisplayMonitors(NULL, NULL, EnumerationCallback, (LPARAM)&params);

    FreeLibrary(shcore);
}

void displayRestore(displayDataArray_t* _displays)
{
    for (uint32_t i = 0; i < _displays->m_size; ++i)
    {
        displayData_t* current = &_displays->m_data[i];

        if (current->m_modeChanged)
        {
            // NOTE(Victor): Getting back to default by passing NULL and 0, see https://learn.microsoft.com/fr-fr/windows/win32/api/winuser/nf-winuser-changedisplaysettingsa
            ChangeDisplaySettingsA(NULL, 0);
        }
    }
}