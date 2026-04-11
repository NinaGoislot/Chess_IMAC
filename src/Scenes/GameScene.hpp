#pragma once
#include "Scene.hpp"
#include "Managers/Game.hpp"

class SceneManager;

class GameScene : public Scene {
public:
    explicit GameScene(SceneManager& sceneManager);

    void render() override;

private:
    SceneManager* _sceneManager = nullptr;
    Game& _game;
};