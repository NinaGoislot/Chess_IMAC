#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include "Move.hpp"
#include "utilities/Vector2D.hpp"
#include "game/settings.hpp"
#include "Render/TextureLoader.hpp"

class Piece {
public:
    Piece();
    Piece(Piece&&)                 = default;
    Piece(const Piece&)            = delete;
    Piece& operator=(const Piece&&)      = delete;
    Piece& operator=(const Piece&) = delete;
    virtual ~Piece()               = 0;
    
    virtual void draw(const ImTextureID &texture, const settings& gameSettings) = 0;

protected:
    std::string           _name;
    std::vector<Vector2D> _allowedMoves;

    virtual void setAllowedMoves() = 0;

};
