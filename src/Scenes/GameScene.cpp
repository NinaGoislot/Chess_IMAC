#include "GameScene.hpp"
#include <imgui.h>
#include "Managers/Game.hpp"
#include "Managers/SceneManager.hpp"
#include "Render/Renderer.hpp"
#include "Scenes/SettingsPanel.hpp"

GameScene::GameScene(SceneManager& sceneManager)
    : _sceneManager(&sceneManager), _game(Game::instance())
{
    _game.newGame();
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
        _game.newGame();
    }

    ImGui::Separator();
    _game.displayBoard();
    ImGui::End();

    SettingsPanel::draw(_game.getSettings());

    ImGui::Begin("Historique");

    ImGui::BeginChild("Scrolling");

    const std::vector<std::string> history = _game.getMoveHistory();
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
