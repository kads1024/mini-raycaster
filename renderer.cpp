#define _USE_MATH_DEFINES

#include "renderer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>

#include "utils.h"

namespace
{
/// Nothing has been drawn yet, so every depth pixel starts effectively infinitely far away.
constexpr float depthClearDistance = 1e3f;

/// Ray marching granularity and give-up distance, both in map grid units.
constexpr float rayMarchStepIncrement = 0.01f;
constexpr float rayMarchMaxDistance = 20.0f;

/// Sprites stop growing past this many pixels on screen.
constexpr int maxSpriteScreenSize = 2000;

/// Sprite texels at or below this alpha are treated as fully transparent.
constexpr uint8_t spriteAlphaCutoff = 128;
} // namespace

int wall_x_texcoord(float gridHitX, float gridHitY, const texture &wallTexture)
{
    // local relative to a single texture
    float currentLocalX = gridHitX - std::floor(gridHitX + 0.5f);
    float currentLocalY = gridHitY - std::floor(gridHitY + 0.5f);

    int xCoord = (std::abs(currentLocalX) > std::abs(currentLocalY) ? currentLocalX : currentLocalY) * wallTexture.pixelSize;

    if (xCoord < 0)
        xCoord += wallTexture.pixelSize;

    assert(xCoord >= 0 && xCoord < int(wallTexture.pixelSize));

    return xCoord;
}

void map_show_sprite(const sprite &inSprite, frameBuffer &buffer, const map &gameMap)
{
    const size_t gridWidth = (buffer.width * 0.5f) / gameMap.width; // size of one map cell on the screen
    const size_t gridHeight = buffer.height / gameMap.height;
    buffer.draw_rectangle(inSprite.posX * gridWidth - 3, inSprite.posY * gridHeight - 3, 6, 6, pack_color(255, 0, 0)); // center it
}

void draw_sprite(const sprite &inSprite, depthBuffer &depth, frameBuffer &buffer, const player &mainPlayer, const texture &spriteTexture)
{
    // atan converts vector to angle (in radians). (in relation to +x axis)
    float spriteDir = std::atan2(inSprite.posY - mainPlayer.y, inSprite.posX - mainPlayer.x);

    // atan2 only outputs (-π, π] but mainPlayer.viewDirectionAngle is linear (10deg and 370 deg) is the same
    // we only need to clamp between (-π, π], else large sprite dir delta will result in sprite far away
    while (spriteDir - mainPlayer.viewDirectionAngle > M_PI)
        spriteDir -= 2 * M_PI;
    while (spriteDir - mainPlayer.viewDirectionAngle < -M_PI)
        spriteDir += 2 * M_PI;

    // distance from the player to the sprite (pythagoras
    float spriteDist = std::sqrt(std::pow(mainPlayer.x - inSprite.posX, 2) + std::pow(mainPlayer.y - inSprite.posY, 2));

    // We now have our 2 main components to put sprite in screen, angle relative to player's forward and distance from player

    // size is inversely proportional to distance
    // if i am 1 grid away(16 pixels), cover the whole screen
    // if i am 2 grid away(32 pixels), cover half the screen
    // if i am 3 grid away(48 pixels), cover 1/3 of the screen
    // ...
    // no need to project for fish eye becuase we are only sampling from sprite pos and not per pixel
    // clamp it at 2000 max size
    size_t spriteScreenSize = std::min(maxSpriteScreenSize, static_cast<int>(buffer.height / spriteDist));

    // translate from angle to column (inverse of column to angle of the view angle loop)
    float spriteAngleDeltaFromPlayer = spriteDir - mainPlayer.viewDirectionAngle;   // get the current angle
    int screenWidth = buffer.width / 2;                                             // get the view port width
    const float pixelsPerRadian = static_cast<float>(screenWidth) / mainPlayer.fov; // how many pixels per radian

    // calculation above is mainPlayer.viewDirectionAngle = 0 means it starts at column 0 (buffer.width / 2) so we need
    // to offset it by half of the screen to position it at the center
    float pixelOffsetToCenter = screenWidth / 2;

    // position sprite on the center using pivot 0
    const float centerColumn = spriteAngleDeltaFromPlayer * pixelsPerRadian + pixelOffsetToCenter;

    int horizontalOffset = static_cast<int>(centerColumn - spriteScreenSize / 2); // center sprite by moving pivot to size*0.5f

    int verticalOffset = static_cast<int>(buffer.height) / 2 - static_cast<int>(spriteScreenSize) / 2;

    // draw per column vertically
    for (size_t pixelX = 0; pixelX < spriteScreenSize; pixelX++)
    {
        if (horizontalOffset + int(pixelX) < 0 || horizontalOffset + int(pixelX) >= screenWidth)
            continue; // don't draw out of bounds

        for (size_t pixelY = 0; pixelY < spriteScreenSize; pixelY++)
        {
            if (verticalOffset + int(pixelY) < 0 || verticalOffset + int(pixelY) >= int(buffer.height))
                continue; // don't draw out of bounds

            uint32_t color = spriteTexture.get(
                spriteTexture.pixelSize * static_cast<float>(pixelX) / spriteScreenSize,
                spriteTexture.pixelSize * static_cast<float>(pixelY) / spriteScreenSize,
                inSprite.textureID);
            uint8_t r, g, b, a;
            unpack_color(color, r, g, b, a);
            if (a > spriteAlphaCutoff)
            {
                if (depth.get_pixel(horizontalOffset + pixelX, verticalOffset + pixelY) < spriteDist)
                    continue; // dont draw column behind the current depth
                depth.set_pixel(horizontalOffset + pixelX, verticalOffset + pixelY, spriteDist);
                buffer.set_pixel(screenWidth + horizontalOffset + pixelX, verticalOffset + pixelY, color);
            }
        }
    }
}

