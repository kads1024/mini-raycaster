#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "framebuffer.h"
#include "map.h"
#include "player.h"
#include "stb_image.h"
#include "utils.h"

bool load_texture(const std::string &fileName, std::vector<uint32_t> &outTexture, size_t &outTextureSize, size_t &outTextureCount)
{
    int numChannels = -1;
    int textureWidth;
    int textureHeight;

    unsigned char *pixelMap = stbi_load(fileName.c_str(), &textureWidth, &textureHeight, &numChannels, 0);
    if (!pixelMap)
    {
        std::cerr << "Error: can not load the textures" << std::endl;
        return false;
    }

    if (4 != numChannels)
    {
        std::cerr << "Error: the texture must be a 32 bit image" << std::endl;
        stbi_image_free(pixelMap);
        return false;
    }

    outTextureCount = textureWidth / textureHeight; // must be square
    outTextureSize = textureWidth / outTextureCount;

    if (textureWidth != textureHeight * int(outTextureCount)) // width must be equal to count*height
    {
        std::cerr << "Error: the texture file must contain N square textures packed horizontally" << std::endl;
        stbi_image_free(pixelMap);
        return false;
    }

    outTexture = std::vector<uint32_t>(textureWidth * textureHeight);
    for (size_t texturePixelY = 0; texturePixelY < textureHeight; texturePixelY++)
    {
        for (size_t texturePixelX = 0; texturePixelX < textureWidth; texturePixelX++)
        {
            // 4 bytes per color
            uint8_t r = pixelMap[(texturePixelX + texturePixelY * textureWidth) * 4 + 0];
            uint8_t g = pixelMap[(texturePixelX + texturePixelY * textureWidth) * 4 + 1];
            uint8_t b = pixelMap[(texturePixelX + texturePixelY * textureWidth) * 4 + 2];
            uint8_t a = pixelMap[(texturePixelX + texturePixelY * textureWidth) * 4 + 3];
            outTexture[texturePixelX + texturePixelY * textureWidth] = pack_color(r, g, b, a);
        }
    }
    stbi_image_free(pixelMap);
    return true;
}

std::vector<uint32_t> textureColumn(const std::vector<uint32_t> &inTexture, size_t textureSize, size_t textureCount, size_t textureID, size_t xCoord, size_t columnHeight)
{
    size_t textureMapWidth = textureSize * textureCount;
    size_t textureMapHeight = textureSize;

    assert((inTexture.size() == textureMapWidth * textureMapHeight) && xCoord < textureSize && textureID < textureCount);

    std::vector<uint32_t> column(columnHeight);

    size_t pixelX = textureID * textureSize + xCoord;
    for (size_t y = 0; y < columnHeight; y++)
    {
        size_t pixelY = textureSize * (static_cast<float>(y) / columnHeight);
        column[y] = inTexture[pixelX + pixelY * textureMapWidth];
    }

    return column;
}

