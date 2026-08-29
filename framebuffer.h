#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

/// @brief 32-bit RGBA colour target, stored row-major as width * height pixels.
struct frameBuffer
{
    size_t width;
    size_t height;
    std::vector<uint32_t> data;

    frameBuffer(size_t width, size_t height, uint32_t color);

    void clear(uint32_t color);
    void set_pixel(size_t x, size_t y, uint32_t color);
    uint32_t get_pixel(size_t x, size_t y) const;
    void draw_rectangle(size_t startX, size_t startY, size_t rectWidth, size_t rectHeight, uint32_t color);
};

/// @brief Per-pixel distance from the player, in map grid units. Kept as float so
/// sub-cell depths survive; a uint32_t buffer would truncate them to whole cells.
struct depthBuffer
{
    size_t width;
    size_t height;
    std::vector<float> data;

    depthBuffer(size_t width, size_t height);

    void clear(float depth);
    void set_pixel(size_t x, size_t y, float depth);
    float get_pixel(size_t x, size_t y) const;
};
