#pragma once

#include "TileFabric.h"

class MapTile
{
private:
    Tiles m_Tiles;
public:
    MapTile(int32_t width, int32_t height);
    ~MapTile() = default;
}