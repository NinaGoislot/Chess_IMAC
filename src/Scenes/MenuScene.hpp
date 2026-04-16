#pragma once
#include <array>
#include "Scene.hpp"

class SceneManager;
class GameManager;

// Main menu scene used to choose game mode and start a match
class MenuScene : public Scene {
public:
    // Constructors
    explicit MenuScene(SceneManager& sceneManager);

    // Render function
    void render() override;

private:
    void drawMainActions(float buttonWidth);
    void drawInterruptedMatchCard(GameManager& game, float buttonWidth, float height = 200.f);
    void drawOverwritePopup();
    void drawSetupPopup(GameManager& game);

    // Parameters
    SceneManager*        _sceneManager            = nullptr;
    int                  _selectedMode            = 0;
    int                  _requestedMode           = 0;
    bool                 _openSetupPopupNextFrame = false;
    std::array<char, 64> _whitePlayerName{};
    std::array<char, 64> _blackPlayerName{};
};
