#pragma once
#include "Scene.hpp"

class SceneManager;

class MenuScene : public Scene {
public:
    explicit MenuScene(SceneManager& sceneManager);

    void render() override;

private:
    SceneManager* _sceneManager = nullptr;
    int           _selectedMode = 0;
};