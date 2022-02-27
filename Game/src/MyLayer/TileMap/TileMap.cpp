#include "pch.h"
#include "TileMap.h"

#include "Core/Assert.h"
#include <string>

#include "TileScript.h"

Map::Map(const MapConfiguration& configuration, Game::Scene* scene)
:m_Configuration(configuration), m_Scene(scene)
{
    GAME_CORE_ASSERT(m_Configuration.MapWidth > 0 && m_Configuration.MapHeight > 0, "Map can't have a 0 width or height");
    GAME_CORE_ASSERT(m_Configuration.TilesWidth > 0 && m_Configuration.TilesHeight > 0, "Tiles can't have a 0 width or height");
    GAME_CORE_ASSERT(m_Scene,"Scene can't be nullptr");

    m_Tiles.resize(m_Configuration.MapHeight);
    int32_t currentRow = 0;
    for(auto& row : m_Tiles)
    {
        int32_t currentCol = 0;
        row.reserve(m_Configuration.MapWidth);
        for(int r = 0; r <= m_Configuration.MapWidth;r++)
        {
            row.push_back(m_Scene->CreateEntity("Tile_"+ std::to_string(currentCol) + "X" + std::to_string(currentRow)));

            auto& scriptComp = row[0].Add<Game::NativeScriptClass>();
            scriptComp.Bind<TileScript>();
            
            row[0].Add<Game::SpriteComponent>(glm::vec4(1.0f,1.0f,1.0f,0.0f)).Visible = false;

            currentCol++;
        }
        currentRow++;
    }
}
