#pragma once

#include <array>
#include <functional>
#include "Board/Board.hpp"
#include "Game/Player.hpp"
#include "Managers/TurnManager.hpp"
#include "Render/TextureManager.hpp"

class PromotionFlow {
public:
    PromotionFlow(Board& board, std::array<Player, 2>& players, TurnManager& turnManager, const TextureManager& textures);

    bool hasPendingPromotion() const;
    void drawPopup();
    void setOnMoveValidated(std::function<void()> callback);

private:
    bool        confirmChoice(PieceType type);
    void        cancelChoice();
    const char* pieceLabel(PieceType type) const;

    Board*                 _board       = nullptr;
    std::array<Player, 2>* _players     = nullptr;
    TurnManager*           _turnManager = nullptr;
    const TextureManager*  _textures    = nullptr;
    std::function<void()>  _onMoveValidated;
};
