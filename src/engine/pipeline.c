#pragma once

typedef enum
{
	BLEND_ONE,
	BLEND_ZERO,
	BLEND_SOURCE_ALPHA,
	BLEND_DEST_ALPHA,
	BLEND_INV_SOURCE_ALPHA,
	BLEND_INV_DEST_ALPHA,
	BLEND_SOURCE_COLOR,
	BLEND_DEST_COLOR,
	BLEND_INV_SOURCE_COLOR,
	BLEND_INV_DEST_COLOR,
	BLEND_CONSTANT,
	BLEND_INV_CONSTANT
} blendingFactor_t;

typedef enum
{
	BLENDOP_ADD,
	BLENDOP_SUBTRACT,
	BLENDOP_REVERSE_SUBTRACT,
	BLENDOP_MIN,
	BLENDOP_MAX
} blendingOperation_t;

typedef enum
{
	COMPARE_ALWAYS,
	COMPARE_NEVER,
	COMPARE_EQUAL,
	COMPARE_NOT_EQUAL,
	COMPARE_LESS,
	COMPARE_LESS_EQUAL,
	COMPARE_GREATER,
	COMPARE_GREATER_EQUAL
} compareMode_t;

typedef enum
{
    CULL_CLOCKWISE,
    CULL_COUNTER_CLOCKWISE,
    CULL_NOTHING 
} cullMode_t;

typedef enum
{
	STENCIL_KEEP,
	STENCIL_ZERO,
	STENCIL_REPLACE,
	STENCIL_INCREMENT,
	STENCIL_INCREMENT_WRAP,
	STENCIL_DECREMENT,
	STENCIL_DECREMENT_WRAP,
	STENCIL_INVERT
} stencilAction_t;

typedef struct pipeline pipeline_t;

void pipelineInit(pipeline_t* _pipeline);