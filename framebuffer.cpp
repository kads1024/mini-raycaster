#include "framebuffer.h"
#include <cassert>

void frameBuffer::clear(const uint32_t color)
{
    data = std::vector<uint32_t>(width * height, color);
}

void frameBuffer::set_pixel(const size_t x, const size_t y, const uint32_t color)
{
    assert(x >= 0 && y >= 0 && x < width && y < height && data.size() == width * height);
    data[x + y * width] = color;
}

uint32_t frameBuffer::get_pixel(const size_t x, const size_t y)
{
    assert(x >= 0 && y >= 0 && x < width && y < height && data.size() == width * height);
    return data[x + y * width];
}

void frameBuffer::draw_rectangle(const size_t startX, const size_t startY, const size_t rectWidth, const size_t rectHeight, const uint32_t color)
{
    assert(data.size() == width * height);
    for (size_t rectPixelY = 0; rectPixelY < rectHeight; rectPixelY++)
    {
        for (size_t rectPixelX = 0; rectPixelX < rectWidth; rectPixelX++)
        {
            size_t currentPixelX = startX + rectPixelX;
            size_t currentPixelY = startY + rectPixelY;

            if (currentPixelX < width && currentPixelY < height)
                data[currentPixelY * width + currentPixelX] = color;
        }
    }
}

void depthBuffer::clear(const float depth)
{
    data = std::vector<float>(width * height, depth);
}

void depthBuffer::set_pixel(const size_t x, const size_t y, const float depth)
{
    assert(x >= 0 && y >= 0 && x < width && y < height && data.size() == width * height);
    data[x + y * width] = depth;
}

float depthBuffer::get_pixel(const size_t x, const size_t y)
{
    assert(x >= 0 && y >= 0 && x < width && y < height && data.size() == width * height);
    return data[x + y * width];
}
