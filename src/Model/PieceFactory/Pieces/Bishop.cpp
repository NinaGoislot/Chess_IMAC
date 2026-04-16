#include "Bishop.hpp"
#include <imgui.h>
#include <vector>
#include "Model/Board/Board.hpp"
#include "utilities/Vector2D.hpp"


Bishop::Bishop()
    : Piece()
{
    _name = "Bishop";
    _type = PieceType::Bishop;
    _color = PieceColor::White;
}

void Bishop::updateAllowedMoves(const Board& board, Vector2D position)
{
   std::vector<Vector2D> directions {
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

Bishop::Bishop(ImTextureID texture)
{
    _name = "Bishop";
    _type = PieceType::Bishop;
    _color = PieceColor::White;
    _texture = texture;
}

Bishop::Bishop(PieceColor color, ImTextureID texture)
{
    _name = "Bishop";
    _type = PieceType::Bishop;
    _color = color;
    _texture = texture;
}

void Bishop::draw(const settings& gameSettings)
{
    ImVec2 squarePos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorScreenPos(squarePos);

    ImGui::Image(_texture, ImVec2(gameSettings.buttonSize, gameSettings.buttonSize));
}
