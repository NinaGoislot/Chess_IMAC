#include "Rook.hpp"
#include <imgui.h>
#include <vector>
#include "Model/Board/Board.hpp"
#include "utilities/Direction.hpp"
#include "utilities/Vector2D.hpp"

Rook::Rook()
    : Piece()
{
    _name  = "Rook";
    _type  = PieceType::Rook;
    _color = PieceColor::White;
}

void Rook::updateAllowedMoves(const Board& board, Vector2D position)
{
    std::vector<Vector2D> directions{
        dir::up(),
        dir::down(),
        dir::left(),
        dir::right()
    };

    _allowedMoves.clear();

    for (const Vector2D& dir : directions)
    {
        Vector2D currentPos = position + dir;

        while (board.isInside(currentPos))
        {
            if (!board.isEmpty(currentPos))
            {
                if (board.isEnemy(currentPos, _color))
                {
                    _allowedMoves.push_back(currentPos);
                }
                break;
            }

            _allowedMoves.push_back(currentPos);

            currentPos = currentPos + dir;
        }
    }
}

Rook::Rook(ImTextureID texture)
{
    _name    = "Rook";
    _type    = PieceType::Rook;
    _color   = PieceColor::White;
    _texture = texture;
}

Rook::Rook(PieceColor color, ImTextureID texture)
{
    _name    = "Rook";
    _type    = PieceType::Rook;
    _color   = color;
    _texture = texture;
}

void Rook::draw(const settings& gameSettings)
{
    ImVec2 squarePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(squarePos);

    ImGui::Image(_texture, ImVec2(gameSettings.buttonSize, gameSettings.buttonSize));
}
