#include "pch.h"
#include "FramebufferRender.h"

#include "Debug/Intrumentator.h"
#include "glad/glad.h"
#include "gl_error_macro_db.h"

#include "glm/glm.hpp"
#include <glm/gtx/quaternion.hpp>
#include "Utils/Generic.h"
#include "Gl_Commands.h"
namespace Game
{

	static glm::vec2 tex_coords[] = {
		{0.0f,0.0f},
		{1.0f,0.0f},
		{1.0f,1.0f},
		{0.0f,1.0f}
	};
	static glm::vec4 QuadVertexPositions[4];

	FramebufferRender::FramebufferRender(const FrameBufferRenderSpecification& spec)
		:m_Specs(spec)
	{
		GAME_PROFILE_FUNCTION();
		//Create ShaderLib
		m_Shaders = MakeScope<ShaderLib>();
		m_Shaders->Add(Shader::CreateShader("assets/shaders/Framebuffer.glsl"));
		m_CurrentShader = m_Shaders->Get("Framebuffer");
		m_CurrentShader->Bind();

		//Create FrameBuffer and setup Shader and post effects details
		SetUpFramebuffer();
		SetUpShader();
		SetUpPostEffects();
		UsePostEffect("sharpen");

		m_Buffer = new FramebufferQuad[4];
		m_BufferPtr = m_Buffer;

		m_VA = VertexArray::CreateVertexArray();

		m_VB = VertexBuffer::CreateVertexBuffer(sizeof(FramebufferQuad) * 4);

		VertexAttribute layout(m_VB);

		layout.AddLayoutFloat(3, sizeof(FramebufferQuad), (const void*)offsetof(FramebufferQuad, Position));
		layout.AddLayoutFloat(2, sizeof(FramebufferQuad), (const void*)offsetof(FramebufferQuad, TexCoords));

		uint32_t indices[6]{};
		uint32_t offset = 0;
		for (int i = 0; i < 6; i += 6)
		{
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;

			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}
		m_IB = IndexBuffer::CreateIndexBuffer(sizeof(indices), indices);

		QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void FramebufferRender::InvalidateFrameBuffer()
	{
		GAME_PROFILE_FUNCTION();
		//Create FrameBuffer
		m_Framebuffer->m_Specs.width = m_Specs.width * m_Specs.scale_factor;
		m_Framebuffer->m_Specs.height = m_Specs.height * m_Specs.scale_factor;
		m_Framebuffer->Invalidate();
		SetUpShader();
		SetUpPostEffects();
	}

	void FramebufferRender::BindFrameBuffer()
	{
		m_Framebuffer->Bind();
	}

	void FramebufferRender::UnbindFrameBuffer()
	{
		m_Framebuffer->Unbind();
	}

	FramebufferRender::~FramebufferRender()
	{
		delete m_Shaders.release();
		if(m_Buffer)
			delete m_Buffer;
	}

	void FramebufferRender::DrawFrameBuffer(const Camera& camera, const glm::mat4& camera_transform)
	{
		GAME_PROFILE_FUNCTION();
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GLCall(glDisable(GL_DEPTH_TEST));
		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(camera_transform);
		m_CurrentShader->Bind();
		m_CurrentShader->SetUniformMat4f("u_ViewProj", viewProj);

		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_Framebuffer->GetColorTexture(0)));

		m_VA->Bind();
		GLCommands::GL_DrawElementsCall(GL_Target::TRIANGLES, 6, GL_Type::UNSIGNED_INT);

