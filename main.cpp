#include <cassert>
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
            assert(currentPixelX < bufferWidth && currentPixelY < bufferHeight);
            buffer[currentPixelY * bufferHeight + currentPixelX] = rectColor;
        }
    }
}

int main()
{
    size_t bufferWidth = 512;
    size_t bufferHeight = 512;
    std::vector<uint32_t> frameBuffer(bufferWidth * bufferHeight, 255);

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

    for (size_t pixelY = 0; pixelY < bufferHeight; pixelY++)
    {
        for (size_t pixelX = 0; pixelX < bufferWidth; pixelX++)
        {
            uint8_t r = ((static_cast<float>(pixelY) / bufferHeight) * 255);
            uint8_t g = ((static_cast<float>(pixelX) / bufferWidth) * 255);
            uint8_t b = 0;
            frameBuffer[pixelX + pixelY * bufferWidth] = pack_color(r, g, b);
        }
    }

    size_t gridWidth = bufferWidth / mapWidth;
    size_t gridHeight = bufferHeight / mapHeight;

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

    draw_rectangle(frameBuffer, bufferWidth, bufferHeight, playerPosX * gridWidth, playerPosY * gridHeight, 5, 5, pack_color(255, 255, 255));

    drop_ppm_image("./out.ppm", frameBuffer, bufferWidth, bufferHeight);

    return 0;
}
