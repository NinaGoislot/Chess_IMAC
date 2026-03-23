#include "Pawn.hpp"
#include <imgui.h>
#include <vector>


Pawn::Pawn()
    : Piece()
{
    _name = "Pawn";
    _type = PieceType::Pawn;
    _color = PieceColor::White;
}

void Pawn::setAllowedMoves(const Board& board, Vector2D position)
{
    _allowedMoves.clear();
    if (_color == PieceColor::White) {
        _allowedMoves.push_back({0, 1});   // forward
        if (position.getY() == 1) {
            _allowedMoves.push_back({0, 2});   // double forward from starting position
        }
        _allowedMoves.push_back({-1, 1});  // capture left
        _allowedMoves.push_back({1, 1});   // capture right
    } else {
        _allowedMoves.push_back({0, -1});  // forward
        if (position.getY() == 6) {
            _allowedMoves.push_back({0, -2});  // double forward from starting position
        }
        _allowedMoves.push_back({-1, -1}); // capture left
        _allowedMoves.push_back({1, -1});  // capture right
    }
}

Pawn::Pawn(ImTextureID texture)
{
    _name = "Pawn";
    _type = PieceType::Pawn;
    _color = PieceColor::White;
    _texture = texture;
}

Pawn::Pawn(PieceColor color, ImTextureID texture)
{
    _name = "Pawn";
    _type = PieceType::Pawn;
    _color = color;
    _texture = texture;
}

void Pawn::draw(const settings& gameSettings)
{
    ImVec2 squarePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(squarePos);

    ImGui::Image(_texture, ImVec2(gameSettings.buttonSize, gameSettings.buttonSize));
}
