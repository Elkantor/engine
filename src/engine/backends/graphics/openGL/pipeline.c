#pragma once

#include <string.h>
#include "../../../pipeline.c"

typedef struct pipeline
{
    cullMode_t m_cullMode;
    compareMode_t m_depthMode;
} piepline_t;

void pipelineInit(pipeline_t* _pipeline)
{
    memset(_pipeline, 0, sizeof(*_pipeline));
}