#pragma once

#include <stdbool.h>
#include <assert.h>
#include "glDebug.c"
#include "../../../renderTarget.c"

typedef struct renderTargetImpl
{
    renderTarget_t m_globalData;

    // implementation data
    unsigned m_framebuffer;
	unsigned m_texture;
	unsigned m_depthTexture;
	int m_format;
	bool m_hasDepth;
} renderTargetImpl_t;

void renderTargetInit(renderTargetImpl_t* _renderTarget, const int _width, const int _height, const int _depthBits, const int _stencilBits, const int _samplesPerPixel, const renderTargetFormat_t _format)
{
	_renderTarget->m_globalData.m_width = _width;
	_renderTarget->m_globalData.m_height = _height;
	_renderTarget->m_globalData.m_texWidth = _width;
	_renderTarget->m_globalData.m_texHeight = _height;
	_renderTarget->m_format = (int)_format;

	glGenTextures(1, &_renderTarget->m_texture);
	assert(glDebugErrorCheck() == false);

	glBindTexture(GL_TEXTURE_2D, _renderTarget->m_texture);
	assert(glDebugErrorCheck() == false);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	assert(glDebugErrorCheck() == false);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	assert(glDebugErrorCheck() == false);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	assert(glDebugErrorCheck() == false);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	assert(glDebugErrorCheck() == false);

	const int texWidth = _renderTarget->m_globalData.m_texWidth;
	const int texHeight = _renderTarget->m_globalData.m_texHeight;

	switch (_format)
	{
		case RENDER_TARGET_FORMAT_128BIT_FLOAT:
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texWidth, texHeight, 0, GL_RGBA, GL_FLOAT, 0);
			assert(glDebugErrorCheck() == false);
			break;
		}
		case RENDER_TARGET_FORMAT_64BIT_FLOAT:
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texWidth, texHeight, 0, GL_RGBA, GL_HALF_FLOAT, 0);
			assert(glDebugErrorCheck() == false);
			break;
		}
		case RENDER_TARGET_FORMAT_16BIT_DEPTH:
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, texWidth, texHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
			assert(glDebugErrorCheck() == false);
			break;
		}
		case RENDER_TARGET_FORMAT_8BIT_RED:
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texWidth, texHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
			assert(glDebugErrorCheck() == false);
			break;
		}
		case RENDER_TARGET_FORMAT_16BIT_RED_FLOAT:
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F_EXT, texWidth, texHeight, 0, GL_RED, GL_HALF_FLOAT, 0);
			assert(glDebugErrorCheck() == false);
			break;
		}
		case RENDER_TARGET_FORMAT_32BIT_RED_FLOAT:
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F_EXT, texWidth, texHeight, 0, GL_RED, GL_FLOAT, 0);
			assert(glDebugErrorCheck() == false);
			break;
		}
		case RENDER_TARGET_FORMAT_32BIT:
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			assert(glDebugErrorCheck() == false);
			break;
		}
		default:
		{
			// NOTE(Victor): Not implemented yet
			assert(false);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		}
	}

	glGenFramebuffers(1, & _renderTarget->m_framebuffer);
	assert(glDebugErrorCheck() == false);

	glBindFramebuffer(GL_FRAMEBUFFER, _renderTarget->m_framebuffer);
	assert(glDebugErrorCheck() == false);

	//setupDepthStencil(_renderTarget, GL_TEXTURE_2D, _depthBits, _stencilBits, texWidth, texHeight);


	if (_format == RENDER_TARGET_FORMAT_16BIT_DEPTH)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _renderTarget->m_texture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		assert(glDebugErrorCheck() == false);
	}
	else
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderTarget->m_texture, 0);
		assert(glDebugErrorCheck() == false);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	assert(glDebugErrorCheck() == false);
	glBindTexture(GL_TEXTURE_2D, 0);
	assert(glDebugErrorCheck() == false);
}