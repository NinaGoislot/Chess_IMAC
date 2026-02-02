#pragma once
#include "Vector2D.hpp"  

namespace dir {
    inline Vector2D up() { return Vector2D{0, 1}; }
    inline Vector2D down() { return Vector2D{0, -1}; }
    inline Vector2D left() { return Vector2D{-1, 0}; }
    inline Vector2D right() { return Vector2D{1, 0}; }
    inline Vector2D up_left() { return Vector2D{-1, 1}; }
    inline Vector2D up_right() { return Vector2D{1, 1}; }
    inline Vector2D down_left() { return Vector2D{-1, -1}; }
    inline Vector2D down_right() { return Vector2D{1, -1}; }
} 
