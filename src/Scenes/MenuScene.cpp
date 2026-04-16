#include "MenuScene.hpp"
#include <imgui.h>
#include "Managers/SceneManager.hpp"
#include "Model/Chaos/ChaosMode.hpp"
#include "UI/GameUiComponents.hpp"
#include "utilities/UiTheme.hpp"

namespace {
/**
 *
 * Affiche les options du mode Chaos dans la fenetre de setup
 * @param options : options Chaos a modifier
 * @return Aucun
 */
void drawChaosRulesSection(ChaosOptions& options)
{
    ImGui::Separator();
    ImGui::Text("Regles Chaos activable (have fun) :");

    ImGui::Checkbox("1. Loi de Weibull (duree de vie)", &options.enableWeibullLifetime);
    ImGui::TextDisabled("La piece perd de la duree de vi quand elle est jouee. Tic Tac, tic tac...");

    ImGui::Checkbox("2. Bernoulli + uniforme discrete (setup pieces)", &options.enableBernoulliBackrowAndShuffle);
    ImGui::TextDisabled("Mélange aleatoire des lignes de depart.");

    ImGui::Checkbox("3. Poisson + uniforme (Kirby)", &options.enableKirbyPoissonUniform);
    ImGui::TextDisabled("Kirby a faim. Il peut apparaitre sur une case et manger des piece au hasard :)");

    ImGui::Checkbox("4. Geometrique + Bernoulli (glissantes + obeissance)", &options.enableGeometricSlidingAndObedience);
    ImGui::TextDisabled("Les pieces peuvent arreter d'obéir. Les pièces glissantes peuvent s'arrêter avant la fin du mouvement complet.");
}
} // namespace

MenuScene::MenuScene(SceneManager& sceneManager)
    : _sceneManager(&sceneManager)
{}

/**
 *
 * Affiche les actions principales du menu (modes et sortie).
 * @param buttonWidth : largeur des boutons principaux.
 * @return Aucun.
 */
void MenuScene::drawMainActions(float buttonWidth)
{
    if (GameUiComponents::drawPrimaryButton("Lancer mode classique", ImVec2(buttonWidth, 48.f)))
    {
        _requestedMode = 0;
        if (_sceneManager->hasInterruptedMatch())
            ImGui::OpenPopup("Confirmer ecrasement");
        else
        {
            _selectedMode = _requestedMode;
            ImGui::OpenPopup("Setup Partie");
        }
    }

    ImGui::Spacing();

    if (GameUiComponents::drawSecondaryButton("Lancer mode chaos", ImVec2(buttonWidth, 48.f)))
    {
        _requestedMode = 1;
        if (_sceneManager->hasInterruptedMatch())
            ImGui::OpenPopup("Confirmer ecrasement");
        else
        {
            _selectedMode = _requestedMode;
            ImGui::OpenPopup("Setup Partie");
        }
    }

    ImGui::Spacing();

    if (GameUiComponents::drawDangerButton("Quitter", ImVec2(buttonWidth, 44.f)))
    {
        _sceneManager->requestQuit();
    }
}

/**
 *
 * Affiche le resume d'une partie interrompue
 * @param game : gestionnaire du match courant
 * @param buttonWidth : largeur cible de la carte
 * @param height : hauteur cible de la carte
 * @return Aucun.
 */
