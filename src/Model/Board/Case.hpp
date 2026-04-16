#pragma once

#include "Model/PieceFactory/Piece.hpp"

// One board tile with position, activity state, and optional piece
class Case {
private:
    int    _x;
    int    _y;
    Piece* _piece = nullptr;
    bool   _isActive;

public:
    // Constructors
    Case() = default;
    Case(int x, int y);

    // Getters
    int    getX() const;
    int    getY() const;
    bool   getIsActive() const;
    bool   getHasPiece() const;
    Piece* getPiece() const;

    // Setters
    void setActive(bool active);
    void setPiece(Piece* p);

    // Actions
    void   removePiece();
    Piece* takePiece();

    void onClick();
};
