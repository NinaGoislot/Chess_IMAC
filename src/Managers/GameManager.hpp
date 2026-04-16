#pragma once

#include "Model/Chaos/ChaosMode.hpp"
#include "Model/Match/MatchState.hpp"
#include "Model/Match/Promotion/PromotionFlow.hpp"
#include "Model/Match/Selection/MoveSelectionController.hpp"
#include "Model/settings.hpp"
#include "Render/Renderer.hpp"
#include "Render/TextureManager.hpp"
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

    // Render/update entry points
    void beginBoardViewsFrame();
    void draw3DBoardView(float deltaTimeSeconds);
    void draw2DBoardView();
    void endBoardViewsFrame();

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
    const std::string&              getActivePlayerName() const;
    int                             getFullTurnCount() const;
    int                             getCurrentTurnNumber() const;
    PieceColor                      getCurrentTurnColor() const;
    bool                            getHasWinner() const;
    const Player*                   getWinner() const;

    // Adders
    void addPlayerWhite(const std::string& name);
    void addPlayerBlack(const std::string& name);
    void addMoveToHistory(const std::string& move);

private:
    void collectBoardInteraction(const std::optional<BoardClick>& clickedTile, const std::optional<BoardClick>& hoveredTile);

    // Parameters
    settings                _settings;
    TextureManager          _textures;
    Renderer                _renderer;
    MatchState              _match;
    PromotionFlow           _promotionFlow;
    MoveSelectionController _moveSelectionController;

    std::optional<BoardClick> _pendingClickedTile;
    std::optional<BoardClick> _pendingHoveredTile;

    // Current session mode
    Mode _mode = Mode::Classic;
};