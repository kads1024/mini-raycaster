#pragma once
#include <cstddef>

/// @brief A billboard drawn at a position on the map grid.
struct sprite
{
    /// @brief Grid position X
    float posX;
    /// @brief Grid position Y
    float posY;
    size_t textureID;
};
