#pragma once
#include <cstddef>

/// @brief Fixed-size grid of wall cells. A cell is either empty or holds a wall texture id.
struct map
{
    size_t width;
    size_t height;

    map();

    /// @brief Wall texture id at the given cell. Only meaningful when the cell is not empty.
    int get(size_t x, size_t y) const;
    bool is_cell_empty(size_t x, size_t y) const;
};
