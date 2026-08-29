#pragma once
#include <cstdint>
#include <vector>

struct frameBuffer
{
    size_t width;
    size_t height;                        
    std::vector<uint32_t> data; 

    void clear(const uint32_t color); 
    void set_pixel(const size_t x, const size_t y, const uint32_t color);
    uint32_t get_pixel(const size_t x, const size_t y);
    void draw_rectangle(const size_t startX, const size_t startY, const size_t rectWidth, const size_t rectHeight, const uint32_t color);
};

/// @brief Per-pixel distance from the player, in map grid units. Kept as float so
/// sub-cell depths survive; a uint32_t buffer would truncate them to whole cells.
struct depthBuffer
{
    size_t width;
    size_t height;
    std::vector<float> data;

    void clear(const float depth);
    void set_pixel(const size_t x, const size_t y, const float depth);
    float get_pixel(const size_t x, const size_t y);
};