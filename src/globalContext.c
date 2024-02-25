#pragma once

// Raylib
#include "../libs/raylib/raylib.h"
#define RLIGHTS_IMPLEMENTATION
#include "../libs/raylib/rlights.h"

typedef enum
{
    GIZMO_NONE = 0,
    GIZMO_TRANSFORM_X,
    GIZMO_TRANSFORM_Y,
    GIZMO_TRANSFORM_Z,
} gizmo_t;

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
    RenderTexture m_renderTextureForeground;
    Model meshGround;
    Model meshCube;
    Model churchMesh;
    Texture2D churchTextureDiffuse;

    // Graveyard
    Model m_graveyardTreeDecoratedMesh;
    Model m_graveyardPineLargeMesh;
    Model m_graveyardArchGateMesh;

    Model* meshSelected;
    BoundingBox bbxSelected;

    Light lights[12];

    float m_gamma;
    uint32_t m_gammaLoc;

    float m_ambient;
    uint32_t m_ambientLoc;

    uint16_t m_outlineSize;
    gizmo_t m_gizmoSelected;
    bool m_bbxChecked : 1;
} globalContext_t;