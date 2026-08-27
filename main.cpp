#define _USE_MATH_DEFINES
#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

uint32_t pack_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
{
    return r + (g << 8) + (b << 16) + (a << 24);
}

void unpack_color(uint32_t inColor, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
{
    r = inColor & 255;
    g = (inColor >> 8) & 255;
    b = (inColor >> 16) & 255;
    a = (inColor >> 24) & 255;
}

void drop_ppm_image(const char* fileName, const std::vector<uint32_t>& buffer, size_t w, size_t h)
{
    assert(buffer.size() == w * h);
    std::ofstream outFile(fileName, std::ios::binary);
    outFile << "P6\n";
    outFile << w << " " << h << "\n";
    outFile << "255\n";

    for (size_t pixel = 0; pixel < buffer.size(); pixel++)
    {
        uint8_t r, g, b, a;
        unpack_color(buffer[pixel], r, g, b, a);
        outFile << static_cast<char>(r) << static_cast<char>(g) << static_cast<char>(b);
    }
    outFile.close();
}

void draw_rectangle(std::vector<uint32_t>& buffer, size_t bufferWidth, size_t bufferHeight, 
    size_t startPixelX, size_t startPixelY, size_t rectWidth, size_t rectHeight,
    uint32_t rectColor)
{
    assert(buffer.size() == bufferWidth * bufferHeight);

    for (size_t rectPixelY = 0; rectPixelY < rectHeight; rectPixelY++)
    {
        for (size_t rectPixelX = 0; rectPixelX < rectWidth; rectPixelX++)
        {
            size_t currentPixelX = startPixelX + rectPixelX;
            size_t currentPixelY = startPixelY + rectPixelY;

            if (currentPixelX >= bufferWidth || currentPixelY >= bufferHeight) 
                continue;

            buffer[currentPixelY * bufferWidth + currentPixelX] = rectColor;
        }
    }
}

int main()
{
    size_t bufferWidth = 1024;
    size_t bufferHeight = 512;
    std::vector<uint32_t> frameBuffer(bufferWidth * bufferHeight, pack_color(255, 255, 255));

    constexpr size_t mapWidth = 16;
    constexpr size_t mapHeight = 16;
    const char map[mapWidth * mapHeight + 1] = // +1 is the null termination
        "0000222222220000"
        "1              0"
        "1      11111   0"
        "1     0        0"
        "0     0  1110000"
        "0     3        0"
        "0   10000      0"
        "0   0   11100  0"
        "0   0   0      0"
        "0   0   1  00000"
        "0       1      0"
        "2       1      0"
        "0       0      0"
        "0 0000000      0"
        "0              0"
        "0002222222200000";

    float playerPosX = 3.456f;
    float playerPosY = 2.345f;
    float playerViewAngle = 1.523f; // in radians
    float fov = M_PI / 3.0f;

    size_t gridWidth = (bufferWidth / mapWidth) * 0.5f;
    size_t gridHeight = bufferHeight / mapHeight;

    // draw map
    for(size_t gridY = 0; gridY < mapHeight; gridY++)
    {
        for (size_t gridX = 0; gridX < mapWidth; gridX++)
        {
            if(map[gridX + gridY*mapWidth] == ' ') 
                continue;

            size_t rectStartPixelX = gridX * gridWidth;
            size_t rectStartPixelY = gridY * gridHeight;
            draw_rectangle(frameBuffer, bufferWidth, bufferHeight, rectStartPixelX, rectStartPixelY, gridWidth, gridHeight, pack_color(0, 255, 255));
        }
    }

    float startFovAngle = playerViewAngle - fov / 2.0f;
    for(size_t fovAngleStep = 0; fovAngleStep < bufferWidth / 2; fovAngleStep++) // loop through each angle 
    {
        float currentFovAngle = startFovAngle + fov * (static_cast<float>(fovAngleStep) / (bufferWidth / 2)); // divide fov angle by how many pixels horizontally

        // rayMarchStepSize is also the distance to the player; values are in map grid coords
        for (float rayMarchStepSize = 0.0f; rayMarchStepSize < 20.0f; rayMarchStepSize += 0.05f)
        {
            float rayMarchGridStepX = playerPosX + rayMarchStepSize * cos(currentFovAngle);
            float rayMarchGridStepY = playerPosY + rayMarchStepSize * sin(currentFovAngle);

            size_t rayMarchStepPixelX = rayMarchGridStepX * gridWidth;
            size_t rayMarchStepPixelY = rayMarchGridStepY * gridHeight;

            frameBuffer[rayMarchStepPixelX + rayMarchStepPixelY * bufferWidth] = pack_color(160, 160, 160);

            // if hits a wall, draw the second half of the screen
            if (map[static_cast<int>(rayMarchGridStepX) + static_cast<int>(rayMarchGridStepY) * mapWidth] != ' ')
            {
                // if i am 1 grid away(16 pixels), cover the whole screen
                // if i am 2 grid away(32 pixels), cover half the screen
                // if i am 3 grid away(48 pixels), cover 1/3 of the screen
                // ...
                float columnHeight = bufferHeight / rayMarchStepSize;  
                draw_rectangle(frameBuffer, bufferWidth, bufferHeight, (bufferWidth / 2 + fovAngleStep), bufferHeight/2 - columnHeight/2, 1, columnHeight, pack_color(0, 255, 255));
                break;
            }
        }
    }

    drop_ppm_image("./out.ppm", frameBuffer, bufferWidth, bufferHeight);

    return 0;
}
