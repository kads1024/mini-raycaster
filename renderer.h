#pragma once
#include <vector>

#include "framebuffer.h"
#include "map.h"
#include "player.h"
#include "sprite.h"
#include "texture.h"

/// @brief Draws one frame: the top-down map on the left half of the buffer, the
/// first-person view on the right half. Writes wall and sprite distances into depth.
void render(frameBuffer &buffer,
            depthBuffer &depth,
            const map &gameMap,
            const player &mainPlayer,
            const std::vector<sprite> &sprites,
            const texture &wallTexture,
            const texture &monsterTexture);
