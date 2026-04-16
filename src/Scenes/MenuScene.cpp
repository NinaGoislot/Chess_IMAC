#include "MenuScene.hpp"
#include <imgui.h>
#include "Managers/SceneManager.hpp"
#include "Model/Chaos/ChaosMode.hpp"
#include "UI/GameUiComponents.hpp"
#include "utilities/UiTheme.hpp"

namespace {
/**
 *
 * Affiche les options du mode Chaos dans la fenetre de setup.
 * @param options : options Chaos a modifier.
 * @return Aucun.
 */
void drawChaosRulesSection(ChaosOptions& options)
{
    ImGui::Separator();
    ImGui::Text("Regles Chaos actives");

    ImGui::Checkbox("1. Loi de Weibull (duree de vie)", &options.enableWeibullLifetime);
    ImGui::TextDisabled("La piece perd de la duree de vie uniquement quand elle est jouee.");

    ImGui::Checkbox("2-3. Bernoulli + uniforme discrete (setup pieces)", &options.enableBernoulliBackrowAndShuffle);
    ImGui::TextDisabled("Chance d'avoir des pions en backrow puis melange aleatoire des lignes de depart.");

    ImGui::Checkbox("4. Poisson + uniforme (Kirby)", &options.enableKirbyPoissonUniform);
    ImGui::TextDisabled("Kirby peut apparaitre sur une case et manger une piece voisine au hasard.");

    ImGui::Checkbox("7-8. Geometrique + Bernoulli (glissantes + obeissance)", &options.enableGeometricSlidingAndObedience);
    ImGui::TextDisabled("Les pieces glissantes peuvent s'arreter avant; certaines pieces refusent d'obeir.");
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
 * Affiche le resume d'une partie interrompue et le bouton de reprise.
 * @param game : gestionnaire du match courant.
 * @param buttonWidth : largeur cible de la carte.
 * @return Aucun.
 */
void MenuScene::drawInterruptedMatchCard(GameManager& game, float buttonWidth)
{
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, UiTheme::secondaryButton.toImVec4());
    ImGui::BeginChild("InterruptedMatchCard", ImVec2(buttonWidth, 145.f), true);
    ImGui::TextColored(UiTheme::panelStrong.toImVec4(), "Partie interrompue");

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
 * Affiche la fenetre de confirmation avant d'ecraser une partie interrompue.
 * @return Aucun.
 */
void MenuScene::drawOverwritePopup()
{
    if (!ImGui::BeginPopupModal("Confirmer ecrasement", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    ImGui::TextUnformatted("Une partie interrompue existe deja.");
    ImGui::TextUnformatted("Creer une nouvelle partie ecrasera cette sauvegarde.");
    ImGui::Separator();

    if (GameUiComponents::drawDangerButton("Ecraser et continuer", ImVec2(220.f, 0.f)))
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
 * Affiche la fenetre de configuration d'une nouvelle partie.
 * @param game : gestionnaire du match courant.
 * @return Aucun.
 */
void MenuScene::drawSetupPopup(GameManager& game)
{
    if (!ImGui::BeginPopupModal("Setup Partie", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    ImGui::TextColored(UiTheme::panelHeader.toImVec4(), "Configuration de la partie");
    ImGui::TextColored(UiTheme::mutedText.toImVec4(), "Personnalise les joueurs puis lance la partie.");
    ImGui::Separator();

    ImGui::InputText("Joueur Blanc", _whitePlayerName.data(), static_cast<int>(_whitePlayerName.size()));
    ImGui::InputText("Joueur Noir", _blackPlayerName.data(), static_cast<int>(_blackPlayerName.size()));

    if (_selectedMode == 1)
    {
        drawChaosRulesSection(game.getChaosOptionsMutable());
    }

    ImGui::Spacing();

    if (GameUiComponents::drawPrimaryButton("Lancer la partie", ImVec2(200.f, 0.f)))
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

    if (GameUiComponents::drawSecondaryButton("Annuler", ImVec2(120.f, 0.f)))
    {
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

/**
 *
 * Construit l'interface principale du menu avec ses popups associes.
 * @return Aucun.
 */
void MenuScene::render()
{
    GameManager& game = _sceneManager->getGame();

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
        "Chess Arena",
        "Une interface propre, moderne et lisible pour lancer ta partie.",
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
}
