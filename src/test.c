#include <stdio.h>
#include <Processthreadsapi.h>
#include <Handleapi.h>
#include <Synchapi.h>

// Compile with: zig cc src/test.c -g -Wall -Werror -lwinmm -lgdi32 -lopengl32 -lraylib -L./libs/raylib

// Raylib
#include "../libs/raylib/raylib.h"

static HANDLE s_threadHandle = NULL;
static _Bool s_active = 1;

unsigned long appKickstart(void* _data)
{
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	InitWindow(1280.f * 0.7f, 720, "Raylib Window 2");

	while (!WindowShouldClose() && s_active)
	{
		BeginDrawing();
		{
			ClearBackground(RED);
		}
		EndDrawing();
	}

	RL_CloseWindow();

	return 0;
}


int main(void)
{
#if defined(THREADED)
	{
		s_threadHandle = CreateThread(NULL, 0, appKickstart, NULL, 0, NULL);
	}
#else
	{
		appKickstart(NULL);
	}
#endif

	return 0;
}

void release(void)
{
	s_active = 0;
	WaitForSingleObject(s_threadHandle, 0xFFFFFFFF);
	CloseHandle(s_threadHandle);

	s_threadHandle = NULL;
}