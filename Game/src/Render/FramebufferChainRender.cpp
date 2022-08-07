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

FramebufferChainRender::FramebufferChainRender (FramebufferRenderSpecification sceneSpecs, FramebufferRenderSpecification screenSpecs)
{
	GAME_PROFILE_FUNCTION();
	// Create Scenes

	m_Chains.push_front(Chain());
	m_Chains[0].RenderData.FramebufferSpecifications = screenSpecs;;
	SetUpScreenFramebufferChain();

	m_SceneChain.RenderData.FramebufferSpecifications = sceneSpecs;
	SetUpSceneFramebufferChain();
}

FramebufferChainRender::~FramebufferChainRender()
{
}

void FramebufferChainRender::OnResize(uint32_t w, uint32_t h)
{
	if (m_SceneChain.OnResizeFunc)
		m_SceneChain.OnResizeFunc(m_SceneChain, w, h);

	for (auto& chain : m_Chains)
		if (chain.OnResizeFunc)
			chain.OnResizeFunc(chain, w, h);

	InvalidateFramebuffers();
}

void FramebufferChainRender::BindSceneFramebuffer()
{
	m_SceneChain.RenderData.Frambuffer->Bind();
	SetGLViewport(m_SceneChain);
}

void FramebufferChainRender::UnbindSceneFramebuffer()
{
	m_SceneChain.RenderData.Frambuffer->Unbind();
}

//void FramebufferChainRender::RenderChain()
//{
//	GAME_PROFILE_FUNCTION();
//
//	size_t s = m_Chains.size();
//	size_t actual = 1;
//	for (auto& chain : m_Chains)
//	{
//		GetCurrentFramebuffer()->Unbind();
//		auto& data = chain.RenderData;
//		auto& func = chain.DrawFunc;
//
//		if (actual < s)
//			GetFrontFramebuffer()->Bind();
//		else
//			Render2D::Disable({ GLEnableCaps::DEPTH_TEST });
//
//		Render2D::ClearNoDepth();
//
//		glm::mat4 viewProj = data.RenderCamera.GetProjection() * glm::inverse(data.CameraTransform);
//		data.CShader->Bind();
//		data.CShader->SetUniformMat4f("u_ViewProj", viewProj);
//		data.CShader->SetUniform1i("u_Framebuffer", 0);
//
//		GLCall(glActiveTexture(GL_TEXTURE0));
//		GLCall(glBindTexture(GL_TEXTURE_2D, GetBackFramebuffer()->GetColorTexture(0)));
//
//		if (func)
//			func(data, m_Specs);
//
//		data.VA->Bind();
//		GLCommands::GL_DrawElementsCall(GL_Target::TRIANGLES, 6, GL_Type::UNSIGNED_INT);
//		actual++;
//	}
//	Render2D::Enable({ GLEnableCaps::DEPTH_TEST });
//	SwapFramebuffer();
//}

static void DrawChain(Chain& chain, bool bindBuffer, Ref<Framebuffer>& previousFramebuffer)
{
	auto& data = chain.RenderData;
	auto& func = chain.DrawFunc;

	if (bindBuffer)
		data.Frambuffer->Bind();
	else
		data.Frambuffer->Unbind();

	FramebufferChainRender::SetGLViewport(chain);
	Render2D::ClearNoDepth();

	glm::mat4 viewProj = data.RenderCamera.GetProjection() * glm::inverse(data.CameraTransform);
	data.CShader->Bind();
	data.CShader->SetUniformMat4f("u_ViewProj", viewProj);
	data.CShader->SetUniform1i("u_Framebuffer", 0);

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, previousFramebuffer->GetColorTexture(0)));

	if (func)
		func(chain, data);

	data.VA->Bind();
	GLCommands::GL_DrawElementsCall(GL_Target::TRIANGLES, 6, GL_Type::UNSIGNED_INT);
}

