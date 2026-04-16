#pragma once

#include "Model/Chaos/ChaosMode.hpp"
#include "Model/Match/Promotion/PromotionFlow.hpp"
#include "Model/Match/MatchState.hpp"
#include "Model/settings.hpp"
#include "Render/Renderer.hpp"
#include "Render/TextureManager.hpp"
#include "Input/MoveSelectionController.hpp"
#include "utilities/AppConfig.hpp"

// High-level app game controller
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

    // Getters
    settings&                       getSettings();
    const settings&                 getSettings() const;
    const std::vector<std::string>& getMoveHistory() const;
    ChaosOptions&                   getChaosOptionsMutable();
    const ChaosOptions&             getChaosOptions() const;
    PromotionFlow&                  getPromotionFlow();
    Mode                            getMode() const;
    const std::string&              getWhitePlayerName() const;
    const std::string&              getBlackPlayerName() const;
    int                             getFullTurnCount() const;
    bool                            getHasWinner() const;
    const Player*                   getWinner() const;

    // Adders
    void addPlayerWhite(const std::string& name);
    void addPlayerBlack(const std::string& name);
    void addMoveToHistory(const std::string& move);

private:
    void handleBoardClick(Vector2D clickedTile);

    // Parameters
    settings       _settings;
    TextureManager _textures;
    Renderer       _renderer;
    MatchState     _match;
    PromotionFlow  _promotionFlow;
    MoveSelectionController     _moveSelectionController;

    // Current session mode
    Mode _mode = Mode::Classic;
};