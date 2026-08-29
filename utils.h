#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

/// @brief Packs RGBA into one 32-bit value, laid out little-endian as bytes r, g, b, a.
uint32_t pack_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
void unpack_color(uint32_t inColor, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a);

void drop_ppm_image(const char *fileName, const std::vector<uint32_t> &buffer, size_t w, size_t h);
