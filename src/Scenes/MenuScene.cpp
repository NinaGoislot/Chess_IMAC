#include "MenuScene.hpp"
#include <imgui.h>
#include <array>
#include "Managers/Game.hpp"
#include "Managers/SceneManager.hpp"

char _whitePlayerName[64] = "";
char _blackPlayerName[64] = "";
bool _openPlayerModal     = false;

MenuScene::MenuScene(SceneManager& sceneManager)
    : _sceneManager(&sceneManager)
{}

void MenuScene::render()
{
    settings& gameSettings = Game::instance().getSettings();

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
        // _sceneManager->launchGameScene();
        ImGui::OpenPopup("Setup Partie");
    }

    ImGui::Spacing();

    // BTN chaos mode
    ImGui::BeginDisabled();
    ImGui::Button("Mode Chaos (A venir)", ImVec2(buttonWidth, 45.f));
    ImGui::EndDisabled();

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
    if (ImGui::BeginPopupModal("Setup Partie", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Configuration de la partie");
        ImGui::Separator();

        ImGui::InputText("Joueur Blanc", _whitePlayerName, IM_ARRAYSIZE(_whitePlayerName));
        ImGui::InputText("Joueur Noir", _blackPlayerName, IM_ARRAYSIZE(_blackPlayerName));

        ImGui::Spacing();

        if (ImGui::Button("Lancer la partie", ImVec2(200.f, 0.f)))
        {
            // Game::instance().setPlayerNames(_whitePlayerName, _blackPlayerName);
            Game::instance().addPlayerWhite(_whitePlayerName);
            Game::instance().addPlayerBlack(_blackPlayerName);

            Game::instance().addMoveToHistory(
                std::string("Début du match entre ")
                + _whitePlayerName
                + " et "
                + _blackPlayerName
                + " !"
            );

            _sceneManager->launchGameScene();
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
