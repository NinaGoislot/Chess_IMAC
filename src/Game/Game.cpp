#include "Game.hpp"
#include <imgui.h>
#include <iostream>
#include "settings.hpp"
#include <memory>
#include "Pieces/Pawn.hpp"

// #include "quick_imgui/quick_imgui.hpp"

void Game::displayBoard(const settings& gameSettings)
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

Game::Game() : _board()
{
}

void Game::placePieces(const settings& gameSettings)
{
    ImTextureID whitePawnTex = LoadTexture("../../textures/pieces/white_pawn.png");
    _board.getCase(0, 0).setPiece(std::make_unique<Pawn>());
    _board.getCase(0,0).getPiece()->draw(whitePawnTex, gameSettings);
}