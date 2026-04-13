#pragma once

#include "Game/Pieces/Piece.hpp"

// Represents one board tile with position, activity state, and optional piece.
class Case {
private:
    // Tile x coordinate.
    int _x;
    // Tile y coordinate.
    int _y;
    // Non-owning pointer to the piece currently on the tile.
    Piece* _piece = nullptr;
    // Selection/highlight state for UI and move feedback.
    bool _isActive;

public:
    // Constructors
    Case() = default;
    Case(int x, int y);

    // Getters
    int getX() const;
    int getY() const;

    bool getIsActive() const;
    // Setters
    void setActive(bool active);

    bool getHasPiece() const;
    Piece* getPiece() const;
    void setPiece(Piece* p);
    // Removes the piece pointer without returning it.
    void removePiece();

    // Transfers the piece pointer out of the tile and clears it.
    Piece* takePiece();

    // Callback-like action when this tile is clicked.
    void onClick();
};

