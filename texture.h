#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

/// @brief An image holding N square textures packed horizontally side by side.
struct texture
{
    size_t width;     ///< full image width, in pixels
    size_t height;    ///< full image height, in pixels
    size_t count;     ///< how many square textures are packed in the image
    size_t pixelSize; ///< width and height of one square texture, in pixels

    std::vector<uint32_t> data;

    explicit texture(const std::string &fileName);

    /// @brief Texel at (x, y) within the square texture at textureIndex.
    uint32_t get(size_t x, size_t y, size_t textureIndex) const;

    /// @brief One texture column stretched vertically to columnHeight pixels.
    std::vector<uint32_t> get_scaled_column(size_t textureId, size_t xCoord, size_t columnHeight) const;
};
