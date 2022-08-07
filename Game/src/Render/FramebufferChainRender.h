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

	struct FramebufferRenderSpecification
	{
		uint32_t Width = 800;
		uint32_t Height = 600;
		float ScaleFactor = 1.0f; // It is multiplied by width and height to scale them 
	};

	struct FramebufferChainRenderData
	{
		Ref<VertexArray> VA;
		Ref<VertexBuffer> VB;
		Ref<IndexBuffer> IB;
		Ref<Shader> CShader;
		Ref<Framebuffer> Frambuffer;
		FramebufferRenderSpecification FramebufferSpecifications;

		FramebufferQuad Quad;
		FramebufferQuad* Buffer = nullptr;
		FramebufferQuad* BufferPtr = nullptr;

		glm::vec3 Position = { 0.0f,0.0f,0.0f };
		glm::vec3 Scale = { 1.0f,1.0f,1.0f };
		glm::vec3 Rotation = { 0.0f,0.0f,0.0f };
		glm::mat4 Transform;

		SceneCamera RenderCamera;
		glm::mat4 CameraTransform;

	private:
		void InvalidateFramebuffer();
		friend class FramebufferChainRender;
	};

	struct Chain;
	// typedef void(*ChainInjectInfoInShaderFunc)(Chain&, const FramebufferChainRenderSpecification&);
	using ChainInjectInfoInShaderFunc = std::function<void(Chain& self, FramebufferChainRenderData&)>;
	using ChainOnResizeFunc = std::function<void(Chain& self, uint32_t w, uint32_t h)>;

	/*
		The shader must have the texture0 slot free
		Use the ChanInjectInfoInShaderFunc to inject info in the shader
	*/
	struct Chain
	{
		bool Ignore = false;
		FramebufferChainRenderData RenderData;
		ChainInjectInfoInShaderFunc DrawFunc = nullptr;
		ChainOnResizeFunc OnResizeFunc = nullptr;
	};

	class FramebufferChainRender
	{
	public:
		FramebufferChainRender
		(
			FramebufferRenderSpecification sceneSpecs = FramebufferRenderSpecification(),
			FramebufferRenderSpecification screenSpecs = FramebufferRenderSpecification()
		);
		~FramebufferChainRender();

		void AddChain(Chain& chain) { m_Chains.push_front(chain); }
		void OnResize(uint32_t w, uint32_t h);

		void BindSceneFramebuffer();
		void UnbindSceneFramebuffer();

		void RenderChain();
		
		int ReadPixel(uint32_t index, int x, int y) { return m_SceneChain.RenderData.Frambuffer->ReadPixel(index, x, y); }
		void ClearAttachment(uint32_t index, int value) { m_SceneChain.RenderData.Frambuffer->ClearAttachment(index, value); }
		static void CalculateQuadTransform(Chain& chain);

	private:
		std::deque<Chain> m_Chains;

		// Chain* m_ScreenChain = nullptr;
		Chain m_SceneChain;
	private:
		void SetUpScreenFramebufferChain();
		void SetUpSceneFramebufferChain();
		void InvalidateFramebuffers();

	public:
		static void StandardChainOnResizeCallback(Chain& self, uint32_t w, uint32_t h);
		static Ref<Framebuffer> CreateFramebuffer(uint32_t w, uint32_t h, float scalor);
		
		SceneCamera& GetCamera() { return m_SceneChain.RenderData.RenderCamera; }
		const SceneCamera& GetCamera() const { return m_SceneChain.RenderData.RenderCamera; }

		glm::mat4& GetCameraTransform() { return m_SceneChain.RenderData.CameraTransform; }
		const glm::mat4& GetCameraTransform() const { return m_SceneChain.RenderData.CameraTransform; }

		static void SetGLViewport(Chain& chain);
	};
}