int main()
{
    frameBuffer buffer;
    buffer.width = 1024;
    buffer.height = 512;
    buffer.clear(pack_color(255, 255, 255));

    map gameMap;

    player mainPlayer{ 3.456f, 2.345f, 1.523f, M_PI / 3.0 };

    // size_t numColors = 10;
    // std::vector<uint32_t> colors(numColors);
    // for (size_t i = 0; i < numColors; i++)
    // {
    //     colors[i] = pack_color(rand() % 255, rand() % 255, rand() % 255);
    // }

    std::vector<uint32_t> wallTextures;
    size_t wallTextureSize;
    size_t wallTextureCount;
    if (!load_texture("./walltext.png", wallTextures, wallTextureSize, wallTextureCount))
    {
        std::cerr << "FAILED TO LOAD walltext.png\n";
        return -1;
    }

    size_t gridWidth = (buffer.width / gameMap.width) * 0.5f;
    size_t gridHeight = buffer.height / gameMap.height;

    // animation loop
    // for(size_t frame = 0; frame < 360; frame++)
    // {
    // }

    // animation loop

    // std::stringstream ss;
    // ss << "./out/" << std::setfill('0') << std::setw(5) << frame << ".ppm";
    // playerViewAngle += 2.0f * M_PI / 360.0f;                                                    // iterate by 1deg in rad
    // frameBuffer = std::vector<uint32_t>(bufferWidth * bufferHeight, pack_color(255, 255, 255)); // clear

    // draw map
    for (size_t gridY = 0; gridY < gameMap.height; gridY++)
    {
        for (size_t gridX = 0; gridX < gameMap.width; gridX++)
        {
            if (gameMap.is_cell_empty(gridX, gridY))
                continue;

            size_t rectStartPixelX = gridX * gridWidth;
            size_t rectStartPixelY = gridY * gridHeight;
            size_t textureId = gameMap.get(gridX, gridY);
            assert(textureId < wallTextureCount);
            buffer.draw_rectangle(rectStartPixelX, rectStartPixelY, gridWidth, gridHeight, wallTextures[textureId * wallTextureSize]);
        }
    }

    float startFovAngle = mainPlayer.viewDirectionAngle - mainPlayer.fov / 2.0f;
    for (size_t fovAngleStep = 0; fovAngleStep < buffer.width / 2; fovAngleStep++) // loop through each angle
    {
        float currentFovAngle = startFovAngle + mainPlayer.fov * (static_cast<float>(fovAngleStep) / (buffer.width / 2)); // divide fov angle by how many pixels horizontally

        // rayMarchStepSize is also the distance to the player; values are in map grid coords
        for (float rayMarchStepSize = 0.0f; rayMarchStepSize < 20.0f; rayMarchStepSize += 0.01f)
        {
            float rayMarchGridStepX = mainPlayer.x + rayMarchStepSize * cos(currentFovAngle);
            float rayMarchGridStepY = mainPlayer.y + rayMarchStepSize * sin(currentFovAngle);

            int rayMarchStepPixelX = rayMarchGridStepX * gridWidth;
            int rayMarchStepPixelY = rayMarchGridStepY * gridHeight;

            buffer.data[rayMarchStepPixelX + rayMarchStepPixelY * buffer.width] = pack_color(160, 160, 160);

            // if hits a wall, draw the second half of the screen
            if (!gameMap.is_cell_empty(rayMarchGridStepX, rayMarchGridStepY))
            {
                size_t textureId = gameMap.get(rayMarchGridStepX, rayMarchGridStepY);
                assert(textureId < wallTextureCount);

                // if i am 1 grid away(16 pixels), cover the whole screen
                // if i am 2 grid away(32 pixels), cover half the screen
                // if i am 3 grid away(48 pixels), cover 1/3 of the screen
                // ...
                float columnHeight = buffer.height / (rayMarchStepSize * cos(currentFovAngle - mainPlayer.viewDirectionAngle));
                // draw_rectangle(frameBuffer, bufferWidth, bufferHeight, (bufferWidth / 2 + fovAngleStep), bufferHeight / 2 - columnHeight / 2, 1, columnHeight, wallTextures[textureId * wallTextureSize]);
                float hitX = rayMarchGridStepX - floor(rayMarchGridStepX + 0.5f);
                float hitY = rayMarchGridStepY - floor(rayMarchGridStepY + 0.5f);

                int xCoord = (abs(hitX) > abs(hitY) ? hitX : hitY) * wallTextureSize;

                if (xCoord < 0)
                    xCoord += wallTextureSize;

                assert(xCoord >= 0 && xCoord < wallTextureSize);

                std::vector<uint32_t> column = textureColumn(wallTextures, wallTextureSize, wallTextureCount, textureId, xCoord, columnHeight);

                // draw second half of the screen
                rayMarchStepPixelX = (buffer.width / 2) + fovAngleStep;

                for (size_t columnY = 0; columnY < columnHeight; columnY++)
                {
                    size_t columnStartPixelY = buffer.height / 2 - columnHeight / 2;

                    rayMarchStepPixelY = columnY + columnStartPixelY;

                    if (rayMarchStepPixelY < 0 || rayMarchStepPixelY >= buffer.height)
                        continue;

                    buffer.data[rayMarchStepPixelX + rayMarchStepPixelY * buffer.width] = column[columnY];
                }

                break;
            } // if hit wall
        } // end raymarch
    } // end angle step

    // std::cout << "\033[H\033[2J"; // clear console
    // std::cout << static_cast<int>((frame / 360.0f) * 100) << "%";

    // size_t textureId = 4;
    // for(size_t texturePixelY = 0; texturePixelY < wallTextureSize; texturePixelY++)
    // {
    //     for (size_t texturePixelX = 0; texturePixelX < wallTextureSize; texturePixelX++)
    //     {
    //         size_t startPixelX = wallTextureSize * textureId;

    //         frameBuffer[texturePixelX + texturePixelY * bufferWidth] =
    //             wallTextures[(startPixelX + texturePixelX) + texturePixelY * (wallTextureSize * wallTextureCount)];
    //     }
    // }

    drop_ppm_image("./out.ppm", buffer.data, buffer.width, buffer.height);
    return 0;
}
