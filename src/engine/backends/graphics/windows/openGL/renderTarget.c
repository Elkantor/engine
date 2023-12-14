#pragma once

#include <stdbool.h>
#include "../../../../renderTarget.c"

typedef struct
{
    renderTarget_t m_globalData;

    // implementation data
    unsigned m_framebuffer;
	unsigned m_texture;
	unsigned m_depthTexture;
	int m_format;
	bool m_hasDepth;
} renderTargetImpl_t;