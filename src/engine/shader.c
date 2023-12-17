#pragma once

typedef enum
{
	SHADER_TYPE_FRAGMENT = 0,
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_COMPUTE,
	SHADER_TYPE_GEOMETRY,
	SHADER_TYPE_TESSELLATION_CONTROL,
	SHADER_TYPE_TESSELLATION_EVALUATION,
	SHADER_TYPE_COUNT
} shaderType_t;

typedef struct shader shader_t;

void shaderInit(shader_t* _shader, const void* _data, const size_t _length, const shaderType_t _type);