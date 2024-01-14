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
    Shader outlineShader;
    Shader gizmoShader;
    Material materialFlatColor;
    uint32_t shaderFlatColorLoc;

    RenderTexture renderTexture;
    Model meshGround;
    Model meshCube;
    Model churchMesh;

    Model* meshSelected;
    BoundingBox bbxSelected;
    Texture2D churchTextureDiffuse;

    Light lights[12];

    float m_gamma;
    int m_outlineSize;
} globalContext_t;