#include "GameScene.hpp"
#include <imgui.h>
#include "Managers/SceneManager.hpp"
#include "Scenes/SettingsPanel.hpp"
#include "UI/GameUiComponents.hpp"

GameScene::GameScene(SceneManager& sceneManager, GameManager::Mode mode)
    : _sceneManager(&sceneManager)
    , _game(sceneManager.getGame())
    , _mode(mode)
{
}

/**
 *
 * rendu de la scene de jeu
 * @return Aucun
 */
void GameScene::render()
{
    constexpr ImVec2 actionButtonSize{170.f, 0.f};

    ImGui::Begin("Partie");
    if (GameUiComponents::drawPrimaryButton("Retour au menu", actionButtonSize))
    {
        _sceneManager->saveInterruptedMatch();
        _sceneManager->launchMenuScene();
        _winnerPopupShown = false;
        ImGui::End();
        return;
    }

    ImGui::SameLine();

    if (GameUiComponents::drawDangerButton("Nouvelle partie", actionButtonSize))
    {
        _game.newGame(_mode);
        _winnerPopupShown = false;
    }

    ImGui::Separator();

    if (_game.getHasWinner())
    {
        GameUiComponents::drawWinnerBanner(_game.getWinner());
        ImGui::Separator();

        if (!_winnerPopupShown)
        {
            ImGui::OpenPopup("Fin de partie");
            _winnerPopupShown = true;
        }
    }

    _game.beginBoardViewsFrame();
    _game.draw3DBoardView(ImGui::GetIO().DeltaTime);

    if (ImGui::BeginPopupModal("Fin de partie", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        const Player* winner     = _game.getWinner();
        const char*   winnerName = (winner != nullptr && !winner->getName().empty()) ? winner->getName().c_str() : "joueur inconnu";
        ImGui::Text("Victoire de %s", winnerName);
        ImGui::Spacing();

        if (GameUiComponents::drawPrimaryButton("Recommencer une partie de fou", ImVec2(180.f, 0.f)))
        {
            _game.newGame(_mode);
            _winnerPopupShown = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (GameUiComponents::drawSecondaryButton("Retour menu", ImVec2(160.f, 0.f)))
        {
            _sceneManager->saveInterruptedMatch();
            _sceneManager->launchMenuScene();
            _winnerPopupShown = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Begin("Vue 2D");
    _game.draw2DBoardView();
    ImGui::End();

    _game.endBoardViewsFrame();
    _game.getPromotionFlow().drawPopup();

    SettingsPanel::draw(_game.getSettings());

    ImGui::Begin("Historique");
    GameUiComponents::drawTurnStatusCard(_game.getCurrentTurnNumber(), _game.getActivePlayerName(), _game.getCurrentTurnColor());
    ImGui::Spacing();
    GameUiComponents::drawMoveHistoryList(_game.getMoveHistory());
    ImGui::End();
}
