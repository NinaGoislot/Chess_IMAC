#include "GameScene.hpp"
#include <imgui.h>
#include "Managers/Game.hpp"
#include "Managers/SceneManager.hpp"

GameScene::GameScene(SceneManager& sceneManager)
    : _sceneManager(&sceneManager)
{
    Game::instance().newGame();
}

void GameScene::render()
{
    ImGui::Begin("Partie");
    if (ImGui::Button("Retour au menu"))
    {
        _sceneManager->launchMenuScene();
    }
    ImGui::SameLine();
    if (ImGui::Button("Nouvelle partie"))
    {
        Game::instance().newGame();
    }

    ImGui::Separator();
    Game::instance().displayBoard();
    ImGui::End();

    Game::instance().getSettings().display();

    ImGui::Begin("Historique");

    ImGui::BeginChild("Scrolling");

    const std::vector<std::string> history = Game::instance().getMoveHistory();
    for (const std::string& move : history)
    {
        ImGui::Text("%s", move.c_str());
    }

    // auto-scrolling
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();

    ImGui::End();
}
