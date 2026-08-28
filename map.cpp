#include "map.h"
#include <cassert>

static const char mapData[] =
        "0000222222220000"
        "1              0"
        "1      11111   0"
        "1     0        0"
        "0     0  1110000"
        "0     3        0"
        "0   10000      0"
        "0   3   11100  0"
        "5   4   0      0"
        "5   4   1  00000"
        "0       1      0"
        "2       1      0"
        "0       0      0"
        "0 0000000      0"
        "0              0"
        "0002222222200000";

map::map() : width(16), height(16) 
{
    assert(sizeof(mapData) == width * height + 1); // null termination
};

int map::get(const size_t x, const size_t y)
{
    assert(x >= 0 && y >= 0 && x < width && y < height);
    return mapData[x + y * width] - '0';
}

bool map::is_cell_empty(const size_t x, const size_t y)
{
    assert(x >= 0 && y >= 0 && x < width && y < height);
    return mapData[x + y * width] == ' ';
}