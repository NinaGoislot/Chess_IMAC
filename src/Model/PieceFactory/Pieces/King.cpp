#include "King.hpp"
#include <imgui.h>
#include <vector>
#include "Model/Board/Board.hpp"
#include "utilities/Direction.hpp"
#include "utilities/Vector2D.hpp"

King::King()
    : Piece()
{
    _name  = "King";
    _type  = PieceType::King;
    _color = PieceColor::White;
}

void King::updateAllowedMoves(const Board& board, Vector2D position)
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

        if (board.isInside(currentPos) && (!board.isEmpty(currentPos) ? board.isEnemy(currentPos, _color) : true))
        {
            _allowedMoves.push_back(currentPos);
        }
    }
}

King::King(ImTextureID texture)
{
    _name    = "King";
    _type    = PieceType::King;
    _color   = PieceColor::White;
    _texture = texture;
}

King::King(PieceColor color, ImTextureID texture)
{
    _name    = "King";
    _type    = PieceType::King;
    _color   = color;
    _texture = texture;
}

void King::draw(const settings& gameSettings)
{
    ImVec2 squarePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(squarePos);

    ImGui::Image(_texture, ImVec2(gameSettings.buttonSize, gameSettings.buttonSize));
}
