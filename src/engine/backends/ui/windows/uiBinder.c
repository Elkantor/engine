#pragma once

#include "../../../ui/uiBinder.c"

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
}

void uiBinderVector3(Vector3* _value, const wchar_t* _func, void* _windowHandle)
{
    MC_HMCALLSCRIPTFUNC csfArgs;
    WCHAR pszRetVal[128] = { 0 };

    wchar_t buff1[32] = { 0 };
    swprintf(buff1, 32, L"%f", _value->x);

    wchar_t buff2[32] = { 0 };
    swprintf(buff2, 32, L"%f", _value->y);

    wchar_t buff3[32] = { 0 };
    swprintf(buff3, 32, L"%f", _value->z);

    csfArgs.cbSize = sizeof(MC_HMCALLSCRIPTFUNC);
    csfArgs.pszRet = pszRetVal;
    csfArgs.iRet = sizeof(pszRetVal) / sizeof(pszRetVal[0]);
    csfArgs.pszArg1 = buff1;
    csfArgs.pszArg2 = buff2;
    csfArgs.pszArg3 = buff3;
    csfArgs.cArgs = 3;

    SendMessageW(_windowHandle, MC_HM_CALLSCRIPTFUNC, (WPARAM)_func, (LPARAM)&csfArgs);
}