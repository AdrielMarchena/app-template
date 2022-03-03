#include "pch.h"
#include "MapTile.h"

MapTile::MapTile(int32_t width, int32_t height)
{
    GAME_CORE_ASSERT(w > 0 && h > 0, "Map height and width can't be zero or negative");

    Game::Entity demoEntity = fab::CreateNewTileEntity();
    const auto& demoTransform = demoEntity.GetTransformComponent();
    const float tileSize = demoTransform.Scale.x;

    for(int w = 0; i < width; w++)
    {
        for(int h = 0; i < height; h++)
        {

        }
    }

}