		GLCall(glEnable(GL_DEPTH_TEST));
	}

	void FramebufferRender::AddShader(const Ref<Shader>& shader, const std::string& name)
	{
		const std::string sn = name.empty() ? shader->GetName() : name;
		m_Shaders->Add(sn,shader);
	}
	
	void FramebufferRender::UseShader(const std::string& name)
	{
		m_CurrentShader = m_Shaders->Get(name);
		SetUpShader();
	}

	/*Empty string will set to identity*/
	void FramebufferRender::UsePostEffect(const std::string& name)
	{
		GAME_PROFILE_FUNCTION();
		if (m_CurrentPostEffect)
			m_CurrentPostEffect->active = false;
		if (name.empty() || utils::ToLower(name) == "none")
		{
			m_CurrentPostEffect = nullptr;
		}
		else
		{
			m_CurrentPostEffect = &m_PostEffects[name];
			m_CurrentPostEffect->active = true;
		}
		UpdatePostEffects();
	}

	void FramebufferRender::SetUpPostEffects()
	{
		GAME_PROFILE_FUNCTION();
		//I was getting a small value for the width and height here, 
		//it was a bug that make a nice effect actually (only with the sharpen kernel tho)
		float offset_x = 1.0f / m_Framebuffer->GetSpec().width;
		float offset_y = 1.0f / m_Framebuffer->GetSpec().height;

		std::vector<glm::vec2> offsets = 
		{
			glm::vec2(-offset_x, offset_y),  glm::vec2(0.0f, offset_y),  glm::vec2(offset_x,offset_y),
			glm::vec2(-offset_x, 0.0f),      glm::vec2(0.0f, 0.0f),      glm::vec2(offset_x,0.0f),
			glm::vec2(-offset_x, -offset_y), glm::vec2(0.0f, -offset_y), glm::vec2(offset_x,-offset_y)
		};

		FramebufferPostEffect identity;
		identity.offsets = offsets;
		identity.kernel =
		{
			 0.0f, 0.0f, 0.0f,
			 0.0f, 1.0f, 0.0f,
			 0.0f, 0.0f, 0.0f,
		};
		m_PostEffects["identity"] = identity;

		FramebufferPostEffect sharpen;
		sharpen.offsets = offsets;
		sharpen.kernel = 
		{
			 0.0f,-1.0f, 0.0f,
			-1.0f, 5.0f,-1.0f,
			 0.0f,-1.0f, 0.0f
		};
		m_PostEffects["sharpen"] = sharpen;

		FramebufferPostEffect box_blur;
		box_blur.offsets = offsets;
		float dec = 1.0f / 9.0f;
		box_blur.kernel =
		{
			dec,dec,dec,
			dec,dec,dec,
			dec,dec,dec
		};
		m_PostEffects["box_blur"] = box_blur;

		FramebufferPostEffect edge_detection0;
		edge_detection0.offsets = offsets;
		edge_detection0.kernel =
		{
			1.0f, 1.0f,1.0f,
			1.0f,-8.0f,1.0f,
			1.0f, 1.0f,1.0f
		};
		m_PostEffects["edge_detection0"] = edge_detection0;

		FramebufferPostEffect edge_detection1;
		edge_detection1.offsets = offsets;
		edge_detection1.kernel =
		{
		   -1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 0.0f,
		    1.0f, 0.0f,-1.0f
		};
		m_PostEffects["edge_detection1"] = edge_detection1;

		FramebufferPostEffect edge_detection2;
		edge_detection2.offsets = offsets;
		edge_detection2.kernel =
		{
			0.0f,-1.0f, 0.0f,
		   -1.0f, 4.0f,-1.0f,
			0.0f,-1.0f, 0.0f
		};
		m_PostEffects["edge_detection2"] = edge_detection2;

		FramebufferPostEffect edge_detection3;
		edge_detection3.offsets = offsets;
		edge_detection3.kernel =
		{
			-1.0f,-1.0f,-1.0f,
			-1.0f, 8.0f,-1.0f,
			-1.0f,-1.0f,-1.0f
		};
		m_PostEffects["edge_detection3"] = edge_detection3;

		FramebufferPostEffect gaussian_blur3x3;
		gaussian_blur3x3.offsets = offsets;
		dec = 1.0f/16.0f;
		gaussian_blur3x3.kernel =
		{
			1.0f * dec, 2.0f * dec, 1.0f * dec,
			2.0f * dec, 4.0f * dec, 2.0f * dec,
			1.0f * dec, 2.0f * dec, 1.0f * dec
		};
		m_PostEffects["gaussian_blur3x3"] = gaussian_blur3x3;

		FramebufferPostEffect gaussian_blur5x5;
		gaussian_blur5x5.offsets = offsets;
		dec = 1.0f / 256.0f;
		gaussian_blur5x5.kernel =
		{
			1.0f * dec, 4.0f * dec, 6.0f * dec, 4.0f * dec, 1.0f * dec,
			4.0f * dec,16.0f * dec,24.0f * dec,16.0f * dec, 4.0f * dec,
			6.0f * dec,24.0f * dec,36.0f * dec,24.0f * dec, 6.0f * dec,
			4.0f * dec,16.0f * dec,24.0f * dec,16.0f * dec, 4.0f * dec,
			1.0f * dec, 4.0f * dec, 6.0f * dec, 4.0f * dec, 1.0f * dec
		};
		m_PostEffects["gaussian_blur5x5"] = gaussian_blur5x5;

		FramebufferPostEffect unsharp_masking;
		unsharp_masking.offsets = offsets;
		dec = -(1.0f / 256.0f);
		unsharp_masking.kernel =
		{
			1.0f * dec, 4.0f * dec,  6.0f * dec, 4.0f * dec, 1.0f * dec,
			4.0f * dec,16.0f * dec, 24.0f * dec,16.0f * dec, 4.0f * dec,
			6.0f * dec,24.0f * dec,-476.0f *dec,24.0f * dec,6.0f * dec,
			4.0f * dec,16.0f * dec, 24.0f * dec,16.0f * dec, 4.0f * dec,
			1.0f * dec, 4.0f * dec,  6.0f * dec, 4.0f * dec, 1.0f * dec
		};
		m_PostEffects["unsharp_masking"] = unsharp_masking;

		FramebufferPostEffect sobel_feldman;
		sobel_feldman.offsets = offsets;
		sobel_feldman.kernel =
		{
			-1.0f, 0.0f, 1.0f,
			-2.0f, 0.0f, 2.0f,
			-1.0f, 0.0f, 1.0f,
		};
		m_PostEffects["sobel_feldman"] = sobel_feldman;

		FramebufferPostEffect victor_gordan;
		victor_gordan.offsets = offsets;
		victor_gordan.kernel =
		{
			-2.0f,-1.0f, 0.0f,
			-1.0f, 1.0f, 1.0f,
			 0.0f, 1.0f, 2.0f,
		};
		m_PostEffects["victor_gordan"] = victor_gordan;
	}

	void FramebufferRender::SetUpFramebuffer()
	{
		GAME_PROFILE_FUNCTION();
		//Create FrameBuffer
		FramebufferSpecification frame_spec;
		frame_spec.Attachments = { FrambufferTextureFormat::RGBA8,FrambufferTextureFormat::RED_INTEGER,FrambufferTextureFormat::DEPTH };
		frame_spec.width = m_Specs.width * m_Specs.scale_factor;
		frame_spec.height = m_Specs.height * m_Specs.scale_factor;
		m_Framebuffer = MakeScope<Framebuffer>(frame_spec);
	}

	void FramebufferRender::UpdatePostEffects()
	{
		GAME_PROFILE_FUNCTION();
		if (m_CurrentPostEffect)
		{
			float offset_x = 1.0f / m_Framebuffer->GetSpec().width;
			float offset_y = 1.0f / m_Framebuffer->GetSpec().height;

			m_CurrentPostEffect->offsets = {
				glm::vec2(-offset_x, offset_y),  glm::vec2(0.0f, offset_y),  glm::vec2(offset_x,offset_y),
				glm::vec2(-offset_x, 0.0f),      glm::vec2(0.0f, 0.0f),      glm::vec2(offset_x,0.0f),
				glm::vec2(-offset_x, -offset_y), glm::vec2(0.0f, -offset_y), glm::vec2(offset_x,-offset_y)
			};
		}
		SetUpShader();
	}

	void FramebufferRender::SetUpShader()
	{
		GAME_PROFILE_FUNCTION();
		float u_Width = m_Framebuffer->GetSpec().width;
		float u_Height = m_Framebuffer->GetSpec().height;
		
		m_CurrentShader->Bind();
		m_CurrentShader->SetUniform1i("u_Framebuffer", 0);
		m_CurrentShader->SetUniform1f("u_Width", u_Width);
		m_CurrentShader->SetUniform1f("u_Height", u_Height);
		if (m_CurrentPostEffect)
		{
			int kernel_size = m_CurrentPostEffect->kernel.size();
			m_CurrentShader->SetUniform1i("kernel_size", kernel_size);
			m_CurrentShader->SetUniform1fv("kernel_5x5_slot", kernel_size, m_CurrentPostEffect->kernel.data());

			int offset_size = m_CurrentPostEffect->offsets.size();
			m_CurrentShader->SetUniform1i("offset_size", offset_size);
			m_CurrentShader->SetUniform1fv("offsets", offset_size, &m_CurrentPostEffect->offsets[0][0]);
		}
		else
			m_CurrentShader->SetUniform1i("kernel_size", 0);
	}

	void FramebufferRender::SetGLViewport(bool use_scalor)
	{
		GAME_PROFILE_FUNCTION();
		int wid = use_scalor ? m_Specs.width * m_Specs.scale_factor : m_Specs.width;
		int hei = use_scalor ? m_Specs.height * m_Specs.scale_factor : m_Specs.height;
		
		GLCall(glViewport(0, 0, wid, hei));
	}

	void FramebufferRender::CalculateQuadTransform()
	{
		GAME_PROFILE_FUNCTION();
		glm::mat4 rotation = glm::toMat4(glm::quat(m_Rotation));
		m_Transform = glm::translate(glm::mat4(1.0f), m_Position)
					  * rotation
					  * glm::scale(glm::mat4(1.0f), m_Scale);

		m_BufferPtr = m_Buffer;
		for (size_t i = 0; i < 4; i++)
		{
			m_BufferPtr->Position = m_Transform * QuadVertexPositions[i];
			m_BufferPtr->TexCoords = tex_coords[i];
			m_BufferPtr++;
		}
		GLsizeiptr size = (uint8_t*)m_BufferPtr - (uint8_t*)m_Buffer;
		m_VB->SubData(size, m_Buffer);
	}
}