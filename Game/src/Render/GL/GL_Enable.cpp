#include "pch.h"
#include "GL_Enable.h"
#include "Core/Assert.h"

#include "glad/glad.h"

namespace Game
{
	unsigned int Game::GLEnable::GetGlEnableCaps(GLEnableCaps cap)
	{
		using t = GLEnableCaps;
		switch (cap)
		{
			case t::BLEND: return GL_BLEND;
			// case t::CLIP_DISTANCE: return GL_CLIP_DISTANCE;
			case t::COLOR_LOGIC_OP: return GL_COLOR_LOGIC_OP;
			case t::CULL_FACE: return GL_CULL_FACE;
			// case t::DEBUG_OUTPUT: return GL_DEBUG_OUTPUT;
			// case t::DEBUG_OUTPUT_SYNCHRONOUS: return GL_DEBUG_OUTPUT_SYNCHRONOUS;
			case t::DEPTH_CLAMP: return GL_DEPTH_CLAMP;
			case t::DEPTH_TEST: return GL_DEPTH_TEST;
			case t::DITHER: return GL_DITHER;
			case t::FRAMEBUFFER_SRGB: return GL_FRAMEBUFFER_SRGB;
			case t::LINE_SMOOTH: return GL_LINE_SMOOTH;
			case t::MULTISAMPLE: return GL_MULTISAMPLE;
			case t::POLYGON_OFFSET_FILL: return GL_POLYGON_OFFSET_FILL;
			case t::POLYGON_OFFSET_LINE: return GL_POLYGON_OFFSET_LINE;
			case t::POLYGON_OFFSET_POINT: return GL_POLYGON_OFFSET_POINT;
			case t::POLYGON_SMOOTH: return GL_POLYGON_SMOOTH;
			case t::PRIMITIVE_RESTART: return GL_PRIMITIVE_RESTART;
			// case t::PRIMITIVE_RESTART_FIXED_INDEX: return GL_PRIMITIVE_RESTART_FIXED_INDEX;
			case t::RASTERIZER_DISCARD: return GL_RASTERIZER_DISCARD;
			case t::SAMPLE_ALPHA_TO_COVERAGE: return GL_SAMPLE_ALPHA_TO_COVERAGE;
			case t::SAMPLE_ALPHA_TO_ONE: return GL_SAMPLE_ALPHA_TO_ONE;
			case t::SAMPLE_COVERAGE: return GL_SAMPLE_COVERAGE;
			// case t::SAMPLE_SHADING: return GL_SAMPLE_SHADING;
			case t::SAMPLE_MASK: return GL_SAMPLE_MASK;
			case t::SCISSOR_TEST: return GL_SCISSOR_TEST;
			case t::STENCIL_TEST: return GL_STENCIL_TEST;
			case t::TEXTURE_CUBE_MAP_SEAMLESS: return GL_TEXTURE_CUBE_MAP_SEAMLESS;
			case t::PROGRAM_POINT_SIZE: return GL_PROGRAM_POINT_SIZE;
			default:
				GAME_CORE_ASSERT(false, "Gl enable cap not supported");
				break;
		}
	}
}
