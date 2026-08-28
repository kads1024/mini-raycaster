#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "framebuffer.h"
#include "map.h"
#include "player.h"
#include "stb_image.h"
#include "texture.h"
#include "utils.h"

void render(frameBuffer &buffer, map &gameMap, player &mainPlayer, texture &wallTexture)
{
    buffer.clear(pack_color(255, 255, 255));

    size_t gridWidth = (buffer.width / gameMap.width) * 0.5f;
    size_t gridHeight = buffer.height / gameMap.height;

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
            assert(textureId < wallTexture.count);
            buffer.draw_rectangle(rectStartPixelX, rectStartPixelY, gridWidth, gridHeight, wallTexture.get(0, 0, textureId));
        }
    }

    // second screen
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

            buffer.set_pixel(rayMarchStepPixelX, rayMarchStepPixelY, pack_color(160, 160, 160));

            // if hits a wall, draw the second half of the screen
            if (gameMap.is_cell_empty(rayMarchGridStepX, rayMarchGridStepY))
                continue;

            size_t textureId = gameMap.get(rayMarchGridStepX, rayMarchGridStepY);
            assert(textureId < wallTexture.count);

            // if i am 1 grid away(16 pixels), cover the whole screen
            // if i am 2 grid away(32 pixels), cover half the screen
            // if i am 3 grid away(48 pixels), cover 1/3 of the screen
            // ...
            float columnHeight = buffer.height / (rayMarchStepSize * cos(currentFovAngle - mainPlayer.viewDirectionAngle));
            float hitX = rayMarchGridStepX - floor(rayMarchGridStepX + 0.5f);
            float hitY = rayMarchGridStepY - floor(rayMarchGridStepY + 0.5f);

            int xCoord = (abs(hitX) > abs(hitY) ? hitX : hitY) * wallTexture.pixelSize; // TODO: change to wall_x_coord

            if (xCoord < 0)
                xCoord += wallTexture.pixelSize;

            assert(xCoord >= 0 && xCoord < wallTexture.pixelSize);

            std::vector<uint32_t> column = wallTexture.get_scaled_column(textureId, xCoord, columnHeight);

            // draw second half of the screen
            rayMarchStepPixelX = (buffer.width / 2) + fovAngleStep;

            for (size_t columnY = 0; columnY < columnHeight; columnY++)
            {
                size_t columnStartPixelY = buffer.height / 2 - columnHeight / 2;

                rayMarchStepPixelY = columnY + columnStartPixelY;

                if (rayMarchStepPixelY >= 0 && rayMarchStepPixelY < buffer.height)
                {
                    buffer.set_pixel(rayMarchStepPixelX, rayMarchStepPixelY, column[columnY]);
                }   
            }

            break;
        } // end raymarch
    } // end angle step
}

