#include "pch.h"
#include "FramebufferChainRender.h"
#include "Debug/Intrumentator.h"

#include "GL/VertexAttribute.h"

#include "GL/gl_error_macro_db.h"
#include "GL/Gl_Commands.h"
#include "Render2D.h"
#include <glm/gtx/quaternion.hpp>

#include "Scene/Components.h"

#include "glad/glad.h"

namespace Game
{
static glm::vec2 tex_coords[] = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f}
};

static glm::vec4 QuadVertexPositions[4] = {
	{ -0.5f, -0.5f, 0.0f, 1.0f },
	{  0.5f, -0.5f, 0.0f, 1.0f },
	{  0.5f,  0.5f, 0.0f, 1.0f },
	{ -0.5f,  0.5f, 0.0f, 1.0f }
};

FramebufferChainRender::FramebufferChainRender(FramebufferChainRenderSpecification specs)
{
	GAME_PROFILE_FUNCTION();
	// Create framebuffers
	for (auto& framebuffer : m_Framebuffers)
		framebuffer = CreateFramebuffer(m_Specs.width, m_Specs.height, m_Specs.scale_factor);
	m_CurrentFramebufferIndex = 0;
	m_CurrentFramebuffer = &m_Framebuffers[m_CurrentFramebufferIndex];

	SetUpFramebufferChain();
}

FramebufferChainRender::~FramebufferChainRender()
{
}

void FramebufferChainRender::OnResize(uint32_t w, uint32_t h)
{
	m_Specs.width = w;
	m_Specs.height = h;
	InvalidateFramebuffers();
	float ar = (float)w / (float)h;

	auto& chain = GetFramebufferChain();
	float sc = chain.RenderData.RenderCamera.GetOrthographicSize();

	chain.RenderData.Position = { 0.0f,0.0f,0.0f };
	chain.RenderData.Scale = { sc * ar,sc, 1.0f };
	chain.RenderData.Rotation = { 0.0f,0.0f,0.0f };

	static TransformComponent FramebufferCameraTransform;
	chain.RenderData.CameraTransform = FramebufferCameraTransform.GetTransform();

	CalculateQuadTransform(chain);
}

void FramebufferChainRender::RenderChain()
{
	GAME_PROFILE_FUNCTION();
	GetCurrentFramebuffer()->Unbind();
	size_t s = m_Chains.size();
	size_t actual = 1;
	for (auto& chain : m_Chains)
	{
		auto& data = chain.RenderData;
		auto& func = chain.DrawFunc;
		SwapFramebuffer();

		Render2D::Clear();
		Render2D::Disable({ GLEnableCaps::DEPTH_TEST });
		glm::mat4 viewProj = data.RenderCamera.GetProjection() * glm::inverse(data.CameraTransform);
		data.CShader->Bind();
		data.CShader->SetUniformMat4f("u_ViewProj", viewProj);

		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, GetBackFramebuffer()->GetColorTexture(0)));

		if (func)
			func(data, m_Specs);

		// TODO: Improve this check (please)
		if (actual < s)
			GetFrontFramebuffer()->Bind();

		data.VA->Bind();
		GLCommands::GL_DrawElementsCall(GL_Target::TRIANGLES, 6, GL_Type::UNSIGNED_INT);

		Render2D::Enable({ GLEnableCaps::DEPTH_TEST });
		actual++;
	}

	GetFrontFramebuffer()->Unbind();
}

void FramebufferChainRender::SwapFramebuffer()
{
	m_CurrentFramebufferIndex = !m_CurrentFramebufferIndex;
	m_CurrentFramebuffer = &m_Framebuffers[m_CurrentFramebufferIndex];
}

Ref<Framebuffer>& FramebufferChainRender::GetFrontFramebuffer()
{
	return *m_CurrentFramebuffer;
}

Ref<Framebuffer>& FramebufferChainRender::GetBackFramebuffer()
{
	return m_Framebuffers[!m_CurrentFramebufferIndex];
}

