#pragma once

#include <Windows.h>

int kickstart(int _argc, char** _argv);

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nCmdShow)
{
	int ret = kickstart(__argc, __argv);
	if (ret != 0)
    {
#ifdef NDEBUG
		MessageBox(0, L"Unknown Error", L"Error", MB_OK);
#else
		__debugbreak();
#endif
	}

	return ret;
}

