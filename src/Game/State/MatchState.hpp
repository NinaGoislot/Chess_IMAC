#pragma once
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Board/Board.hpp"
#include "Game/Chaos/ChaosMode.hpp"
#include "Game/Player.hpp"
#include "Game/State/GameRules.hpp"
#include "Game/State/MoveExecutor.hpp"
#include "Game/State/PromotionService.hpp"
#include "Managers/TurnManager.hpp"
#include "Render/TextureManager.hpp"
#include "utilities/MoveAttempt.hpp"

// Owns a complete chess match state and orchestrates rules, move execution, and chaos.
class MatchState {
public:
    enum class Mode {
        Classic,
        Chaos,
    };

    // Constructors
    explicit MatchState(const TextureManager& textures);

    // Lifecycle
    void newMatch(Mode mode = Mode::Classic);

    // Rules pipeline
    bool tryMove(Vector2D from, Vector2D to);
    bool choosePromotion(PieceType type);
    void cancelPendingPromotion();

    // Selection/rules helpers for UI systems.
    bool                  canSelect(Vector2D tile) const;
    std::vector<Vector2D> getLegalMovesFrom(Vector2D from) const;

    using PendingPromotionInfo = PromotionService::PendingPromotionInfo;

    // Getters
    const Board&                      getBoard() const;
    Board&                            getBoard();
    PieceColor                        getCurrentTurn() const;
    TurnManager&                      getTurnManager();
    const TurnManager&                getTurnManager() const;
    const std::vector<std::string>&   getMoveHistory() const;
    std::optional<std::pair<int, int>> getKirbyPosition() const;
    bool                              getHasKirbyAt(int x, int y) const;
    bool                              getHasPendingPromotion() const;
    std::optional<PendingPromotionInfo> getPendingPromotion() const;
    Mode                              getMode() const;
    ChaosOptions&                     getChaosOptionsMutable();
    const ChaosOptions&               getChaosOptions() const;

    // Simple mutators used by menus/UI.
    void addPlayerWhite(const std::string& name);
    void addPlayerBlack(const std::string& name);
    void addMoveToHistory(const std::string& move);

private:
    // Pre-move chaos hook; may alter or cancel attempt.
    bool applyChaosPreMove(MoveAttempt& attempt);
    // Removes captured piece from owning player set.
    void consumeCapturedPiece(Piece* capturedPiece);

    // Initial board setup helpers.
    void placePieces();
    void placePiecesForPlayer(int backRankY, int pawnRankY, Player& owner);
    // Advances turn and triggers chaos turn callbacks.
    void applyTurnProgression();

    // Core match state.
    Board                    _board;
    TurnManager              _turnManager;
    std::array<Player, 2>    _players;
    std::vector<std::string> _moveHistory;

    // Stateless services used to process one move.
    GameRules                _rules;
    MoveExecutor             _executor;
    PromotionService         _promotion;

    // Optional chaos subsystem and texture dependency.
    std::unique_ptr<ChaosMode> _chaosMode;
    const TextureManager*    _textures = nullptr;

    // Current match mode.
    Mode                     _mode     = Mode::Classic;
};