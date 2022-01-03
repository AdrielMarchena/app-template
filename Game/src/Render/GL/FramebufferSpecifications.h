#pragma once

#include <vector>
#include <initializer_list>

namespace Game
{
	enum class FrambufferTextureFormat
	{
		NONE = 0,
		RGBA8,
		RGB,
		RED_INTEGER,
		DEPTH24STENCIL8,
		DEPTH = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FrambufferTextureFormat TextureFormat = FrambufferTextureFormat::NONE;

		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FrambufferTextureFormat format)
			:TextureFormat(format) {}

	};

	struct FramebufferAttachmentSpecification
	{
		std::vector<FramebufferTextureSpecification> Attachment;

		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			:Attachment(attachments) {}

	};

	struct FramebufferSpecification
	{
		uint32_t width;
		uint32_t height;
		uint32_t samples = 1;
		FramebufferAttachmentSpecification Attachments;
		bool SwapChainTarget = false;
	};
}