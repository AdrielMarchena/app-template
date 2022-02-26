#include "pch.h"
#include "Framebuffer.h"

#include "glad/glad.h"
#include "gl_error_macro_db.h"
#include "Core/Assert.h"

#include "TextureSpecifications.h"

namespace Game
{
	namespace utils
	{
		struct ColectionTextureSpecification
		{
			GL_TextureFilter MinFilter;
			GL_TextureFilter MagFilter;
			GL_TextureWrap WrapS;
			GL_TextureWrap WrapT;
			GL_TextureWrap WrapR;
		};

		static inline GLenum TextureTarget2D(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static inline void CreateTexture(bool multisampled, uint32_t* tex_id, uint32_t count)
		{
			GLCall(glGenTextures(count, tex_id));
		}

		static inline void BindTexture(bool multisampled, uint32_t tex_id)
		{
			GLCall(glBindTexture(TextureTarget2D(multisampled), tex_id));
		}

		static inline void AttachColorTexture(const ColectionTextureSpecification& tex_spec, uint32_t tex_id, int samples, GLenum format, uint32_t width, uint32_t height, int index,GLenum dataformat)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				GLCall(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE));
			}
			else
			{
				GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, dataformat, GL_UNSIGNED_BYTE, nullptr));

				GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLSwitch::TextureFilter(tex_spec.MinFilter)));
				GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLSwitch::TextureFilter(tex_spec.MagFilter)));
				GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLSwitch::TextureWrap(tex_spec.WrapS)));
				GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLSwitch::TextureWrap(tex_spec.WrapT)));
				GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GLSwitch::TextureWrap(tex_spec.WrapR)));

			}
			GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget2D(multisampled), tex_id, 0));
		}

		static inline void CreateDepthBuffers(bool multisampled, uint32_t* buffer_id, uint32_t count)
		{
			GLCall(glGenRenderbuffers(count, buffer_id));
		}

		static inline void AttachDepthBuffer(uint32_t buffer_id, GLenum format, GLenum attach_type , uint32_t width, uint32_t height)
		{
			GLCall(glBindRenderbuffer(GL_RENDERBUFFER, buffer_id));
			GLCall(glRenderbufferStorage(GL_RENDERBUFFER, format, width, height));
			GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attach_type, GL_RENDERBUFFER, buffer_id));
		}

		static inline bool IsDepthFormat(FrambufferTextureFormat format)
		{
			switch (format)
			{
			case FrambufferTextureFormat::DEPTH24STENCIL8: return true;
			}
			return false;
		}
	}

	Framebuffer::Framebuffer(const FramebufferSpecification& specs)
		:m_Id(NULL),m_DepthBuffer(NULL),m_Specs(specs)
	{
		
		for (auto format : m_Specs.Attachments.Attachment)
		{
			if (!utils::IsDepthFormat(format.TextureFormat))
				m_ColorAttachmentSpecs.emplace_back(format);
			else
				m_DepthAttachmentSpec = format;
		}
		Invalidate();
	}

	Framebuffer::~Framebuffer()
	{
		if (m_Id)
		{
			GLCall(glDeleteFramebuffers(1, &m_Id));
			m_Id = NULL;
		}
		if (m_DepthBuffer)
		{
			GLCall(glDeleteRenderbuffers(1, &m_DepthBuffer));
			m_DepthBuffer = NULL;
		}
		if (m_ColorTextures.size())
		{
			GLCall(glDeleteTextures(m_ColorTextures.size(), m_ColorTextures.data()));
			m_ColorTextures.clear();
		}
	}

	void Framebuffer::Invalidate()
	{
		if (m_Id)
		{
			GLCall(glDeleteFramebuffers(1, &m_Id));
			m_Id = NULL;
		}
		if (m_DepthBuffer)
		{
			GLCall(glDeleteRenderbuffers(1, &m_DepthBuffer));
			m_DepthBuffer = NULL;
		}
		if (m_ColorTextures.size())
		{
			GLCall(glDeleteTextures(m_ColorTextures.size(), m_ColorTextures.data()));
			m_ColorTextures.clear();
		}

		GLCall(glGenFramebuffers(1, &m_Id));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Id));
		GAME_CORE_ASSERT(m_Id, "Framebuffer could not be created");

		bool multisample = m_Specs.samples > 1;
		bool depth_exist = m_DepthAttachmentSpec.TextureFormat != FrambufferTextureFormat::NONE;

		utils::ColectionTextureSpecification texture_specifications;
		texture_specifications.MagFilter = GL_TextureFilter::LINEAR;
		texture_specifications.MinFilter = GL_TextureFilter::LINEAR;
		texture_specifications.WrapS = GL_TextureWrap::CLAMP_EDGE;
		texture_specifications.WrapR = GL_TextureWrap::CLAMP_EDGE;
		texture_specifications.WrapT = GL_TextureWrap::CLAMP_EDGE;

		if (m_ColorAttachmentSpecs.size())
		{
			m_ColorTextures.resize(m_ColorAttachmentSpecs.size());
			utils::CreateTexture(multisample, m_ColorTextures.data(), m_ColorTextures.size());

			for (size_t i = 0; i < m_ColorTextures.size(); i++)
			{
				utils::BindTexture(multisample, m_ColorTextures[i]);

				switch (m_ColorAttachmentSpecs[i].TextureFormat)
				{
				case FrambufferTextureFormat::RGBA8:
					utils::AttachColorTexture(texture_specifications, m_ColorTextures[i], m_Specs.samples, GL_RGBA8, m_Specs.width, m_Specs.height, i,GL_RGBA);
					break;
				case FrambufferTextureFormat::RGB:
					utils::AttachColorTexture(texture_specifications, m_ColorTextures[i], m_Specs.samples, GL_RGB, m_Specs.width, m_Specs.height, i, GL_RGB);
					break;
				case FrambufferTextureFormat::RED_INTEGER:
					utils::AttachColorTexture(texture_specifications, m_ColorTextures[i], m_Specs.samples, GL_R32I, m_Specs.width, m_Specs.height, i, GL_RED_INTEGER);
					break;
				}
			}
		}

		if (depth_exist)
		{
			utils::CreateDepthBuffers(multisample, &m_DepthBuffer, 1);

			switch (m_DepthAttachmentSpec.TextureFormat)
			{
			case FrambufferTextureFormat::DEPTH24STENCIL8:
				utils::AttachDepthBuffer(m_DepthBuffer, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specs.width, m_Specs.height);
				break;
			}
		}

		if (m_ColorTextures.size() > 1)
		{
			GAME_CORE_ASSERT(m_ColorTextures.size() <= 4,"Should exist at least 1 color texture attachment");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 ,GL_COLOR_ATTACHMENT2 ,GL_COLOR_ATTACHMENT3 };
			GLCall(glDrawBuffers(m_ColorTextures.size(), buffers));
		}
		else if (m_ColorTextures.empty())
		{
			GLCall(glDrawBuffer(GL_NONE));
		}

		GLCall(GAME_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer incomplete"));

		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

		m_EmptyData = std::vector<uint8_t>(m_Specs.width * m_Specs.height * 4, m_EmptyValue);
	}

	void Framebuffer::Bind()
	{
		GAME_CORE_ASSERT(m_Id,"FrameBuffer id can not be 0");
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Id));
		/*if (m_DepthBuffer)
			GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer));*/
	}

	void Framebuffer::Unbind()
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		//GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	}

	int Framebuffer::ReadPixel(uint32_t index, int x, int y)
	{
		GAME_CORE_ASSERT(index < m_ColorTextures.size(),"index is greater than color textures available");
		GLCall(glEnable(GL_MULTISAMPLE));
		int pixel;
		glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);
		return pixel;
	}

	void Framebuffer::ClearAttachment(uint32_t index, int value)
	{
		GAME_CORE_ASSERT(index < m_ColorTextures.size(),"There is no color textures on this framebuffer");

		auto& spec = m_ColorAttachmentSpecs[index];
		utils::BindTexture(false, m_ColorTextures[index]);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Specs.width, m_Specs.height, GL_RED_INTEGER, GL_INT, &m_EmptyData[0]);
	}

	Ref<Framebuffer> Framebuffer::CreateFramebuffer(const FramebufferSpecification& specs)
	{
		return MakeRef<Framebuffer>(specs);
	}

}