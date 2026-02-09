#pragma once

#include <string>
#include <vector>
#include "Move.hpp"

class Piece {
public:
    Piece();
    Piece(Piece&&)                 = default;
    Piece(const Piece&)            = default;
    Piece& operator=(Piece&&)      = default;
    Piece& operator=(const Piece&) = default;
    virtual ~Piece()               = 0;

protected:
    std::string       _name;
    std::vector<Move> _allowedMoves;

    virtual void setAllowedMoves() = 0;
};
