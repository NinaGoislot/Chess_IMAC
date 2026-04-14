#pragma once

#include "Game/State/MatchState.hpp"
#include "Render/TextureManager.hpp"

// Handles pending pawn promotion UI and validates selected promotion piece.
class PromotionFlow {
public:
    // Constructors
    PromotionFlow(MatchState& matchState, const TextureManager& textures);

    bool getHasPendingPromotion() const;
    // Render function: draws promotion popup and choice buttons.
    void drawPopup();

private:
    // Confirms user promotion choice and applies piece replacement.
    bool        confirmChoice(PieceType type);
    // Cancels current promotion choice and restores previous state.
    void        cancelChoice();
    const char* pieceLabel(PieceType type) const;

    // Non-owning runtime dependencies.
    MatchState*           _match    = nullptr;
    const TextureManager* _textures = nullptr;
};
