#pragma once
#include "framebuffer.h"
#include "map.h"
#include "player.h"
#include "sprite.h"
#include "texture.h"

void render(frameBuffer &buffer, depthBuffer &depth, map &gameMap, player &mainPlayer, std::vector<sprite> &sprites, texture &wallTexture, texture &monsterTexture);