#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cassert>
#include <iostream>

#include "utils.h"

texture::texture(const std::string &fileName) : width(0), height(0), count(0), pixelSize(0), data()
{
    int numChannels = -1;
    int textureWidth;
    int textureHeight;

    unsigned char *pixelMap = stbi_load(fileName.c_str(), &textureWidth, &textureHeight, &numChannels, 0);
    if (!pixelMap)
    {
        std::cerr << "Error: can not load the textures" << std::endl;
        return;
    }

    if (4 != numChannels)
    {
        std::cerr << "Error: the texture must be a 32 bit image" << std::endl;
        stbi_image_free(pixelMap);
        return;
    }

    count = textureWidth / textureHeight; // must be square
    pixelSize = textureWidth / count;

    if (textureWidth != textureHeight * int(count)) // width must be equal to count*height
    {
        std::cerr << "Error: the texture file must contain N square textures packed horizontally" << std::endl;
        stbi_image_free(pixelMap);
        return;
    }

    width = textureWidth;
    height = textureHeight;
    data = std::vector<uint32_t>(width * height);

    for (size_t texturePixelY = 0; texturePixelY < size_t(textureHeight); texturePixelY++)
    {
        for (size_t texturePixelX = 0; texturePixelX < size_t(textureWidth); texturePixelX++)
        {
            // 4 bytes per color
            uint8_t r = pixelMap[(texturePixelX + texturePixelY * textureWidth) * 4 + 0];
            uint8_t g = pixelMap[(texturePixelX + texturePixelY * textureWidth) * 4 + 1];
            uint8_t b = pixelMap[(texturePixelX + texturePixelY * textureWidth) * 4 + 2];
            uint8_t a = pixelMap[(texturePixelX + texturePixelY * textureWidth) * 4 + 3];
            data[texturePixelX + texturePixelY * textureWidth] = pack_color(r, g, b, a);
        }
    }
    stbi_image_free(pixelMap);
}

uint32_t texture::get(size_t x, size_t y, size_t textureIndex) const
{
    assert(x < pixelSize && y < pixelSize && textureIndex < count);
    return data[(textureIndex * pixelSize + x) + y * width];
}

std::vector<uint32_t> texture::get_scaled_column(size_t textureId, size_t xCoord, size_t columnHeight) const
{
    assert(xCoord < pixelSize && textureId < count);
    std::vector<uint32_t> column(columnHeight);
    for (size_t y = 0; y < columnHeight; y++)
    {
        size_t pixelY = pixelSize * (static_cast<float>(y) / columnHeight);
        column[y] = get(xCoord, pixelY, textureId);
    }
    return column;
}
