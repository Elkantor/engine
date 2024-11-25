#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Compile with: zig cc src/test.c -g -Wall -Werror -Wshadow -Wconversion -lDwmapi -lwinmm -lgdi32 -lopengl32 -lraylib -L./libs/raylib -I./src -I./libs

#include "engine/moduleHelper.c"
#include "engine/utils/memory/memoryStack.c"
#include "engine/utils/types.c"

#include "raylib/raylib.h"
#include "raylib/rlgl.h"
#include "raylib/raymath.h"
#include "game/world.c"

#define RAYGUI_IMPLEMENTATION
#include "raylib/raygui.h"
#include "raylib/raygui_style.h"
#include <dwmapi.h>


memoryStackStruct stackScratch;

static void DrawContent(Vector2 position, Vector2 _scroll, WorldStruct* _world)
{
	char buffer[10] = { 0 };
	
	snprintf(buffer, 10, "%f", _world->m_uniforms[UNIFORM_RUNTIME].m_value);
	if (GuiSlider((RL_Rectangle) { position.x + 20 + _scroll.x, position.y + 50 + _scroll.y, 150, 25 }, buffer, "Light Pos", &_world->m_uniforms[UNIFORM_RUNTIME].m_value, -10.f, 10.f))
	{
		SetShaderValue(_world->m_shader, _world->m_uniforms[UNIFORM_RUNTIME].m_loc, &_world->m_uniforms[UNIFORM_RUNTIME].m_value, SHADER_UNIFORM_FLOAT);
	}

	snprintf(buffer, 10, "%f", _world->m_uniforms[UNIFORM_FOG].m_value);
	if (GuiSliderBar((RL_Rectangle) { position.x + 20 + _scroll.x, position.y + 100 + _scroll.y, 150, 25 }, buffer, "Fog Percentage", &_world->m_uniforms[UNIFORM_FOG].m_value, 0.f, 1.f))
	{
		SetShaderValue(_world->m_shader, _world->m_uniforms[UNIFORM_FOG].m_loc, &_world->m_uniforms[UNIFORM_FOG].m_value, SHADER_UNIFORM_FLOAT);
	}
   /* GuiButton((RL_Rectangle) { position.x + 20 + _scroll.x, position.y + 50  + _scroll.y, 100, 25 }, "Button 1");
    GuiButton((RL_Rectangle) { position.x + 20 + _scroll.x, position.y + 100 + _scroll.y, 100, 25 }, "Button 2");
    GuiButton((RL_Rectangle) { position.x + 20 + _scroll.x, position.y + 150 + _scroll.y, 100, 25 }, "Button 3");
    GuiLabel((RL_Rectangle) { position.x + 20 + _scroll.x, position.y + 200 + _scroll.y, 250, 25 }, "A Label");
    GuiLabel((RL_Rectangle) { position.x + 20 + _scroll.x, position.y + 250 + _scroll.y, 250, 25 }, "Another Label");
    GuiLabel((RL_Rectangle) { position.x + 20 + _scroll.x, position.y + 300 + _scroll.y, 250, 25 }, "Yet Another Label");*/
}

