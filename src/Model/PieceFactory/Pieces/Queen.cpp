#include "Queen.hpp"
#include <imgui.h>
#include <vector>
#include "Model/Board/Board.hpp"
#include "utilities/Direction.hpp"
#include "utilities/Vector2D.hpp"

Queen::Queen()
    : Piece()
{
    _name  = "Queen";
    _type  = PieceType::Queen;
    _color = PieceColor::White;
}

void Queen::updateAllowedMoves(const Board& board, Vector2D position)
{
    std::vector<Vector2D> directions{
        dir::up(),
        dir::down(),
        dir::left(),
        dir::right(),
        dir::up_right(),
        dir::down_right(),
        dir::up_left(),
        dir::down_left()
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

Queen::Queen(ImTextureID texture)
{
    _name    = "Queen";
    _type    = PieceType::Queen;
    _color   = PieceColor::White;
    _texture = texture;
}

Queen::Queen(PieceColor color, ImTextureID texture)
{
    _name    = "Queen";
    _type    = PieceType::Queen;
    _color   = color;
    _texture = texture;
}

void Queen::draw(const settings& gameSettings)
{
    ImVec2 squarePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(squarePos);

    ImGui::Image(_texture, ImVec2(gameSettings.buttonSize, gameSettings.buttonSize));
}
