#pragma once
#include "Managers/Game.hpp"
#include "Scene.hpp"


class SceneManager;

// Gameplay scene that renders and drives an active chess game.
class GameScene : public Scene {
public:
    // Constructors
    explicit GameScene(SceneManager& sceneManager, Game::Mode mode);

    // Render function: draws game UI, board, and interactions.
    void render() override;

private:
    // Non-owning scene manager used for scene transitions.
    SceneManager* _sceneManager = nullptr;
    // Reference to the singleton game state.
    Game&         _game;
    // Mode used for current match instance.
    Game::Mode    _mode = Game::Mode::Classic;
};
