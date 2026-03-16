#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include "Game/settings.hpp"
#include "utilities/Vector2D.hpp"

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

class Piece {
public:
    Piece() = default;
    virtual ~Piece()               = 0;
    
    virtual void draw(const settings& gameSettings);

    PieceColor               color() const { return _color; }
    PieceType                type() const { return _type; }
    const std::string&       name() const { return _name; }

protected:
    std::string           _name;
    std::vector<Vector2D> _allowedMoves;
    ImTextureID           _texture = nullptr;
    PieceColor            _color;
    PieceType             _type;

    virtual void setAllowedMoves() = 0;
};
