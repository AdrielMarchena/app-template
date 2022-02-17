#include "pch.h"
#include "Texture.h"

#include "gl_error_macro_db.h"
#include "glad/glad.h"
#include "Core/Core.h"

#include "stb_image.h"

#include "Utils/Generic.h"

namespace Game
{ 

	Ref<Texture> Texture::m_WhiteTexture;

	Texture::Texture(const TextureSpecifications& info, const std::string& path)
		:m_ImageInfo(info),m_Path(path),m_Name(info.Name)
	{
		GAME_CORE_ASSERT(info.Width > 0 &&
			info.Height > 0 &&
			info.Channels > 0 &&
			info.Channels < 5
		,"Some TextureSpecifications is wrong");

		if (m_ImageInfo.Channels == 4)
		{
			if(!m_ImageInfo.InternalFormat)
				m_ImageInfo.InternalFormat = GL_RGBA8;
			if(!m_ImageInfo.DataFormat)
				m_ImageInfo.DataFormat = GL_RGBA;
		}
		else if (m_ImageInfo.Channels == 3)
		{
			if (!m_ImageInfo.InternalFormat)
				m_ImageInfo.InternalFormat = GL_RGB8;
			if (!m_ImageInfo.DataFormat)
				m_ImageInfo.DataFormat = GL_RGB;
		}
		else if (m_ImageInfo.Channels == 1)
		{
			if (!m_ImageInfo.InternalFormat)
				m_ImageInfo.InternalFormat = GL_RED;
			if (!m_ImageInfo.DataFormat)
				m_ImageInfo.DataFormat = GL_RED;
		}
		
		GAME_CORE_ASSERT(m_ImageInfo.InternalFormat & m_ImageInfo.DataFormat, "Format not supported");

		GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, info.UnpackAligment));
		GLCall(glGenTextures(1, &m_Id));
		GAME_CORE_ASSERT(m_Id, "Texture Id is 0");

		uint32_t target = GLSwitch::TextureTarget(m_ImageInfo.Target);

		GLCall(glBindTexture(target, m_Id));
		GLCall(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GLSwitch::TextureFilter(m_ImageInfo.MagFilter)));
		GLCall(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GLSwitch::TextureFilter(m_ImageInfo.MinFilter)));
		GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_S, GLSwitch::TextureWrap(m_ImageInfo.WrapS)));
		GLCall(glTexParameteri(target, GL_TEXTURE_WRAP_T, GLSwitch::TextureWrap(m_ImageInfo.WrapT)));

		if(info.TextureLodBias > -1.0f)
			GLCall(glTexParameterf(target, GL_TEXTURE_LOD_BIAS, info.TextureLodBias));

		if (target == GL_TEXTURE_3D)
		{
			GLCall(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GLSwitch::TextureWrap(m_ImageInfo.WrapR)));
			GLCall(glTexImage3D(GL_TEXTURE_3D,
				0,
				m_ImageInfo.InternalFormat,
				m_ImageInfo.Width,
				m_ImageInfo.Height,
				m_ImageInfo.Depth,
				0,
				m_ImageInfo.DataFormat,
				GLSwitch::TextureType(m_ImageInfo.Type),
				0
			));
			/*glPixelStorei(GL_UNPACK_ROW_LENGTH, 1024);
			for (int z = 0; z < 32; ++z) {
				glPixelStorei(GL_UNPACK_SKIP_PIXELS, 32 * z);
				glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, z, m_ImageInfo.Width, m_ImageInfo.Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, info.Buffer);
			}
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);*/
		}
		else
		{
			GLCall(glTexImage2D(target,
				0,
				m_ImageInfo.InternalFormat,
				m_ImageInfo.Width,
				m_ImageInfo.Height,
				0,
				m_ImageInfo.DataFormat,
				GLSwitch::TextureType(m_ImageInfo.Type),
				info.Buffer
			));
		}
		
		if(info.GenerateMipMap)
			glGenerateMipmap(target);

		GLCall(glBindTexture(target, 0));

		if (!info.KeepSourceBuffer)
		{
			m_ImageInfo.Buffer = nullptr;
		}

		if (info.CopySourceBuffer && info.KeepSourceBuffer)
		{
			size_t buffer_size = static_cast<size_t>(info.Width) * info.Height * info.Channels * sizeof(TextureBufferType);
			m_ImageInfo.Buffer = CreateTextureBuffer(buffer_size);
			//memcpy_s(m_ImageInfo.Buffer, buffer_size, info.Buffer, buffer_size);
			memcpy(m_ImageInfo.Buffer,info.Buffer,buffer_size);
		}

		if (m_ImageInfo.DeleteSourceBuffer && !info.KeepSourceBuffer)
			DeleteTextureBuffer(info.Buffer);

		if (m_Name == "" || m_Name.empty())
		{
			if (m_Path == "" || m_Path.empty())
				m_Name = std::string(Base_Unamed_Texture);
			else
			{
				auto lastSlash = m_Path.find_last_of("/\\");
				lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
				auto lastDot = m_Path.rfind('.');
				auto count = lastDot == std::string::npos ? m_Path.size() - lastSlash : lastDot - lastSlash;
				m_Name = path.substr(lastSlash, count);
			}
		}
	}

	Texture::~Texture()
	{
		if (m_Id)
		{
			glDeleteTextures(1, &m_Id);
			m_Id = NULL;
		}
		if (m_ImageInfo.Buffer)
		{
			if(m_ImageInfo.CopySourceBuffer || m_ImageInfo.KeepSourceBuffer)
				DeleteTextureBuffer(m_ImageInfo.Buffer);
			m_ImageInfo.Buffer = nullptr;
		}
	}

	Ref<Texture> Texture::CreateTexture(TextureSpecifications& info)
	{
		return MakeRef<Texture>(info);
	}

	Ref<Texture> Texture::CreateTexture(const std::string& path, const std::string& name)
	{
		auto info = GetImageInfo(path);
		info.Name = name;
		info.KeepSourceBuffer = true;
		return MakeRef<Texture>(GetImageInfo(path),path);
	}

	/*void Texture::CreatePng(const std::string& path, const TextureSpecifications& image_info)
	{
		BASE_ASSERT(image_info.Width > 0 && image_info.Height > 0 && image_info.Buffer != nullptr && path != "" && image_info.Channels > 2);
		auto type = image_info.Channels == 4 ? LodePNGColorType::LCT_RGBA : LodePNGColorType::LCT_RGB;
		lodepng::encode(path, image_info.Buffer, image_info.Width, image_info.Height, type);
	}

	void Texture::CreatePng(const std::string& path) const
	{
		BASE_ASSERT(m_ImageInfo.Width > 0 && m_ImageInfo.Height > 0 && m_ImageInfo.Buffer != nullptr && path != "" && m_ImageInfo.Channels > 2);
		auto type = m_ImageInfo.Channels == 4 ? LodePNGColorType::LCT_RGBA : LodePNGColorType::LCT_RGB;
		lodepng::encode(path, m_ImageInfo.Buffer, m_ImageInfo.Width, m_ImageInfo.Height, type);
	}*/

	Ref<Texture> Texture::GetWhiteTexture()
	{
		static bool once = [&]()
		{
			TextureSpecifications info;
			info.Width = 1;
			info.Height = 1;
			info.Channels = 4;
			info.Buffer = CreateTextureBuffer(1,1,info.Channels);
			info.DeleteSourceBuffer = false;
			info.MinFilter = GL_TextureFilter::NEAREST;
			info.MagFilter = GL_TextureFilter::NEAREST;
			info.WrapS = GL_TextureWrap::REPEAT;
			info.WrapT = GL_TextureWrap::REPEAT;

			for (unsigned int ix = 0; ix < 1; ++ix)
			{
				for (unsigned int iy = 0; iy < 1; ++iy)
				{
					info.Buffer[ix * 1 * 4 + iy * 4 + 0] = 0xff; //red
					info.Buffer[ix * 1 * 4 + iy * 4 + 1] = 0xff; //green
					info.Buffer[ix * 1 * 4 + iy * 4 + 2] = 0xff; //blue
					info.Buffer[ix * 1 * 4 + iy * 4 + 3] = 0xff; //alpha
				}
			}

			m_WhiteTexture = Texture::CreateTexture(info);
			return true;
		}();
		return m_WhiteTexture;
	}

	TextureBufferType* Texture::CreateTextureBuffer(size_t size)
	{
		GAME_CORE_ASSERT(size > 0,"Buffer size can't be 0");
		return (TextureBufferType*)malloc(size);
	}

	TextureBufferType* Texture::CreateTextureBuffer(unsigned int w, unsigned int h, unsigned int channels)
	{
		return CreateTextureBuffer(static_cast<size_t>(w) * h * channels * sizeof(TextureBufferType));
	}

	void Texture::DeleteTextureBuffer(TextureBufferType* buffer)
	{
		if(buffer)
			free(buffer);
	}

	void Texture::DeleteWhiteTexture()
	{
		m_WhiteTexture.reset();
	}

	TextureSpecifications Texture::GetImageInfo(const std::string& path)
	{
		const std::string ext = utils::ToLower(path.substr(path.find_last_of('.') + 1));
		
		//if (ext == "png")
		//{
		//	return GetImageInfoLodePNG(path);
		//}
		
		return GetImageInfoStbi(path);
	}

	/*TextureSpecifications Texture::GetImageInfoLodePNG(const std::string& path)
	{
		TextureSpecifications info;

		//TODO: Flip vertically
		unsigned w = 0, h = 0;
		std::vector<unsigned char> image;

		unsigned error = lodepng::decode(image, w, h, path);
		if (error) //TODO: Deal properly with this error
			GAME_CORE_ASSERT(false,lodepng_error_text(error));

		info.Width = w;
		info.Height = h;
		info.Channels = 4;
		auto size = image.size();
		info.Buffer = CreateTextureBuffer(image.size());
		/*TextureBufferType* BufferPtr = info.Buffer;
		
		unsigned char* buff = image.data();
		unsigned char* last_ptr = &image.back();

		for (int i = h - 1; i >= 0; i--)
		{
			unsigned char* ptr = last_ptr - w;
			for (int j = 0; j < w; j++)
			{
				*BufferPtr = *ptr;
				BufferPtr++;
				ptr++;
			}
			if (last_ptr <= info.Buffer)
				break;
			last_ptr = ptr;
		}//

		std::copy(image.begin(), image.end(), info.Buffer);
		image.clear();

		info.DeleteSourceBuffer = false;
		info.CopySourceBuffer = false;
		info.KeepSourceBuffer = true;
		return info;
	}*/

	TextureSpecifications Texture::GetImageInfoStbi(const std::string& path)
	{
		//TODO: Fix this stbi 'no SOI' bullshit 
		TextureSpecifications info;

		stbi_set_flip_vertically_on_load(1);
		auto px = stbi_load(path.c_str(), &info.Width, &info.Height, &info.Channels, 0);

		const char* er = stbi_failure_reason();
		if (er)
		{
			if(px)
				stbi_image_free(px);
			info.Buffer = nullptr;
			GAME_CORE_ASSERT(false, "Failed to load image");
		}
		info.Buffer = px;

		info.DeleteSourceBuffer = false;
		info.CopySourceBuffer = false;
		info.KeepSourceBuffer = true;
		return info;
	}
}