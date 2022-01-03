#pragma once
#include <stdint.h>
#include <string_view>
namespace Game
{
	constexpr std::string_view Base_Unamed_Texture = "Unamed_Texture";

	//TODO: Add enum to specify mipmap things (maybe)
	enum class GL_TextureFilter
	{
		LINEAR = 1,
		NEAREST,
		LINEAR_MIPMAP_LINEAR,
		NEAREST_MIPMAP_NEAREST

	};
	enum class GL_TextureWrap
	{
		REPEAT = 1,
		MIRROR_REPEAT,
		CLAMP_EDGE,
		CLAMP_BORDER,
	};

	enum class GL_TextureTarget
	{
		TEXTURE_2D = 1,
		TEXTURE_3D
	};

	enum class GL_TexType
	{
		NONE,
		UNSIGNED_INT = 1,
		BYTE,
		UNSIGNED_BYTE,
		FLOAT
	};

	typedef uint8_t TextureBufferType;
	struct TextureSpecifications
	{
		//Image details
		int32_t Width = 1;
		int32_t Height = 1;
		int32_t Depth = 1;
		int32_t Channels = 4;
		/*
		* if empty, will try get the name from the file
		* Example = "D:\Path\To\Texture.png" the name would be "Texture"
		*/
		std::string Name = ""; //if empty, the name will be the file name without extension
		TextureBufferType* Buffer = nullptr; //Texture Buffer

		//Texture details
		uint32_t UnpackAligment = 1;

		GL_TextureTarget Target = GL_TextureTarget::TEXTURE_2D;
		GL_TextureFilter MinFilter = GL_TextureFilter::NEAREST;
		GL_TextureFilter MagFilter = GL_TextureFilter::NEAREST;
		GL_TextureWrap WrapS = GL_TextureWrap::CLAMP_EDGE;
		GL_TextureWrap WrapT = GL_TextureWrap::CLAMP_EDGE;
		GL_TextureWrap WrapR = GL_TextureWrap::CLAMP_EDGE;
		float TextureLodBias = -1.0f;
		bool GenerateMipMap = true;

		TextureSpecifications(const TextureSpecifications&) = default;
		TextureSpecifications() = default;

		// Buffer options
		bool DeleteSourceBuffer = false; // Set to true to delete the buffer on texture creation
		bool KeepSourceBuffer = true; // Set to true to Keep the pointer to the source buffer
		bool CopySourceBuffer = false; // Set to true to Copy the buffer (memcpy_s)

		uint32_t InternalFormat = 0; //If zero, will guess using Channels value
		uint32_t DataFormat = 0; //If zero, will guess using Channels value
		GL_TexType Type = GL_TexType::UNSIGNED_INT; //If zero, GL_UNSIGNED_BYTE
	};

	class GLSwitch
	{
	public:
		static uint32_t TextureFilter(GL_TextureFilter filter_type);
		static uint32_t TextureWrap(GL_TextureWrap wrap_type);
		static uint32_t TextureTarget(GL_TextureTarget wrap_type);
		static uint32_t TextureType(GL_TexType type);
	};
}