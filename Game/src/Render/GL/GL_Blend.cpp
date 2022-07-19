#include "pch.h"
#include "GL_Blend.h"
#include "Core/Assert.h"

#include "glad/glad.h"

namespace Game
{
	unsigned int GLBlend::GetGLBlend(GLBlendFactor factor)
	{
		using t = GLBlendFactor;
		switch (factor)
		{
		case t::ZERO: return GL_ZERO;
		case t::ONE: return GL_ONE;
		case t::SRC_COLOR: return GL_SRC_COLOR;
		case t::ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
		case t::DST_COLOR: return GL_DST_COLOR;
		case t::ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
		case t::SRC_ALPHA: return GL_SRC_ALPHA;
		case t::ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
		case t::DST_ALPHA: return GL_DST_ALPHA;
		case t::ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
		case t::CONSTANT_COLOR: return GL_CONSTANT_COLOR;
		case t::ONE_MINUS_CONSTANT_COLOR: return GL_ONE_MINUS_CONSTANT_COLOR;
		case t::CONSTANT_ALPHA: return GL_CONSTANT_ALPHA;
		case t::ONE_MINUS_CONSTANT_ALPHA: return GL_ONE_MINUS_CONSTANT_ALPHA;
		case t::SRC_ALPHA_SATURATE: return GL_SRC_ALPHA_SATURATE;
		case t::SRC1_COLOR: return GL_SRC1_COLOR;
		case t::ONE_MINUS_SRC1_COLOR: return GL_ONE_MINUS_SRC1_COLOR;
		case t::SRC1_ALPHA: return GL_SRC1_ALPHA;
		case t::ONE_MINUS_SRC1_ALPHA: return GL_ONE_MINUS_SRC1_ALPHA;
		default:
			GAME_CORE_ASSERT(false, "Gl blend factor not supported");
			break;
		}
	}
}