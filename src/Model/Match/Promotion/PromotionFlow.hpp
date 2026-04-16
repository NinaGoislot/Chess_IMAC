#pragma once

#include "Model/Match/MatchState.hpp"
#include "Render/TextureManager.hpp"

class PromotionFlow {
public:
    // Constructors
    PromotionFlow(MatchState& matchState, const TextureManager& textures);

    // Getters
    bool getHasPendingPromotion() const;

    // Render
    void drawPopup();

private:
    bool        confirmChoice(PieceType type);
    void        cancelChoice();
    const char* pieceLabel(PieceType type) const;

    // Parameters
    MatchState*           _match    = nullptr;
    const TextureManager* _textures = nullptr;
};
