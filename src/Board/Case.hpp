#pragma once

#include <memory>
// #include "Piece.hpp"

class Case {
private:
    int _x;
    int _y;
    // std::unique_ptr<Piece> _piece;

public:
    Case() = default;
    Case(int x, int y);

    int getX() const;
    int getY() const;

    bool hasPiece() const;
    // Piece* getPiece() const;
    // void setPiece(std::unique_ptr<Piece> p);
    void removePiece();
};