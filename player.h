#pragma once

/// @brief Camera and movement state for the viewer.
struct player
{
    float x;                  ///< grid position X
    float y;                  ///< grid position Y
    float viewDirectionAngle; ///< facing, in radians, relative to +x
    float fov;                ///< horizontal field of view, in radians

    int turn; ///< -1 turning left, +1 turning right, 0 still
    int walk; ///< -1 walking back, +1 walking forward, 0 still
};
