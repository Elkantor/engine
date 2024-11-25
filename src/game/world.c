#pragma once

#include "engine/moduleHelper.c"
#include "engine/utils/types.c"

#undef module
#define module world

typedef struct UniformStruct
{
	f32 m_value;
	i32 m_loc;
} UniformStruct;

typedef enum UniformID
{
	UNIFORM_FOG = 0,
	UNIFORM_RUNTIME,
	UNIFORM_COUNT,
} UniformID;

typedef struct WorldStruct
{
	Shader m_shader;
	UniformStruct m_uniforms[10];
} WorldStruct;

fn(void, init, WorldStruct* _world)
{
	static const UniformStruct uniforms[] =
	{
		[UNIFORM_FOG] 		= { 0.033f,		-1 },
		[UNIFORM_RUNTIME] 	= { 4.17f, 		-1 },
	};

	memcpy_s(_world->m_uniforms, sizeof(_world->m_uniforms), uniforms, sizeof(uniforms)); 
}
end(init)

module_export(
	init
)