#include "MenuScene.hpp"
#include <imgui.h>
#include "Game/Chaos/ChaosMode.hpp"
#include "Managers/SceneManager.hpp"


namespace {
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

void MenuScene::render()
{
    GameManager& game = _sceneManager->getGame();
    settings& gameSettings = game.getSettings();

    ImGui::Begin("Menu principal");
    ImGui::Text("Bienvenue dans le super jeu d'echecs de Paul et Nina");

    ////////////////////// Sub-title //////////////////////
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Bienvenue!").x) * 0.5f);
    ImGui::Text("Bienvenue!");

    ImGui::Spacing();

    /////////////////////// Buttons ///////////////////////
    float buttonWidth = 280.f;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - buttonWidth) * 0.5f);
    ImGui::BeginGroup();

    // BTN classic mode
    if (ImGui::Button("Mode Classique", ImVec2(buttonWidth, 45.f)))
    {
        _selectedMode = 0;
        ImGui::OpenPopup("Setup Partie");
    }

    ImGui::Spacing();

    // BTN chaos mode
    if (ImGui::Button("Mode Chaos", ImVec2(buttonWidth, 45.f)))
    {
        _selectedMode = 1;
        ImGui::OpenPopup("Setup Partie");
    }

    ImGui::Spacing();

    // BTN Quit
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));

    if (ImGui::Button("Quitter le jeu", ImVec2(buttonWidth, 45.f)))
    {
        _sceneManager->requestQuit();
    }

    ImGui::PopStyleColor(3);
    ImGui::EndGroup();

    /////////////////////// Player setup modal ///////////////////////
    if (ImGui::BeginPopupModal("Setup Partie", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Configuration de la partie");
        ImGui::Separator();

        ImGui::InputText("Joueur Blanc", _whitePlayerName.data(), static_cast<int>(_whitePlayerName.size()));
        ImGui::InputText("Joueur Noir", _blackPlayerName.data(), static_cast<int>(_blackPlayerName.size()));

        if (_selectedMode == 1)
        {
            drawChaosRulesSection(game.getChaosOptionsMutable());
        }

        ImGui::Spacing();

        if (ImGui::Button("Lancer la partie", ImVec2(200.f, 0.f)))
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
            _sceneManager->launchGameScene(mode);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Annuler", ImVec2(120.f, 0.f)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    /////////////////////// END ///////////////////////

    ImGui::End();

    // ImGui::ShowDemoWindow();
}

