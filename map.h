#pragma once
#include <cstdint>

struct map
{
    size_t width;
    size_t height;

    map();
    int get(const size_t x, const size_t y);
    bool is_cell_empty(const size_t x, const size_t );
};
