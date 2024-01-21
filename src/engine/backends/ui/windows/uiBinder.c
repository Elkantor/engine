#pragma once

#include "../../../ui/uiBinder.c"
#include <wchar.h>

void uiBinderF32(float* _value, const wchar_t* _func, void* _windowHandle)
{
    MC_HMCALLSCRIPTFUNC csfArgs;
    WCHAR pszRetVal[32] = { 0 };

    wchar_t buff[32] = { 0 };
    swprintf(buff, 32, L"%f", *_value);

    csfArgs.cbSize = sizeof(MC_HMCALLSCRIPTFUNC);
    csfArgs.pszRet = pszRetVal;
    csfArgs.iRet = sizeof(pszRetVal) / sizeof(pszRetVal[0]);
    csfArgs.pszArg1 = buff;
    csfArgs.cArgs = 1;

    SendMessageW(_windowHandle, MC_HM_CALLSCRIPTFUNC, (WPARAM)_func, (LPARAM)&csfArgs);

    if (wcsnlen_s(pszRetVal, sizeof(pszRetVal) / sizeof(pszRetVal[0])) > 0)
    {
        *_value = _wtof(pszRetVal);
        return;
    }
}

void uiBinderI32(int32_t* _value, const wchar_t* _func, void* _windowHandle)
{
    WCHAR pszRetVal[32] = { 0 };

    MC_HMCALLSCRIPTFUNC csfArgs =
    {
        .cbSize = sizeof(MC_HMCALLSCRIPTFUNC),
        .pszRet = pszRetVal,
        .iRet = sizeof(pszRetVal) / sizeof(pszRetVal[0]),
        .cArgs = 1,
        .iArg1 = *_value,
    };

    SendMessageW(_windowHandle, MC_HM_CALLSCRIPTFUNC, (WPARAM)_func, (LPARAM)&csfArgs);

    if (wcsnlen_s(pszRetVal, sizeof(pszRetVal) / sizeof(pszRetVal[0])) > 0)
    {
        *_value = _wtoi(pszRetVal);
        return;
    }
}

void uiBinderVector3(Vector3* _value, const wchar_t* _func, void* _windowHandle)
{
    wchar_t buff1[32] = { 0 };
    swprintf(buff1, 32, L"%f", _value->x);

    wchar_t buff2[32] = { 0 };
    swprintf(buff2, 32, L"%f", _value->y);

    wchar_t buff3[32] = { 0 };
    swprintf(buff3, 32, L"%f", _value->z);

    WCHAR pszRetVal[128];

    MC_HMCALLSCRIPTFUNC csfArgs =
    {
        .cbSize = sizeof(MC_HMCALLSCRIPTFUNC),
        .pszRet = pszRetVal,
        .iRet = sizeof(pszRetVal) / sizeof(pszRetVal[0]),
        .cArgs = 3,
        .pszArg1 = buff1,
        .pszArg2 = buff2,
        .pszArg3 = buff3,
    };

    SendMessageW(_windowHandle, MC_HM_CALLSCRIPTFUNC, (WPARAM)_func, (LPARAM)&csfArgs);

    if (wcsnlen_s(pszRetVal, 128) > 0)
    {
        size_t current = 0;
        float values[3] = { 0 };
        WCHAR* currentAddr = (WCHAR*) pszRetVal;

        for (size_t i = 0; i < 128 || current < 3; ++i)
        {
            if (pszRetVal[i] == ' ')
            {
                pszRetVal[i] = '\0';
            
                values[current] = _wtof(currentAddr);
                current += 1;

                i += 1;
                currentAddr = pszRetVal + i;
            }
            else if (pszRetVal[i] == '\0')
            {
                break;
            }
        }

        _value->x = values[0];
        _value->y = values[1];
        _value->z = values[2];
    }
}