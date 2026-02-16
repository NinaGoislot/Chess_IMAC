#pragma once

#include <string>
#include <vector>
#include "Move.hpp"
#include "utilities/Vector2D.hpp"

class Piece {
public:
    Piece();
    Piece(Piece&&)                 = default;
    Piece(const Piece&)            = delete;
    Piece& operator=(const Piece&&)      = delete;
    Piece& operator=(const Piece&) = delete;
    virtual ~Piece()               = 0;

protected:
    std::string           _name;
    std::vector<Vector2D> _allowedMoves;

    virtual void setAllowedMoves() = 0;
};
