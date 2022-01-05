#pragma once

#include "Core/Core.h"
#include "GL_Include.h"
#include "glm/glm.hpp"
#include "Framebuffer.h"
#include "Texture.h"
#include "Render/Camera.h"
#include <unordered_map>
namespace Game
{
	struct FramebufferQuad
	{
		glm::vec3 Position;
		glm::vec2 TexCoords;
	};

	struct FrameBufferRenderSpecification
	{
		uint32_t width = 800;
		uint32_t height = 600;
		float scale_factor = 1.0f; //Is multiplied by width and height to scale them 
		bool clamp_to_one = true;
	};

	struct FramebufferPostEffect
	{
		bool active = false;
		std::vector<glm::vec2> offsets;
		std::vector<float> kernel;
	};

	class FramebufferRender
	{
	private:
		FrameBufferRenderSpecification m_Specs;
		Ref<VertexArray> m_VA;
		Ref<VertexBuffer> m_VB;
		Ref<IndexBuffer> m_IB;
		Scope<ShaderLib> m_Shaders;
		Ref<Shader> m_CurrentShader;
		Ref<Framebuffer> m_Framebuffer;

		FramebufferQuad m_Quad;

		FramebufferQuad* m_Buffer = nullptr;
		FramebufferQuad* m_BufferPtr = nullptr;

		std::unordered_map<std::string, FramebufferPostEffect> m_PostEffects;
		FramebufferPostEffect* m_CurrentPostEffect = nullptr;

		glm::vec3 m_Position = {0.0f,0.0f,0.0f};
		glm::vec3 m_Scale = {1.0f,1.0f,1.0f};
		glm::vec3 m_Rotation = {0.0f,0.0f,0.0f};
		glm::mat4 m_Transform;
	public:
		FramebufferRender(const FrameBufferRenderSpecification& spec);
		~FramebufferRender();

		void DrawFrameBuffer(const Camera& camera, const glm::mat4& camera_transform);

		void InvalidateFrameBuffer();

		void BindFrameBuffer();
		void UnbindFrameBuffer();

		const FrameBufferRenderSpecification& GetSpec() const { return m_Specs; }
		FrameBufferRenderSpecification& GetSpec() { return m_Specs; }

		/* If name is empty, will use shader.name */
		void AddShader(const Ref<Shader>& shader, const std::string& name = std::string());
		void UseShader(const std::string& name);

		/* If name is empty, will use texture.name */
		void AddLookUpTable(const Ref<Texture>& texture, const std::string& name = std::string());
		void UseLookUpTable(const std::string& name);

		int ReadPixel(uint32_t index, int x, int y) { return m_Framebuffer->ReadPixel(index, x, y); }
		void ClearAttachment(uint32_t index, int value) { m_Framebuffer->ClearAttachment(index,value); }

		void UsePostEffect(const std::string& name);
		const std::unordered_map<std::string, FramebufferPostEffect>& GetPostEffects() const { return m_PostEffects; }

		void SetQuadPosition(const glm::vec3& position) { m_Position = position; CalculateQuadTransform(); }
		void SetQuadScale(const glm::vec3& scale) {		  m_Scale = scale;		 CalculateQuadTransform(); }
		void SetQuadRotation(const glm::vec3& rotation) { m_Rotation = rotation; CalculateQuadTransform(); }

		uint32_t GetFramebufferImage() const { return m_Framebuffer->GetColorTexture(); }

		Ref <Shader> GetCurrentShader() const { return m_CurrentShader; }

		void SetGLViewport(bool use_scalor);
	private:
		void CalculateQuadTransform();

		void SetUpFramebuffer();
		void SetUpShader();
		void SetUpPostEffects();
		void UpdatePostEffects();
	};

}