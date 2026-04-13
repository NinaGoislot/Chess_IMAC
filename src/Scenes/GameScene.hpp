#pragma once
#include "Managers/Game.hpp"
#include "Scene.hpp"


class SceneManager;

class GameScene : public Scene {
public:
    explicit GameScene(SceneManager& sceneManager, Game::Mode mode);

    void render() override;

private:
    SceneManager* _sceneManager = nullptr;
    Game&         _game;
    Game::Mode    _mode = Game::Mode::Classic;
};