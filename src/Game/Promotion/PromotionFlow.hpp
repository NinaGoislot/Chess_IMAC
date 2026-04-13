#pragma once

#include <array>
#include <functional>
#include "Board/Board.hpp"
#include "Game/Player.hpp"
#include "Managers/TurnManager.hpp"
#include "Render/TextureManager.hpp"

// Handles pending pawn promotion UI and validates selected promotion piece.
class PromotionFlow {
public:
    // Constructors
    PromotionFlow(Board& board, std::array<Player, 2>& players, TurnManager& turnManager, const TextureManager& textures);

    bool getHasPendingPromotion() const;
    // Render function: draws promotion popup and choice buttons.
    void drawPopup();
    // Setters
    void setOnMoveValidated(std::function<void()> callback);

private:
    // Confirms user promotion choice and applies piece replacement.
    bool        confirmChoice(PieceType type);
    // Cancels current promotion choice and restores previous state.
    void        cancelChoice();
    const char* pieceLabel(PieceType type) const;

    // Non-owning runtime dependencies.
    Board*                 _board       = nullptr;
    std::array<Player, 2>* _players     = nullptr;
    TurnManager*           _turnManager = nullptr;
    const TextureManager*  _textures    = nullptr;
    // Callback triggered after promotion confirms a completed move.
    std::function<void()>  _onMoveValidated;
};
