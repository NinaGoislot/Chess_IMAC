#pragma once
#include "Vector2D.hpp"  

// Provides reusable unit directions for move generation.
namespace dir {
    // Get upward grid direction.
    inline Vector2D up() { return Vector2D{0, 1}; }
    // Get downward grid direction.
    inline Vector2D down() { return Vector2D{0, -1}; }
    // Get left grid direction.
    inline Vector2D left() { return Vector2D{-1, 0}; }
    // Get right grid direction.
    inline Vector2D right() { return Vector2D{1, 0}; }
    // Get up-left diagonal direction.
    inline Vector2D up_left() { return Vector2D{-1, 1}; }
    // Get up-right diagonal direction.
    inline Vector2D up_right() { return Vector2D{1, 1}; }
    // Get down-left diagonal direction.
    inline Vector2D down_left() { return Vector2D{-1, -1}; }
    // Get down-right diagonal direction.
    inline Vector2D down_right() { return Vector2D{1, -1}; }
} 
