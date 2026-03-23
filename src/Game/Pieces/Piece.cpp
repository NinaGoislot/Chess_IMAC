#include "Piece.hpp"

Piece::Piece() = default;
Piece::~Piece() = default;

void Piece::draw(const settings& gameSettings)
{
    ImVec2 squarePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(squarePos);
    ImGui::Image(_texture, ImVec2(gameSettings.buttonSize, gameSettings.buttonSize));
}

const std::vector<Vector2D>& Piece::getAllowedMoves() const
{
    return _allowedMoves;
}
