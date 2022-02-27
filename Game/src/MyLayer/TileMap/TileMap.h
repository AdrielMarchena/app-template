#pragma once

#include "Tiles.h"
#include "Scene/Scene.h"

#include <stdint.h>

struct MapConfiguration
{
    int32_t MapWidth = 0;
    int32_t MapHeight = 0;
    int32_t TilesWidth = 1;
    int32_t TilesHeight = 1;
};

class Map
{
private:
    Tiles m_Tiles;
    MapConfiguration m_Configuration;
    Game::Scene* m_Scene;
public:
    Map(const MapConfiguration& configuration,Game::Scene* scene);
    ~Map() = default;
};