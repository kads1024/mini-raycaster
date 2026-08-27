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

int main()
{
    size_t w = 512;
    size_t h = 512;
    std::vector<uint32_t> frameBuffer(w * h, 255);

    for(size_t pixelY = 0; pixelY < h; pixelY++)
    {
        for (size_t pixelX = 0; pixelX < w; pixelX++)
        {
            uint8_t r = ((float(pixelY) / h) * 255);
            uint8_t g = ((float(pixelX) / w) * 255);
            uint8_t b = 0;
            frameBuffer[pixelX + pixelY * w] = pack_color(r, g, b);
        }
    }

    drop_ppm_image("./out.ppm", frameBuffer, w, h);

    return 0;
}
