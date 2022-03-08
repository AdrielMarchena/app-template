/*#include "pch.h"
#include "TileFabric.h"

#include "Scene/Components.h"

#include <string>

namespace fab
{
    Game::Entity CreateNewTileEntity(int32_t index)
    {
        auto scene = Game::Scene::GetCurrentSceneRef();
        GAME_CORE_ASSERT(scene,"Scene is not valid");

        const std::string tag = "Tile_" + std::to_string(index);

		Game::Entity ent = Scene->CreateEntity(tag);
		ent.Add<Game::SpriteComponent>({1.0f,1.0f,1.0f,0.0f});
        ent.GetTransformComponent().Scale *= 0.25f;
		auto& rigBody = ent.Add<Game::RigidBody2DComponent>();
		auto& boxColider = ent.Add<Game::BoxColider2DComponent>();
		rigBody.Type = Game::RigidBody2DComponent::BodyType::Dynamic;
    }
}*/