#pragma once

#include "Render/Camera.h"
#include "GL/Texture.h"
namespace Game
{
	namespace Colors
	{
		constexpr glm::vec4 White{ 1.0f,1.0f,1.0f,1.0f };
	}

#ifdef GAME_USING_ENTITYID
	#define GAME_ENTITY_ID entityID
	#define GAME_DECLARE_ENTITY_ID int GAME_ENTITY_ID
	#define GAME_DO_IF_ENTITYID(x) x
	#define GAME_COMMA_ENTITYID(x) ,GAME_DO_IF_ENTITYID(x)
#else
	#define GAME_ENTITY_ID
	#define GAME_DECLARE_ENTITY_ID
	#define GAME_COMMA_ENTITYID(x)
	#define GAME_DO_IF_ENTITYID(x)
#endif

	enum class DrawInfo
	{
		Quad,
		Circle
	};
	struct RenderStats;
	struct DrawGeometryData;
	struct DrawDataCollection;
	/**
	* Render2D class.
	*/
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

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color GAME_COMMA_ENTITYID(GAME_DECLARE_ENTITY_ID));
		static void DrawQuad(const glm::mat4& transform, Ref<Texture> texture, const glm::vec4& color GAME_COMMA_ENTITYID(GAME_DECLARE_ENTITY_ID));

		static void DrawCircle(const glm::mat4& transform, float thick, float fade,const glm::vec4& color GAME_COMMA_ENTITYID(GAME_DECLARE_ENTITY_ID));
		static void DrawCircle(const glm::mat4& transform, float thick, float fade, Ref<Texture> texture, const glm::vec4& color GAME_COMMA_ENTITYID(GAME_DECLARE_ENTITY_ID));

		static RenderStats GetRenderInfo(const DrawInfo& geometry = DrawInfo::Quad);

	};
}



