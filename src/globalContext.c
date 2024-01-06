#pragma once

// Raylib
#include "../libs/raylib/raylib.h"
#define RLIGHTS_IMPLEMENTATION
#include "../libs/raylib/rlights.h"

// NOTE(Victor): This struct is passed as parameter to app update function, each frame
typedef struct globalContext
{
    Camera3D camera;
    Vector3 cubePosition;
    Shader mainShader;
    Model meshGround;
    Model meshCube;
    Light lights[12];
} globalContext_t;