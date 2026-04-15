#pragma once

#include <memory>
#include "Managers/GameManager.hpp"
#include "Scenes/Scene.hpp"
#include "utilities/AppConfig.hpp"


// Scene switching state
class SceneManager {
public:
    // Getters
    static SceneManager& instance();

    // Constructors
    SceneManager(const SceneManager&)            = delete;
    SceneManager(SceneManager&&)                 = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager& operator=(SceneManager&&)      = delete;
    ~SceneManager()                              = default;

    // Init
    void init(const AppConfig& config);

    // Render
    void renderCurrentScene();

    // Launchers
    void launchMenuScene();
    void launchGameScene(GameManager::Mode mode = GameManager::Mode::Classic, bool startNew = true);

    // Interrupted match flow
    void saveInterruptedMatch();
    void clearInterruptedMatch();
    bool hasInterruptedMatch() const;
    void resumeInterruptedMatch();

    // Getters
    GameManager&       getGame();
    const GameManager& getGame() const;

    // Requests application shutdown
    void requestQuit();
    bool getShouldQuit() const;

private:
    enum class PendingSceneAction {
        None,
        LaunchMenu,
        LaunchGame,
    };

    SceneManager() = default;
    void applyPendingSceneAction();

    // Parameters
    std::unique_ptr<Scene> _currentScene;
    std::unique_ptr<GameManager> _game;
    bool quitRequested = false;
    bool _isRenderingScene = false;
    bool _hasInterruptedMatch = false;
    PendingSceneAction _pendingSceneAction = PendingSceneAction::None;
    GameManager::Mode _pendingGameMode = GameManager::Mode::Classic;
    bool _pendingStartNewGame = true;
};
