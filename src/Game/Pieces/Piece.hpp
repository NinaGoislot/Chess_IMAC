#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include "game/settings.hpp"
#include "utilities/Vector2D.hpp"

class Piece {
public:
    Piece();
    virtual ~Piece() = default;

    virtual void draw(const settings& gameSettings);

protected:
    std::string           _name;
    std::vector<Vector2D> _allowedMoves;
    ImTextureID           _texture = nullptr;

    virtual void setAllowedMoves() = 0;
};
