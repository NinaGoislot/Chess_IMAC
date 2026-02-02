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

private:
    std::string       name;
    std::vector<Move> allowedMoves;
};
