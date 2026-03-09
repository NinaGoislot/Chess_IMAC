#include "Pawn.hpp"
#include <imgui.h>
#include <vector>


Pawn::Pawn()
    : Piece()
{
    _name = "Pawn";
    setAllowedMoves();
}

void Pawn::setAllowedMoves()
{
    Vector2D move1(0, 1);
    _allowedMoves.push_back(move1);
}

Pawn::Pawn(ImTextureID texture)
{
    _texture = texture;
    setAllowedMoves();
}

void Pawn::draw(const settings& gameSettings)
{
    ImVec2 squarePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(squarePos);

    ImGui::Image(_texture, ImVec2(gameSettings.buttonSize, gameSettings.buttonSize));
}
