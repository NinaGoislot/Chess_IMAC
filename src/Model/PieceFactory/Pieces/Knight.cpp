#include "Knight.hpp"
#include <imgui.h>
#include <vector>
#include "Model/Board/Board.hpp"
#include "utilities/Vector2D.hpp"


Knight::Knight()
    : Piece()
{
    _name = "Knight";
    _type = PieceType::Knight;
    _color = PieceColor::White;
}

void Knight::updateAllowedMoves(const Board& board, Vector2D position)
{
   std::vector<Vector2D> directions {
    {1, 2},   // up-up-right
    {1, -2},  // down-down-right
    {-1, 2},  // up-up-left
    {-1, -2}, // down-down-left
    {2, 1},   // right-right-up
    {2, -1},  // right-right-down
    {-2, 1},  // left-left-up
    {-2, -1}  // left-left-down
   };

   _allowedMoves.clear();

    for (const Vector2D& dir : directions) {
        Vector2D currentPos = position + dir;

        if (board.isInside(currentPos) && (!board.isEmpty(currentPos) ? board.isEnemy(currentPos, _color) : true)) {
            _allowedMoves.push_back(currentPos);
        }
    }
}

Knight::Knight(ImTextureID texture)
{
    _name = "Knight";
    _type = PieceType::Knight;
    _color = PieceColor::White;
    _texture = texture;
}

Knight::Knight(PieceColor color, ImTextureID texture)
{
    _name = "Knight";
    _type = PieceType::Knight;
    _color = color;
    _texture = texture;
}

void Knight::draw(const settings& gameSettings)
{
    ImVec2 squarePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(squarePos);

    ImGui::Image(_texture, ImVec2(gameSettings.buttonSize, gameSettings.buttonSize));
}
