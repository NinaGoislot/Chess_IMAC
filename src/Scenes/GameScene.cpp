#include "GameScene.hpp"
#include <imgui.h>
#include "Managers/SceneManager.hpp"
#include "Scenes/SettingsPanel.hpp"

GameScene::GameScene(SceneManager& sceneManager, GameManager::Mode mode)
    : _sceneManager(&sceneManager)
    , _game(sceneManager.getGame())
    , _mode(mode)
{
}

void GameScene::render()
{
    ImGui::Begin("Partie");
    if (ImGui::Button("Retour au menu"))
    {
        _sceneManager->saveInterruptedMatch();
        _sceneManager->launchMenuScene();
        ImGui::End();
        return;
    }
    ImGui::SameLine();
    if (ImGui::Button("Nouvelle partie"))
    {
        _game.newGame(_mode);
    }

    ImGui::Separator();
    _game.displayBoard(ImGui::GetIO().DeltaTime);
    _game.getPromotionFlow().drawPopup();

    ImGui::End();

    SettingsPanel::draw(_game.getSettings());

    ImGui::Begin("Historique");

    ImGui::BeginChild("Scrolling");

    const std::vector<std::string>& history = _game.getMoveHistory();
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

