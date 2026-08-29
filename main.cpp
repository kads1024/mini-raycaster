#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>

#include "renderer.h"
#include "utils.h"

int main()
{
    frameBuffer buffer;
    buffer.width = 1024;
    buffer.height = 512;
    buffer.clear(pack_color(255, 255, 255));

    depthBuffer depth;
    depth.width = 512;
    depth.height = 512;

    player mainPlayer{3.456f, 2.345f, 1.523f, M_PI / 3.0};

    map gameMap;

    texture wallTextures("./walltext.png");
    texture monsterTextures("./monsters.png");
    if (!wallTextures.count || !monsterTextures.count)
    {
        std::cerr << "FAILED TO LOAD TEXTURES\n";
        return -1;
    }

    size_t gridWidth = (buffer.width / gameMap.width) * 0.5f;
    size_t gridHeight = buffer.height / gameMap.height;

    std::vector<sprite> sprites{{3.523, 3.812, 2}, {1.834, 8.765, 0}, {5.323, 5.365, 1}, {4.123, 10.265, 1}}; // gridPositions
    render(buffer, depth, gameMap, mainPlayer, sprites, wallTextures, monsterTextures);

    drop_ppm_image("./out.ppm", buffer.data, buffer.width, buffer.height);

    return 0;
}
