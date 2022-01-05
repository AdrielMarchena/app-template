#pragma once

#include "Render/Camera.h"
#include "GL/Texture.h"
namespace Game
{
	namespace Colors
	{
		constexpr glm::vec4 White{ 1.0f,1.0f,1.0f,1.0f };
	}

	//TODO: Make this without looking up

	/**
	* Render2D class.
	*/
	enum class DrawInfo
	{
		Quad
	};
	struct RenderStats;
	struct DrawGeometryData;
	struct DrawDataCollection;

	class Render2D
	{
	private:

	public:
		static bool Init();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);

		static void Clear();

		static void BeginBatch();
		static void EndBatch();
		static void Flush();

		//static void EndScene();

		static void Dispose();

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, Ref<Texture> texture, const glm::vec4& color = Colors::White);

		static RenderStats GetRenderInfo(const DrawInfo& geometry = DrawInfo::Quad);

	};
}



