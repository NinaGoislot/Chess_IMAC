#include "BoardRenderer.hpp"
#include <imgui.h>
#include <iostream>

BoardRenderer::BoardRenderer(TextureManager& textures)
    : _textures(textures)
{
}

void BoardRenderer::draw(Board& board, const settings& gameSettings) const
{
    for (int y = 0; y < Board::SIZE; y++)
    {
        for (int x = 0; x < Board::SIZE; x++)
        {
            ImGui::PushID(x + y * Board::SIZE);

            bool white = (x + y) % 2 == 0;

            ImVec4 color = white ? gameSettings.getWhite() : gameSettings.getBlack();

            ImGui::PushStyleColor(ImGuiCol_Button, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

            const Case& currentCase = board.getCase(x, y);

            ImVec2 pos = ImGui::GetCursorScreenPos();

            if (ImGui::Button(" ", ImVec2{gameSettings.buttonSize, gameSettings.buttonSize}))
            {
                board.onCaseClicked(x, y);
            }

            if (currentCase.hasPiece())
            {
                Piece* piece = currentCase.getPiece();

                ImGui::SetCursorScreenPos(pos);
                piece->draw(gameSettings);
            }
            if (currentCase.isActive())
            {
                color = gameSettings.getHighlight();
            }

            ImGui::PopStyleColor(3);
            ImGui::PopID();

            if (x < Board::SIZE - 1)
                ImGui::SameLine();
        }
    }
}