#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include "Game/settings.hpp"
#include "utilities/Vector2D.hpp"

class Board;

enum class PieceColor {
    White,
    Black,
};

enum class PieceType {
    Pawn,
    Rook,
    Knight,
    Bishop,
    Queen,
    King,
};

// Base chess piece interface with shared rendering and move metadata.
class Piece {
public:
    // Constructors
    Piece()          = default;
    virtual ~Piece() = 0;

    // Render function: draws the piece in 2D UI mode.
    virtual void          draw(const settings& gameSettings);
    // Getters
    std::vector<Vector2D> getAllowedMoves() const { return _allowedMoves; }
    // void                  updateAllowedMoves(const Board& board, Vector2D position) { getAllowedMoves(board, position); }

    PieceColor         getColor() const { return _color; }
    PieceType          getType() const { return _type; }
    const std::string& getName() const { return _name; }
    // Recomputes allowed moves from board context and piece position.
    virtual void updateAllowedMoves(const Board& board, Vector2D position) = 0;

protected:
    // Human-readable piece name.
    std::string           _name;
    // Cached legal move destinations.
    std::vector<Vector2D> _allowedMoves;

    // Texture used by 2D renderer.
    ImTextureID _texture = nullptr;
    // Piece side.
    PieceColor  _color;
    // Piece role/type.
    PieceType   _type;

};
