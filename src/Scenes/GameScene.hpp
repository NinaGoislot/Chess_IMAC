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
    // ¨Parameters
    SceneManager*     _sceneManager = nullptr;
    GameManager&      _game;
    GameManager::Mode _mode             = GameManager::Mode::Classic;
    bool              _winnerPopupShown = false;
};