void render(frameBuffer &buffer, depthBuffer &depth, const map &gameMap, const player &mainPlayer, const std::vector<sprite> &sprites, const texture &wallTexture, const texture &monsterTexture)
{
    buffer.clear(pack_color(255, 255, 255));
    depth.clear(depthClearDistance);

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
        for (float rayMarchStepSize = 0.0f; rayMarchStepSize < rayMarchMaxDistance; rayMarchStepSize += rayMarchStepIncrement)
        {
            float rayMarchGridStepX = mainPlayer.x + rayMarchStepSize * std::cos(currentFovAngle);
            float rayMarchGridStepY = mainPlayer.y + rayMarchStepSize * std::sin(currentFovAngle);

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
            float projectedDistance = rayMarchStepSize * std::cos(currentFovAngle - mainPlayer.viewDirectionAngle);

            float columnHeight = buffer.height / projectedDistance;

            int xCoord = wall_x_texcoord(rayMarchGridStepX, rayMarchGridStepY, wallTexture);

            std::vector<uint32_t> column = wallTexture.get_scaled_column(textureId, xCoord, columnHeight);

            // draw second half of the screen
            rayMarchStepPixelX = (buffer.width / 2) + fovAngleStep;

            size_t columnStartPixelY = buffer.height / 2 - columnHeight / 2;

            for (size_t columnY = 0; columnY < columnHeight; columnY++)
            {
                rayMarchStepPixelY = columnY + columnStartPixelY;

                if (rayMarchStepPixelY >= 0 && rayMarchStepPixelY < int(buffer.height))
                {
                    depth.set_pixel(fovAngleStep, rayMarchStepPixelY, rayMarchStepSize);
                    buffer.set_pixel(rayMarchStepPixelX, rayMarchStepPixelY, column[columnY]);
                }
            }

            break;
        } // end raymarch
    } // end angle step

    for (const sprite &currentSprite : sprites)
    {
        map_show_sprite(currentSprite, buffer, gameMap);
        draw_sprite(currentSprite, depth, buffer, mainPlayer, monsterTexture);
    }
}