void MenuScene::drawInterruptedMatchCard(GameManager& game, float buttonWidth, float height)
{
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, UiTheme::secondaryButton.toImVec4());
    ImGui::BeginChild("InterruptedMatchCard", ImVec2(buttonWidth, height), true);
    ImGui::TextColored(UiTheme::panelStrong.toImVec4(), "Partie récente");

    ImGui::Text("Joueur blanc: %s", game.getWhitePlayerName().c_str());
    ImGui::Text("Joueur noir: %s", game.getBlackPlayerName().c_str());
    ImGui::Text("Mode: %s", game.getMode() == GameManager::Mode::Chaos ? "Chaos" : "Classique");
    ImGui::Text("Tours joues: %d", game.getFullTurnCount());

    if (GameUiComponents::drawPrimaryButton("Reprendre", ImVec2(180.f, 0.f)))
    {
        _sceneManager->resumeInterruptedMatch();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

/**
 *
 * Affiche la fenetre de confirmation avant d'ecraser une partie interrompue
 * @return Aucun
 */
void MenuScene::drawOverwritePopup()
{
    if (!ImGui::BeginPopupModal("Confirmer ecrasement", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    ImGui::TextUnformatted("Une partie interrompue existe deja.");
    ImGui::TextUnformatted("Creer une nouvelle partie ecrasera cette sauvegarde.");
    ImGui::Separator();

    if (GameUiComponents::drawDangerButton("Ecrase tout !", ImVec2(220.f, 0.f)))
    {
        _sceneManager->clearInterruptedMatch();
        _selectedMode            = _requestedMode;
        _openSetupPopupNextFrame = true;
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (GameUiComponents::drawSecondaryButton("Annuler", ImVec2(120.f, 0.f)))
    {
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

/**
 *
 * Affiche la fenetre de configuration d'une nouvelle partie
 * @param game : gestionnaire du match courant
 * @return Aucun
 */
void MenuScene::drawSetupPopup(GameManager& game)
{
    ImGui::SetNextWindowSize(ImVec2(900.f, 560.f), ImGuiCond_Appearing);
    if (!ImGui::BeginPopupModal("Setup Partie", nullptr, ImGuiWindowFlags_NoResize))
        return;

    ImGui::TextColored(UiTheme::panelHeader.toImVec4(), "Configuration de la partie");
    ImGui::Separator();

    ImGui::InputText("Joueur Blanc", _whitePlayerName.data(), static_cast<int>(_whitePlayerName.size()));
    ImGui::InputText("Joueur Noir", _blackPlayerName.data(), static_cast<int>(_blackPlayerName.size()));

    if (_selectedMode == 1)
    {
        drawChaosRulesSection(game.getChaosOptionsMutable());
    }

    ImGui::Spacing();

    if (GameUiComponents::drawPrimaryButton("Lancer la partie de fou !", ImVec2(390.f, 50.f)))
    {
        game.addPlayerWhite(_whitePlayerName.data());
        game.addPlayerBlack(_blackPlayerName.data());

        game.addMoveToHistory(
            std::string("Début du match entre ")
            + _whitePlayerName.data()
            + " et "
            + _blackPlayerName.data()
            + " !"
        );

        const GameManager::Mode mode = (_selectedMode == 1) ? GameManager::Mode::Chaos : GameManager::Mode::Classic;
        _sceneManager->launchGameScene(mode, true);
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (GameUiComponents::drawSecondaryButton("Annuler", ImVec2(120.f, 50.f)))
    {
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

/**
 *
 * Construit l'interface principale du menu avec ses popups associes
 * @return Aucun
 */
void MenuScene::render()
{
    GameManager& game = _sceneManager->getGame();

    ImFont*     menuFont = nullptr;
    const auto& fonts    = ImGui::GetIO().Fonts->Fonts;
    if (fonts.Size > 1)
    {
        // Font #0: regular default, Font #1: larger/heavier variant loaded at startup.
        menuFont = fonts[1];
        ImGui::PushFont(menuFont);
    }

    if (_openSetupPopupNextFrame)
    {
        ImGui::OpenPopup("Setup Partie");
        _openSetupPopupNextFrame = false;
    }

    ImGui::SetNextWindowSize(ImVec2(560.f, 660.f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Menu principal", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.f, 10.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.f);

    GameUiComponents::drawMenuHero({
        "Le super jeu d'echecs de Paul et Nina !",
        "Un banger absolu.",
    });

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const float buttonWidth = 320.f;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - buttonWidth) * 0.5f);
    ImGui::BeginGroup();

    drawMainActions(buttonWidth);

    ImGui::EndGroup();

    if (_sceneManager->hasInterruptedMatch())
    {
        drawInterruptedMatchCard(game, buttonWidth);
    }

    drawOverwritePopup();
    drawSetupPopup(game);

    ImGui::PopStyleVar(2);

    ImGui::End();

    if (menuFont != nullptr)
    {
        ImGui::PopFont();
    }
}
