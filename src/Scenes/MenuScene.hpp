#pragma once
#include <array>
#include "Scene.hpp"

class SceneManager;

// Main menu scene used to choose game mode and start a match.
class MenuScene : public Scene {
public:
    // Constructors
    explicit MenuScene(SceneManager& sceneManager);

    // Render function: draws menu UI and mode buttons.
    void render() override;

private:
    // Non-owning scene manager used to launch next scenes.
    SceneManager* _sceneManager = nullptr;
    // Selected mode index from menu controls.
    int           _selectedMode = 0;
    // Player names are explicit scene state, not hidden globals.
    std::array<char, 64> _whitePlayerName{};
    std::array<char, 64> _blackPlayerName{};
};
