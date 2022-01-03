#pragma once

#include "Core/Core.h"
#include "Core/Assert.h"
#include "Texture.h"
#include <vector>
#include "FramebufferSpecifications.h"
namespace Game
{
	class FramebufferRender;
	class Framebuffer
	{
	private:
		uint32_t m_Id;
		uint32_t m_DepthBuffer;
		std::vector<uint32_t> m_ColorTextures;
		std::vector<uint8_t> m_EmptyData;
		int m_EmptyValue = -1;
		//uint32_t m_DepthTexture;
		FramebufferSpecification m_Specs;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
		FramebufferTextureSpecification m_DepthAttachmentSpec;
	public:
		Framebuffer(const FramebufferSpecification& specs);
		~Framebuffer();

		uint32_t GetColorTexture(uint32_t index = 0) const { GAME_CORE_ASSERT(index < m_ColorTextures.size(),""); return m_ColorTextures[index]; }
		//Ref<render::Texture> GetDepthTexture() const { return m_DepthTexture; }
		void Invalidate();

		void Bind();
		void Unbind();

		glm::vec2 GetSize() const { return {(float)m_Specs.width,(float)m_Specs.height}; }

		int ReadPixel(uint32_t index, int x, int y);
		void ClearAttachment(uint32_t index, int value);

		const FramebufferSpecification& GetSpec() const { return m_Specs; }
		FramebufferSpecification& GetSpec() { return m_Specs; }
		static Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification& specs);

	private:
		friend FramebufferRender;
	};
}