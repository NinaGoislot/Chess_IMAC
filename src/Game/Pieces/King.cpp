#include "King.hpp"
#include <imgui.h>
#include <vector>
#include "Board/Board.hpp"
#include "utilities/Vector2D.hpp"


King::King()
    : Piece()
{
    _name = "King";
    _type = PieceType::King;
    _color = PieceColor::White;
}

void King::setAllowedMoves(const Board& board, Vector2D position)
{
   std::vector<Vector2D> directions {
    {0, 1},   // up
    {0, -1},  // down
    {-1, 0},  // left
    {1, 0},   // right
    {1, 1},   // up-right
    {1, -1},  // down-right
    {-1, 1},  // up-left
    {-1, -1}  // down-left
   };

   _allowedMoves.clear();

    for (const Vector2D& dir : directions) {
        Vector2D currentPos = position + dir;

        if (board.isInside(currentPos) && (!board.isEmpty(currentPos) ? board.isEnemy(currentPos, _color) : true)) {
            _allowedMoves.push_back(currentPos);
        }
    }
}

King::King(ImTextureID texture)
{
    _name = "King";
    _type = PieceType::King;
    _color = PieceColor::White;
    _texture = texture;
}

King::King(PieceColor color, ImTextureID texture)
{
    _name = "King";
    _type = PieceType::King;
    _color = color;
    _texture = texture;
}

void King::draw(const settings& gameSettings)
{
    ImVec2 squarePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(squarePos);

    ImGui::Image(_texture, ImVec2(gameSettings.buttonSize, gameSettings.buttonSize));
}
