#pragma once

#include "Game/Pieces/Piece.hpp"

class Case {
private:
    int _x;
    int _y;
    Piece* _piece = nullptr;
    bool _isActive;

public:
    Case() = default;
    Case(int x, int y);

    int getX() const;
    int getY() const;

    bool isActive() const;
    void setActive(bool active);

    bool hasPiece() const;
    Piece* getPiece() const;
    void setPiece(Piece* p);
    void removePiece();

    Piece* takePiece();

    void onClick();
};