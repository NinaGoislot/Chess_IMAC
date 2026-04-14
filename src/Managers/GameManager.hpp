#pragma once

#include "Game/Chaos/ChaosMode.hpp"
#include "Game/Promotion/PromotionFlow.hpp"
#include "Game/State/MatchState.hpp"
#include "Game/settings.hpp"
#include "Managers/InputManager.hpp"
#include "Render/Renderer.hpp"
#include "Render/TextureManager.hpp"
#include "Systems/MoveSystem.hpp"
#include "utilities/AppConfig.hpp"

// High-level app game controller used by scenes and UI.
class GameManager {
public:
    enum class Mode {
        Classic,
        Chaos,
    };

    // Constructors
    explicit GameManager(const AppConfig& config);

    // Lifecycle
    void newGame(Mode mode = Mode::Classic);

    // Render/update entry point
    void displayBoard(float deltaTimeSeconds);

    // Input callbacks
    void onCursorPosition(double xpos, double ypos);
    void onScroll(double xoffset, double yoffset);

    // Getters
    settings&                     getSettings();
    const settings&               getSettings() const;
    const std::vector<std::string>& getMoveHistory() const;
    ChaosOptions&                 getChaosOptionsMutable();
    const ChaosOptions&           getChaosOptions() const;
    PromotionFlow&                getPromotionFlow();
    Mode                          getMode() const;

    // Mutators used by menu and UI widgets
    void addPlayerWhite(const std::string& name);
    void addPlayerBlack(const std::string& name);
    void addMoveToHistory(const std::string& move);

private:
    // Applies one tile click intent to the move/selection system.
    void handleBoardClick(Vector2D clickedTile);

    // Runtime dependencies and owned game subsystems.
    settings       _settings;
    TextureManager _textures;
    Renderer       _renderer;
    MatchState     _match;
    PromotionFlow  _promotionFlow;
    MoveSystem     _moveSystem;
    InputManager   _inputManager;

    // Current session mode.
    Mode _mode = Mode::Classic;
};