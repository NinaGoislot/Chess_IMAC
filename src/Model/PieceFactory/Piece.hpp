#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include "Model/settings.hpp"
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

// Base chess piece class
class Piece {
public:
    // Constructors
    Piece()          = default;
    virtual ~Piece() = 0;

    // Render
    virtual void draw(const settings& gameSettings);

    // Getters
    std::vector<Vector2D> getAllowedMoves() const { return _allowedMoves; }
    // void                  updateAllowedMoves(const Board& board, Vector2D position) { getAllowedMoves(board, position); }
    PieceColor         getColor() const { return _color; }
    PieceType          getType() const { return _type; }
    const std::string& getName() const { return _name; }

    // update
    virtual void updateAllowedMoves(const Board& board, Vector2D position) = 0;

protected:
    // Parameters
    std::string           _name;
    std::vector<Vector2D> _allowedMoves;
    ImTextureID           _texture = nullptr;
    PieceColor            _color;
    PieceType             _type;
};
