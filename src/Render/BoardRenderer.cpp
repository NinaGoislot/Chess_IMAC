#include "BoardRenderer.hpp"
#include <imgui.h>

BoardRenderer::BoardRenderer(TextureManager& textures)
    : _textures(textures)
{
}

void BoardRenderer::draw(Board& board, const settings& gameSettings)
{
    if (gameSettings.use3D)
    {
        ImVec2 available = ImGui::GetContentRegionAvail();
        available.x      = std::max(available.x, 64.f);
        available.y      = std::max(available.y, 64.f);

        _board3DRenderer.render(board, gameSettings, static_cast<int>(available.x), static_cast<int>(available.y));

        ImTextureID texture = _board3DRenderer.colorTexture();
        if (texture != nullptr)
        {
            ImGui::Image(texture, available, ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
            return;
        }
    }

    if (!gameSettings.use3D)
    {
        for (int y = 0; y < Board::SIZE; y++)
        {
            for (int x = 0; x < Board::SIZE; x++)
            {
                ImGui::PushID(x + y * Board::SIZE);

                bool        white       = (x + y) % 2 == 0;
                const Case& currentCase = board.getCase(x, y);

                ImVec4 color = white ? gameSettings.getWhite() : gameSettings.getBlack();
                if (currentCase.isActive())
                {
                    color = gameSettings.getHighlight();
                }

                ImGui::PushStyleColor(ImGuiCol_Button, color);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

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

                ImGui::PopStyleColor(3);
                ImGui::PopID();

                if (x < Board::SIZE - 1)
                    ImGui::SameLine();
            }
        }
    }
}