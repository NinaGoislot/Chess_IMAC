#include "Queen.hpp"
#include <imgui.h>
#include <vector>
#include "Board/Board.hpp"
#include "utilities/Vector2D.hpp"


Queen::Queen()
    : Piece()
{
    _name = "Queen";
    _type = PieceType::Queen;
    _color = PieceColor::White;
}

void Queen::updateAllowedMoves(const Board& board, Vector2D position)
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

        while (board.isInside(currentPos)) {
            if (!board.isEmpty(currentPos)) {
                if (board.isEnemy(currentPos, _color)) {
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
    _name = "Queen";
    _type = PieceType::Queen;
    _color = PieceColor::White;
    _texture = texture;
}

Queen::Queen(PieceColor color, ImTextureID texture)
{
    _name = "Queen";
    _type = PieceType::Queen;
    _color = color;
    _texture = texture;
}

void Queen::draw(const settings& gameSettings)
{
    ImVec2 squarePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(squarePos);

    ImGui::Image(_texture, ImVec2(gameSettings.buttonSize, gameSettings.buttonSize));
}
