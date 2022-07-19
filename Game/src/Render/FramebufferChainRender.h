#pragma once

#include "GL/Framebuffer.h"
#include "GL/VertexArray.h"
#include "GL/VertexBuffer.h"
#include "GL/IndexBuffer.h"
#include "GL/Shader.h"

#include "glm/glm.hpp"
#include "GameCamera.h"
namespace Game
{
	struct FramebufferQuad
	{
		glm::vec3 Position;
		glm::vec2 TexCoords;
	};

	struct FramebufferChainRenderSpecification
	{
		uint32_t width = 800;
		uint32_t height = 600;
		float scale_factor = 1.0f; // It is multiplied by width and height to scale them 
		bool clamp_to_one = true;
	};

	struct FramebufferChainRenderData
	{
		Ref<VertexArray> VA;
		Ref<VertexBuffer> VB;
		Ref<IndexBuffer> IB;
		Ref<Shader> Shader;

		FramebufferQuad Quad;
		FramebufferQuad* Buffer = nullptr;
		FramebufferQuad* BufferPtr = nullptr;

		glm::vec3 Position = { 0.0f,0.0f,0.0f };
		glm::vec3 Scale = { 1.0f,1.0f,1.0f };
		glm::vec3 Rotation = { 0.0f,0.0f,0.0f };
		glm::mat4 Transform;

		SceneCamera RenderCamera;
		glm::mat4 CameraTransform;
	};

	struct Chain;
	// typedef void(*ChainInjectInfoInShaderFunc)(Chain&, const FramebufferChainRenderSpecification&);
	using ChainInjectInfoInShaderFunc = std::function<void(FramebufferChainRenderData&, const FramebufferChainRenderSpecification&)>;

	/*
		The shader must have leave the texture0 slot free
		Use the ChanInjectInfoInShaderFunc to inject info in the shader
	*/
	struct Chain
	{
		FramebufferChainRenderData RenderData;
		ChainInjectInfoInShaderFunc DrawFunc = nullptr;
	};

	class FramebufferChainRender
	{
	public:
		FramebufferChainRender(FramebufferChainRenderSpecification specs = FramebufferChainRenderSpecification());
		~FramebufferChainRender();

		void AddChain(Chain& chain) { m_Chains.push_front(chain); }
		void OnResize(uint32_t w, uint32_t h);

		void Bind() { GetCurrentFramebuffer()->Bind(); }
		void Unbind() { GetCurrentFramebuffer()->Unbind(); };

		void RenderChain();
		
		Ref<Framebuffer>& GetFrontFramebuffer();
		Ref<Framebuffer>& GetBackFramebuffer();
		Ref<Framebuffer>& GetCurrentFramebuffer() { return *m_CurrentFramebuffer; }

		int ReadPixel(uint32_t index, int x, int y) { return GetCurrentFramebuffer()->ReadPixel(index, x, y); }
		void ClearAttachment(uint32_t index, int value) { GetCurrentFramebuffer()->ClearAttachment(index, value); }

	private:
		std::deque<Chain> m_Chains;
		std::array<Ref<Framebuffer>, 2> m_Framebuffers;
		bool m_CurrentFramebufferIndex = false; // Abusing the fact that 0 is false and 1 is true
		Ref<Framebuffer>* m_CurrentFramebuffer = nullptr;
		FramebufferChainRenderSpecification m_Specs;
		FramebufferChainRenderData m_RenderData;
		void SwapFramebuffer();

	private:
		static Ref<Framebuffer> CreateFramebuffer(uint32_t w, uint32_t h, float scalor);
		void SetUpFramebufferChain();
		void InvalidateFramebuffers();
		static void CalculateQuadTransform(Chain& chain);
		Chain& GetFramebufferChain();

	public:
		SceneCamera& GetCamera() { return m_RenderData.RenderCamera; }
		const SceneCamera& GetCamera() const { return m_RenderData.RenderCamera; }

		glm::mat4& GetCameraTransform() { return m_RenderData.CameraTransform; }
		const glm::mat4& GetCameraTransform() const { return m_RenderData.CameraTransform; }

		void SetGLViewport(bool use_scalor);
		void SetScalorFactor(float scalor);
		float GetScalorFactor() const { return m_Specs.scale_factor; };
	};
}
