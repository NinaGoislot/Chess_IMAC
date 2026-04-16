#pragma once
#include "Managers/GameManager.hpp"
#include "Scene.hpp"

class SceneManager;

// Gameplay scene that renders and drives an active chess game.
class GameScene : public Scene {
public:
    // Constructors
    explicit GameScene(SceneManager& sceneManager, GameManager::Mode mode);

    // Render function: draws game UI, board, and interactions.
    void render() override;

private:
    // Non-owning scene manager used for scene transitions.
    SceneManager* _sceneManager = nullptr;
    // Reference to the app controller.
    GameManager& _game;
    // Mode used for current match instance.
    GameManager::Mode _mode             = GameManager::Mode::Classic;
    bool              _winnerPopupShown = false;
};
