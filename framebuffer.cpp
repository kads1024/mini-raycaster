#include "framebuffer.h"

#include <cassert>

frameBuffer::frameBuffer(size_t width, size_t height, uint32_t color)
    : width(width), height(height)
{
    clear(color);
}

void frameBuffer::clear(uint32_t color)
{
    data.assign(width * height, color);
}

void frameBuffer::set_pixel(size_t x, size_t y, uint32_t color)
{
    assert(x < width && y < height && data.size() == width * height);
    data[x + y * width] = color;
}

uint32_t frameBuffer::get_pixel(size_t x, size_t y) const
{
    assert(x < width && y < height && data.size() == width * height);
    return data[x + y * width];
}

void frameBuffer::draw_rectangle(size_t startX, size_t startY, size_t rectWidth, size_t rectHeight, uint32_t color)
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

depthBuffer::depthBuffer(size_t width, size_t height)
    : width(width), height(height)
{
    clear(0.0f);
}

void depthBuffer::clear(float depth)
{
    data.assign(width * height, depth);
}

void depthBuffer::set_pixel(size_t x, size_t y, float depth)
{
    assert(x < width && y < height && data.size() == width * height);
    data[x + y * width] = depth;
}

float depthBuffer::get_pixel(size_t x, size_t y) const
{
    assert(x < width && y < height && data.size() == width * height);
    return data[x + y * width];
}
