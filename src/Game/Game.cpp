#include "Game.hpp"
#include <imgui.h>
#include <iostream>
#include "_settings.hpp"

// #include "quick_imgui/quick_imgui.hpp"

void ChessGame::displayBoard(const settings &gameSettings)
{
    for (int y{0}; y < Board::SIZE; y++)
    {
        for (int x{0}; x < Board::SIZE; x++)
        {
            ImGui::PushID(x + y * Board::SIZE);
            if (y % 2 == 0)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, (x % 2 == 0) ? gameSettings.getWhite() : gameSettings.getBlack());
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, (x % 2 == 0) ? gameSettings.getBlack() : gameSettings.getWhite());
            }
            if (ImGui::Button(" ", ImVec2{gameSettings.buttonSize, gameSettings.buttonSize}))
            {
                std::cout << "Clicked on case (" << x << ", " << y << ")\n";
            }
            ImGui::PopStyleColor();
            ImGui::PopID();
            if (x < Board::SIZE - 1)
            {
                ImGui::SameLine(); // Keep buttons on the same line
            }
        }
    }
}

ChessGame::ChessGame() : _board()
{
}