void FramebufferChainRender::RenderChain()
{
	GAME_PROFILE_FUNCTION();

	Ref<Framebuffer>& previousFramebuffer = m_SceneChain.RenderData.Frambuffer;
	previousFramebuffer->Unbind();

	Render2D::Disable({ GLEnableCaps::DEPTH_TEST });
	size_t s = m_Chains.size();
	size_t actual = 1;
	for (auto& chain : m_Chains)
	{
		if (chain.Ignore) continue;
		
		DrawChain(chain, actual < s, previousFramebuffer);
		previousFramebuffer = chain.RenderData.Frambuffer;
		actual++;
	}

	Render2D::Enable({ GLEnableCaps::DEPTH_TEST });
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

void FramebufferChainRender::SetGLViewport(Chain& chain)
{
	uint32_t w = chain.RenderData.FramebufferSpecifications.Width * chain.RenderData.FramebufferSpecifications.ScaleFactor;
	uint32_t h = chain.RenderData.FramebufferSpecifications.Height * chain.RenderData.FramebufferSpecifications.ScaleFactor;
	GLCall(glViewport(0, 0, w, h));
}

void FramebufferChainRender::StandardChainOnResizeCallback(Chain& self, uint32_t w, uint32_t h)
{
	float sc = self.RenderData.RenderCamera.GetOrthographicSize();
	float ar = (float)w / (float)h;

	self.RenderData.Position =  { 0.0f, 0.0f, 0.0f };
	self.RenderData.Scale =		{ sc * ar, sc, 1.0f };
	self.RenderData.Rotation =  { 0.0f, 0.0f, 0.0f };
	
	static TransformComponent FramebufferCameraTransform;
	self.RenderData.CameraTransform = FramebufferCameraTransform.GetTransform();
	
	FramebufferChainRender::CalculateQuadTransform(self);
}

void FramebufferChainRender::SetUpSceneFramebufferChain()
{
	Chain& chain = m_SceneChain;

	chain.RenderData.CShader = m_Chains[0].RenderData.CShader;
	chain.RenderData.CShader->Bind();
	chain.RenderData.CShader->SetUniform1i("u_Framebuffer", 0);

	chain.RenderData.Frambuffer = CreateFramebuffer
	(
		chain.RenderData.FramebufferSpecifications.Width,
		chain.RenderData.FramebufferSpecifications.Height,
		chain.RenderData.FramebufferSpecifications.ScaleFactor
	);

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
	uint32_t w = chain.RenderData.FramebufferSpecifications.Width;
	uint32_t h = chain.RenderData.FramebufferSpecifications.Height;

	float ar = (float)w / (float)h;
	chain.RenderData.RenderCamera.SetViewportSize(w, h);
	chain.RenderData.RenderCamera.SetOrthographicNearClip(-1.0f);
	chain.RenderData.RenderCamera.SetOrthographicFarClip(10.0f);
	
	float sc = chain.RenderData.RenderCamera.GetOrthographicSize();

	chain.RenderData.Position = { 0.0f,0.0f,0.0f };
	chain.RenderData.Scale = { sc * ar,sc, 1.0f };
	chain.RenderData.Rotation = { 0.0f,0.0f,0.0f };

	chain.RenderData.IB = IndexBuffer::CreateIndexBuffer(sizeof(indices), indices);
	chain.RenderData.VA->Unbind();

	CalculateQuadTransform(chain);

	chain.OnResizeFunc = StandardChainOnResizeCallback;

	m_Chains.push_front(chain);
}

void FramebufferChainRender::SetUpScreenFramebufferChain()
{
	Chain& chain = m_Chains[0];

	chain.RenderData.CShader = Shader::CreateShader("assets/shaders/Framebuffer.glsl");
	chain.RenderData.CShader->Bind();
	chain.RenderData.CShader->SetUniform1i("u_Framebuffer", 0);

	chain.RenderData.Frambuffer = CreateFramebuffer
	(
		chain.RenderData.FramebufferSpecifications.Width,
		chain.RenderData.FramebufferSpecifications.Height,
		chain.RenderData.FramebufferSpecifications.ScaleFactor
	);

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
	uint32_t w = chain.RenderData.FramebufferSpecifications.Width;
	uint32_t h = chain.RenderData.FramebufferSpecifications.Height;

	float ar = (float)w / (float)h;
	chain.RenderData.RenderCamera.SetViewportSize(w, h);
	chain.RenderData.RenderCamera.SetOrthographicNearClip(-1.0f);
	chain.RenderData.RenderCamera.SetOrthographicFarClip(10.0f);

	float sc = chain.RenderData.RenderCamera.GetOrthographicSize();

	chain.RenderData.Position = { 0.0f,0.0f,0.0f };
	chain.RenderData.Scale = { sc * ar,sc, 1.0f };
	chain.RenderData.Rotation = { 0.0f,0.0f,0.0f };

	chain.RenderData.IB = IndexBuffer::CreateIndexBuffer(sizeof(indices), indices);
	chain.RenderData.VA->Unbind();

	CalculateQuadTransform(chain);
	chain.OnResizeFunc = StandardChainOnResizeCallback;
}

void FramebufferChainRender::InvalidateFramebuffers()
{
	GAME_PROFILE_FUNCTION();
	m_SceneChain.RenderData.InvalidateFramebuffer();

	for (auto& chain : m_Chains)
	{
		chain.RenderData.InvalidateFramebuffer();
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

void FramebufferChainRenderData::InvalidateFramebuffer()
{
	Frambuffer->GetSpec().width = FramebufferSpecifications.Width * FramebufferSpecifications.ScaleFactor;
	Frambuffer->GetSpec().height = FramebufferSpecifications.Height * FramebufferSpecifications.ScaleFactor;
	Frambuffer->Invalidate();
}

}
