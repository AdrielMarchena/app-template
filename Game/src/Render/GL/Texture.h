#pragma once

#include "Core/Core.h"
#include "glm/glm.hpp"
#include "TextureSpecifications.h"
#include <string>
namespace Game
{
	class Texture
	{
	private:
		uint32_t m_Id = NULL;
		TextureSpecifications m_ImageInfo;
		std::string m_Path;
		std::string m_Name;
		static Ref<Texture> m_WhiteTexture;
	public:
		Texture(const TextureSpecifications& info ,const std::string& path = std::string());
		Texture() = default;

		~Texture();

		static Ref<Texture> CreateTexture(TextureSpecifications& info);
		static Ref<Texture> CreateTexture(const std::string& path, const std::string& name = std::string());

		uint32_t GetId() const { return m_Id; };
		const std::string& GetPath() const { return m_Path; }

		const std::string& GetName() const { return m_Name; }

		static void CreatePng(const std::string& path,const TextureSpecifications& image_info);
		void CreatePng(const std::string& path) const;

		const TextureSpecifications& GetInformation() const { return m_ImageInfo; }
		TextureSpecifications& GetInformation() { return m_ImageInfo; }

		static Ref<Texture> GetWhiteTexture();
		/* */

		//new Base::render::TextureBufferType[info.Width * info.Height * info.Channels * sizeof(Base::render::TextureBufferType)];
		static TextureBufferType* CreateTextureBuffer(size_t size);
		static TextureBufferType* CreateTextureBuffer(unsigned int w,unsigned int h,unsigned int channels);
		static void DeleteTextureBuffer(TextureBufferType* buffer);

		static void DeleteWhiteTexture();

		glm::vec2 GetSize() const { return glm::vec2(m_ImageInfo.Width, m_ImageInfo.Height); }

		operator bool() const { return (bool)m_Id; }
		static TextureSpecifications GetImageInfo(const std::string& path);
		static TextureSpecifications GetImageInfoLodePNG(const std::string& path);
		static TextureSpecifications GetImageInfoStbi(const std::string& path);
	private:
	};
}