int main()
{
    frameBuffer buffer;
    buffer.width = 1024;
    buffer.height = 512;
    buffer.clear(pack_color(255, 255, 255));

    player mainPlayer{3.456f, 2.345f, 1.523f, M_PI / 3.0};

    map gameMap;

    texture wallTextures("./walltext.png");

    if (wallTextures.count <= 0)
    {
        std::cerr << "FAILED TO LOAD walltext.png\n";
        return -1;
    }

    size_t gridWidth = (buffer.width / gameMap.width) * 0.5f;
    size_t gridHeight = buffer.height / gameMap.height;

    // animation loop
    for (size_t frame = 0; frame < 360; frame++)
    {
        std::stringstream ss;
        ss << "./out/" << std::setfill('0') << std::setw(5) << frame << ".ppm";
        mainPlayer.viewDirectionAngle += 2.0f * M_PI / 360.0f; // iterate by 1deg in rad
        buffer.clear(pack_color(255, 255, 255));

        render(buffer, gameMap, mainPlayer, wallTextures);
         // draw map
        // for (size_t gridY = 0; gridY < gameMap.height; gridY++)
        // {
        //     for (size_t gridX = 0; gridX < gameMap.width; gridX++)
        //     {
        //         if (gameMap.is_cell_empty(gridX, gridY))
        //             continue;

        //         size_t rectStartPixelX = gridX * gridWidth;
        //         size_t rectStartPixelY = gridY * gridHeight;
        //         size_t textureId = gameMap.get(gridX, gridY);
        //         assert(textureId < wallTextureCount);
        //         buffer.draw_rectangle(rectStartPixelX, rectStartPixelY, gridWidth, gridHeight, wallTextures[textureId * wallTextureSize]);
        //     }
        // }

        // float startFovAngle = mainPlayer.viewDirectionAngle - mainPlayer.fov / 2.0f;
        // for (size_t fovAngleStep = 0; fovAngleStep < buffer.width / 2; fovAngleStep++) // loop through each angle
        // {
        //     float currentFovAngle = startFovAngle + mainPlayer.fov * (static_cast<float>(fovAngleStep) / (buffer.width / 2)); // divide fov angle by how many pixels horizontally

        //     // rayMarchStepSize is also the distance to the player; values are in map grid coords
        //     for (float rayMarchStepSize = 0.0f; rayMarchStepSize < 20.0f; rayMarchStepSize += 0.01f)
        //     {
        //         float rayMarchGridStepX = mainPlayer.x + rayMarchStepSize * cos(currentFovAngle);
        //         float rayMarchGridStepY = mainPlayer.y + rayMarchStepSize * sin(currentFovAngle);

        //         int rayMarchStepPixelX = rayMarchGridStepX * gridWidth;
        //         int rayMarchStepPixelY = rayMarchGridStepY * gridHeight;

        //         buffer.data[rayMarchStepPixelX + rayMarchStepPixelY * buffer.width] = pack_color(160, 160, 160);

        //         // if hits a wall, draw the second half of the screen
        //         if (!gameMap.is_cell_empty(rayMarchGridStepX, rayMarchGridStepY))
        //         {
        //             size_t textureId = gameMap.get(rayMarchGridStepX, rayMarchGridStepY);
        //             assert(textureId < wallTextureCount);

        //             // if i am 1 grid away(16 pixels), cover the whole screen
        //             // if i am 2 grid away(32 pixels), cover half the screen
        //             // if i am 3 grid away(48 pixels), cover 1/3 of the screen
        //             // ...
        //             float columnHeight = buffer.height / (rayMarchStepSize * cos(currentFovAngle - mainPlayer.viewDirectionAngle));
        //             // draw_rectangle(frameBuffer, bufferWidth, bufferHeight, (bufferWidth / 2 + fovAngleStep), bufferHeight / 2 - columnHeight / 2, 1, columnHeight, wallTextures[textureId * wallTextureSize]);
        //             float hitX = rayMarchGridStepX - floor(rayMarchGridStepX + 0.5f);
        //             float hitY = rayMarchGridStepY - floor(rayMarchGridStepY + 0.5f);

        //             int xCoord = (abs(hitX) > abs(hitY) ? hitX : hitY) * wallTextureSize;

        //             if (xCoord < 0)
        //                 xCoord += wallTextureSize;

        //             assert(xCoord >= 0 && xCoord < wallTextureSize);

        //             std::vector<uint32_t> column = textureColumn(wallTextures, wallTextureSize, wallTextureCount, textureId, xCoord, columnHeight);

        //             // draw second half of the screen
        //             rayMarchStepPixelX = (buffer.width / 2) + fovAngleStep;

        //             for (size_t columnY = 0; columnY < columnHeight; columnY++)
        //             {
        //                 size_t columnStartPixelY = buffer.height / 2 - columnHeight / 2;

        //                 rayMarchStepPixelY = columnY + columnStartPixelY;

        //                 if (rayMarchStepPixelY < 0 || rayMarchStepPixelY >= buffer.height)
        //                     continue;

        //                 buffer.data[rayMarchStepPixelX + rayMarchStepPixelY * buffer.width] = column[columnY];
        //             }

        //             break;
        //         } // if hit wall
        //     } // end raymarch
        // } // end angle step

        std::cout << "\033[H\033[2J"; // clear console
        std::cout << static_cast<int>((frame / 360.0f) * 100) << "%";

        drop_ppm_image("./out.ppm", buffer.data, buffer.width, buffer.height);
    }

    return 0;
}
