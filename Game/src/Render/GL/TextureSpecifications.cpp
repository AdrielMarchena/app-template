#include "pch.h"
#include "TextureSpecifications.h"

#include "Core/Assert.h"
#include "glad/glad.h"

namespace Game
{
	uint32_t GLSwitch::TextureFilter(GL_TextureFilter filter_type)
	{
		switch (filter_type)
		{
		case GL_TextureFilter::LINEAR:
			return GL_LINEAR;
		case GL_TextureFilter::NEAREST:
			return GL_NEAREST;
		case GL_TextureFilter::LINEAR_MIPMAP_LINEAR:
			return GL_LINEAR_MIPMAP_LINEAR;
		case GL_TextureFilter::NEAREST_MIPMAP_NEAREST:
			return GL_NEAREST_MIPMAP_NEAREST;
		}
		return 0;
	}
	uint32_t GLSwitch::TextureWrap(GL_TextureWrap wrap_type)
	{
		switch (wrap_type)
		{
		case GL_TextureWrap::REPEAT:
			return GL_REPEAT;
		case GL_TextureWrap::MIRROR_REPEAT:
			return GL_MIRRORED_REPEAT;
		case GL_TextureWrap::CLAMP_EDGE:
			return GL_CLAMP_TO_EDGE;
		case GL_TextureWrap::CLAMP_BORDER:
			return GL_CLAMP_TO_BORDER;
		default:
			GAME_CORE_ASSERT(false, "Unknow Texture Wrap");
			break;
		}
		return 0;
	}
	uint32_t GLSwitch::TextureTarget(GL_TextureTarget wrap_type)
	{
		switch (wrap_type)
		{
		case GL_TextureTarget::TEXTURE_2D:
			return GL_TEXTURE_2D;
		case GL_TextureTarget::TEXTURE_3D:
			return GL_TEXTURE_3D;
		default:
			GAME_CORE_ASSERT(false, "Unknow Texture Target");
			break;
		}
		return 0;
	}

	uint32_t GLSwitch::TextureType(GL_TexType type)
	{
		switch (type)
		{
		case GL_TexType::UNSIGNED_INT:
			return GL_UNSIGNED_INT;
		case GL_TexType::FLOAT:
			return GL_FLOAT;
		case GL_TexType::BYTE:
			return GL_BYTE;
		case GL_TexType::NONE:
		case GL_TexType::UNSIGNED_BYTE:
			return GL_UNSIGNED_BYTE;
		default:
			GAME_CORE_ASSERT(false, "Unknow Texture Type");
			break;
		}
		return 0;
	}

}