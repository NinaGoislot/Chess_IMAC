#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include "Game/settings.hpp"
#include "utilities/Vector2D.hpp"

class Piece {
public:
    Piece();
    Piece(Piece&&)                 = default;
    Piece(const Piece&)            = delete;
    Piece& operator=(Piece&&)      = delete;
    Piece& operator=(const Piece&) = delete;
    virtual ~Piece()               = 0;
    
    virtual void draw(const settings& gameSettings);
    const std::vector<Vector2D>& getAllowedMoves() const;

protected:
    std::string           _name;
    std::vector<Vector2D> _allowedMoves;
    ImTextureID           _texture = nullptr;

    virtual void setAllowedMoves() = 0;
};