Ref<Framebuffer> FramebufferChainRender::CreateFramebuffer(uint32_t w, uint32_t h, float scalor)
{
	//Create FrameBuffer
	FramebufferSpecification frame_spec;
	frame_spec.Attachments = { FrambufferTextureFormat::RGBA8,FrambufferTextureFormat::RED_INTEGER,FrambufferTextureFormat::DEPTH };
	frame_spec.width = w * scalor;
	frame_spec.height = h * scalor;
	return MakeScope<Framebuffer>(frame_spec);
}

void FramebufferChainRender::SetUpFramebufferChain()
{
	Chain chain;

	chain.RenderData.CShader = Shader::CreateShader("assets/shaders/Framebuffer.glsl");

	chain.RenderData.Buffer = new FramebufferQuad[4];
	chain.RenderData.BufferPtr = chain.RenderData.Buffer;

	chain.RenderData.VA = VertexArray::CreateVertexArray();

	chain.RenderData.VB = VertexBuffer::CreateVertexBuffer(sizeof(FramebufferQuad) * 4);

	VertexAttribute layout(chain.RenderData.VB);

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
	uint32_t w = m_Specs.width;
	uint32_t h = m_Specs.height;

	float ar = (float)w / (float)h;
	chain.RenderData.RenderCamera.SetViewportSize(w, h);
	chain.RenderData.RenderCamera.SetOrthographicNearClip(-1.0f);
	chain.RenderData.RenderCamera.SetOrthographicFarClip(10.0f);
	
	float sc = chain.RenderData.RenderCamera.GetOrthographicSize();

	chain.RenderData.Position = { 0.0f,0.0f,0.0f };
	chain.RenderData.Scale = { sc * ar,sc, 1.0f };
	chain.RenderData.Rotation = { 0.0f,0.0f,0.0f };

	CalculateQuadTransform(chain);

	chain.RenderData.IB = IndexBuffer::CreateIndexBuffer(sizeof(indices), indices);
	chain.RenderData.VA->Unbind();
	m_Chains.push_front(chain);
}

void FramebufferChainRender::InvalidateFramebuffers()
{
	GAME_PROFILE_FUNCTION();
	for (auto& framebuffer : m_Framebuffers)
	{
		framebuffer->GetSpec().width = m_Specs.width * m_Specs.scale_factor;
		framebuffer->GetSpec().height = m_Specs.height * m_Specs.scale_factor;
		framebuffer->Invalidate();
	}
}

void FramebufferChainRender::CalculateQuadTransform(Chain& chain)
{
	GAME_PROFILE_FUNCTION();

	glm::mat4 rotation = glm::toMat4(glm::quat(chain.RenderData.Rotation));
	chain.RenderData.Transform = glm::translate(glm::mat4(1.0f), chain.RenderData.Position)
		* rotation
		* glm::scale(glm::mat4(1.0f), chain.RenderData.Scale);

	chain.RenderData.BufferPtr = chain.RenderData.Buffer;
	for (size_t i = 0; i < 4; i++)
	{
		chain.RenderData.BufferPtr->Position = chain.RenderData.Transform * QuadVertexPositions[i];
		chain.RenderData.BufferPtr->TexCoords = tex_coords[i];
		chain.RenderData.BufferPtr++;
	}
	GLsizeiptr size = (uint8_t*)chain.RenderData.BufferPtr - (uint8_t*)chain.RenderData.Buffer;
	chain.RenderData.VB->SubData(size, chain.RenderData.Buffer);
}

Chain& FramebufferChainRender::GetFramebufferChain()
{
	GAME_CORE_ASSERT(!m_Chains.empty(), "The Framebuffer chain is not setup or not on the deque");
	return *(m_Chains.end() - 1);
}

void FramebufferChainRender::SetGLViewport(bool use_scalor)
{
	int wid = use_scalor ? m_Specs.width * m_Specs.scale_factor : m_Specs.width;
	int hei = use_scalor ? m_Specs.height * m_Specs.scale_factor : m_Specs.height;

	GLCall(glViewport(0, 0, wid, hei));
}

void FramebufferChainRender::SetScalorFactor(float scalor)
{
	m_Specs.scale_factor = scalor;
	InvalidateFramebuffers();
}

}