i32 main(void)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	defer(InitWindow(1280.f, 720, "Raylib Window 2"), RL_CloseWindow()) {
		GuiLoadStyleDark();

		// NOTE(VictorGallet): Window caption bar dark mode with custom properties
		{
			const HWND windowHandle = GetWindowHandle();

			u32 value = 1;
			DwmSetWindowAttribute(windowHandle, 20, &value, sizeof(value));

			u32 corner = 3; // DWMWCP_ROUNDSMALL
			const u32 cornerPreference = 33; // DWMWA_WINDOW_CORNER_PREFERENCE
			DwmSetWindowAttribute(windowHandle, cornerPreference, &corner, sizeof(corner));

			COLORREF borderColor = 0x00787878;
			const u32 borderColorPreference = 34; // DWMWA_BORDER_COLOR
			DwmSetWindowAttribute(windowHandle, borderColorPreference, &borderColor, sizeof(borderColor));

			COLORREF textColor = 0x00B4B4B4;
			const u32 textColorPreference = 36; //DWMWA_TEXT_COLOR
			DwmSetWindowAttribute(windowHandle, textColorPreference, &textColor, sizeof(textColor));

			unsigned long style = WS_POPUP | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SIZEBOX | WS_CAPTION;
			SetWindowLongPtr(windowHandle, GWL_STYLE, style);
			ShowWindow(windowHandle, SW_SHOW);
		}

		memset(&stackScratch, 0, sizeof(stackScratch));

		scopeVar(Vector2* playerPos = memoryStack.push(&stackScratch, sizeof(Vector2)), 
			playerPos->y = -1.f;

			scopeVar(WorldStruct* myWorld = memoryStack.push(&stackScratch, sizeof(WorldStruct)),
				world.init(myWorld);

				scopeVar(Camera* camera = memoryStack.push(&stackScratch, sizeof(Camera)),
					camera->position = (Vector3){ 0.f, 8.f, 8.f };						// Camera position
					camera->target = (Vector3){ playerPos->x, 2.f, playerPos->y };      // Camera looking at point
					camera->up = (Vector3){ 0.0f, 1.0f, 0.0f };							// Camera up vector (rotation towards target)
					camera->fovy = 80.0f;												// Camera field-of-view Y
					camera->projection = CAMERA_PERSPECTIVE;							// Camera projection type

					deferVar(myWorld->m_shader = LoadShader(0, "res/shaders/raymarching.fs"), UnloadShader(myWorld->m_shader)) {
						// Get shader locations for required uniforms
						i32 viewEyeLoc = GetShaderLocation(myWorld->m_shader, "viewEye");
						i32 viewCenterLoc = GetShaderLocation(myWorld->m_shader, "viewCenter");
						i32 resolutionLoc = GetShaderLocation(myWorld->m_shader, "resolution");
						i32 cubePositionLoc = GetShaderLocation(myWorld->m_shader, "cubePosition");
						
						myWorld->m_uniforms[UNIFORM_FOG].m_loc 		= GetShaderLocation(myWorld->m_shader, "fogPercentage");
						myWorld->m_uniforms[UNIFORM_RUNTIME].m_loc 	= GetShaderLocation(myWorld->m_shader, "runTime");

						const f32 resolution[2] = { (f32)GetScreenWidth(), (f32)GetScreenHeight() };
						SetShaderValue(myWorld->m_shader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);

						for (u32 i = 0; i < sizeof(myWorld->m_uniforms) / sizeof(myWorld->m_uniforms[0]); ++i)
						{
							SetShaderValue(myWorld->m_shader, myWorld->m_uniforms[i].m_loc, &myWorld->m_uniforms[i].m_value, SHADER_UNIFORM_FLOAT);
						}

						while (!WindowShouldClose())
						{
							if (IsKeyDown(KEY_RIGHT)) 	{ playerPos->x += 0.01f; }
							if (IsKeyDown(KEY_LEFT)) 	{ playerPos->x -= 0.01f; }
							if (IsKeyDown(KEY_UP)) 		{ playerPos->y -= 0.01f; }
							if (IsKeyDown(KEY_DOWN)) 	{ playerPos->y += 0.01f; }

							const f32 cubePosition[2] = { playerPos->x, playerPos->y };
							SetShaderValue(myWorld->m_shader, cubePositionLoc, cubePosition, SHADER_UNIFORM_VEC2);

							const f32 cameraPos[3] = { playerPos->x, camera->position.y, camera->position.z + playerPos->y };
							SetShaderValue(myWorld->m_shader, viewEyeLoc, cameraPos, SHADER_UNIFORM_VEC3);

							camera->target = (Vector3){ playerPos->x, 2.0f, playerPos->y + 2.f };
							const f32 cameraTarget[3] = { camera->target.x, camera->target.y, camera->target.z };
							SetShaderValue(myWorld->m_shader, viewCenterLoc, cameraTarget, SHADER_UNIFORM_VEC3);

							defer(BeginDrawing(), EndDrawing()) {
								ClearBackground(WHITE);

								defer(BeginShaderMode(myWorld->m_shader), EndShaderMode()) {
									DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
								}

								GuiWindowFloating(&window_position, &window_size, &minimized, &moving, &resizing, &DrawContent, (Vector2) { 400, 400 }, &scroll, myWorld, "Movable & Scalable Window");

								DrawFPS(GetScreenWidth() - 120, 10);
							}
						}
					}
				)
			)
		)
	}
	return 0;
}