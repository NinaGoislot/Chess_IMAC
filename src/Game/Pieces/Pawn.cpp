#include "Pawn.hpp"
#include <vector>
#include "utilities/Direction.hpp"
#include <imgui.h>

Pawn::Pawn()
    : Piece()
{
    setAllowedMoves();
}

Pawn::~Pawn() = default;

void Pawn::setAllowedMoves()
{
    Vector2D move1(0, 1);

    _allowedMoves.push_back(move1);
}

void Pawn::draw(const ImTextureID &texture, const settings& gameSettings)
{
    

    ImVec2 squarePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(squarePos);
    ImGui::Image(texture, ImVec2(gameSettings.buttonSize, gameSettings.buttonSize));
}

