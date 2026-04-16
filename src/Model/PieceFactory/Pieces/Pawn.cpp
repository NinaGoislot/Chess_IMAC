#include "Pawn.hpp"
#include <imgui.h>
#include <vector>
#include "Model/Board/Board.hpp"

Pawn::Pawn()
    : Piece()
{
    _name  = "Pawn";
    _type  = PieceType::Pawn;
    _color = PieceColor::White;
}

void Pawn::updateAllowedMoves(const Board& board, Vector2D position)
{
    _allowedMoves.clear();

    const float direction = (_color == PieceColor::White) ? -1.f : 1.f;
    const float startRow  = (_color == PieceColor::White) ? 6.f : 1.f;

    const Vector2D oneStep(position.getX(), position.getY() + direction);
    if (board.isInside(oneStep) && board.isEmpty(oneStep))
    {
        _allowedMoves.push_back(oneStep);

        const Vector2D twoStep(position.getX(), position.getY() + 2.f * direction);
        if (position.getY() == startRow && board.isInside(twoStep) && board.isEmpty(twoStep))
        {
            _allowedMoves.push_back(twoStep);
        }
    }

    const Vector2D captureLeft(position.getX() - 1.f, position.getY() + direction);
    const Vector2D captureRight(position.getX() + 1.f, position.getY() + direction);

    if (board.isEnemy(captureLeft, _color))
    {
        _allowedMoves.push_back(captureLeft);
    }
    if (board.isEnemy(captureRight, _color))
    {
        _allowedMoves.push_back(captureRight);
    }
}

Pawn::Pawn(ImTextureID texture)
{
    _name    = "Pawn";
    _type    = PieceType::Pawn;
    _color   = PieceColor::White;
    _texture = texture;
}

Pawn::Pawn(PieceColor color, ImTextureID texture)
{
    _name    = "Pawn";
    _type    = PieceType::Pawn;
    _color   = color;
    _texture = texture;
}

void Pawn::draw(const settings& gameSettings)
{
    ImVec2 squarePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(squarePos);

    ImGui::Image(_texture, ImVec2(gameSettings.buttonSize, gameSettings.buttonSize));
}
