#include "BoardRenderer.hpp"
#include <imgui.h>
#include <iostream>

BoardRenderer::BoardRenderer(TextureManager& textures)
    : _textures(textures)
{
}

void BoardRenderer::draw(const Board& board, const settings& gameSettings) const
{
    for (int y = 0; y < Board::SIZE; y++)
    {
        for (int x = 0; x < Board::SIZE; x++)
        {
            ImGui::PushID(x + y * Board::SIZE);

            bool white = (x + y) % 2 == 0;
            

            ImGui::PushStyleColor(
                ImGuiCol_Button,
                white ? gameSettings.getWhite() : gameSettings.getBlack()
            );

            const Case& currentCase   = board.getCase(x, y);

            ImVec2      pos = ImGui::GetCursorScreenPos();

            if (ImGui::Button(" ", ImVec2{gameSettings.buttonSize, gameSettings.buttonSize}))
            {
                std::cout << "Clicked " << x << "," << y << "\n";
            }

            if (currentCase.hasPiece())
            {
    
                Piece* piece = currentCase.getPiece();

                ImGui::SetCursorScreenPos(pos);
                piece->draw(gameSettings);
            }

            ImGui::PopStyleColor();
            ImGui::PopID();

            if (x < Board::SIZE - 1)
                ImGui::SameLine();
        }
    }
}