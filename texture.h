#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct texture
{
    size_t width;
    size_t height;
    size_t count;
    size_t pixelSize;
    std::vector<uint32_t> data;

    texture(const std::string fileName); 
    uint32_t& get(const size_t x, const size_t y, const size_t textureIndex);
    std::vector<uint32_t> get_scaled_column(const size_t textureId, const size_t xCoord, const size_t columnHeight);
};