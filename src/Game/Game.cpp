#include "Game.hpp"
#include <iostream>
#include <imgui.h>
// #include "quick_imgui/quick_imgui.hpp"

void ChessGame::displayBoard()
{
    ImGui::Begin("Chess Board");
    for (int y{0}; y < Board::SIZE; y++)
    {
        for (int x{0}; x < Board::SIZE; x++)
        {
            ImGui::PushID(x + y * Board::SIZE);
            if (y % 2 == 0)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, (x % 2 == 0) ? ImVec4{0.9f, 0.9f, 0.8f, 1.f} : ImVec4{0.46f, 0.58f, 0.33f, 1.f});
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, (x % 2 == 0) ? ImVec4{0.46f, 0.58f, 0.33f, 1.f} : ImVec4{0.9f, 0.9f, 0.8f, 1.f});
            }
            if (ImGui::Button(" ", ImVec2{50.f, 50.f}))
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
    ImGui::End();
}

void ChessGame::placePieces()
{
    
}

ChessGame::ChessGame() : _board()
{

    
}
