#include "Game/State/PromotionService.hpp"

bool PromotionService::getHasPending() const
{
    return _pending.has_value();
}

std::optional<PromotionService::PendingPromotionInfo> PromotionService::getPendingInfo() const
{
    if (!_pending.has_value())
        return std::nullopt;

    PendingPromotionInfo info;
    info.x     = static_cast<int>(_pending->to.getX());
    info.y     = static_cast<int>(_pending->to.getY());
    info.color = _pending->color;
    return info;
}

void PromotionService::clear()
{
    _pending.reset();
}

bool PromotionService::start(const Board::MoveResult& result, PieceColor currentTurn)
{
    if (!result.isPromotion)
        return false;

    // Snapshot board state so the promotion can be resolved or canceled later.
    PendingPromotionState pending;
    pending.from          = result.from;
    pending.to            = result.to;
    pending.pawn          = result.movedPiece;
    pending.capturedPiece = result.capturedPiece;
    pending.color         = (result.movedPiece != nullptr) ? result.movedPiece->getColor() : currentTurn;
    _pending              = pending;
    return true;
}

bool PromotionService::resolve(Board& board, std::array<Player, 2>& players, PieceType type)
{
    if (!_pending.has_value())
        return false;

    // Create promoted piece from the owner pool and place it on destination tile.
    const PendingPromotionState pending = _pending.value();

    Player& owner = (pending.color == PieceColor::White) ? players[0] : players[1];
    Piece*  promotedPiece = owner.addPiece(type);
    if (promotedPiece == nullptr)
        return false;

    const int toX = static_cast<int>(pending.to.getX());
    const int toY = static_cast<int>(pending.to.getY());
    board.getCase(toX, toY).setPiece(promotedPiece);

    consumeCapturedPiece(players, pending.capturedPiece);
    _pending.reset();
    return true;
}

void PromotionService::cancel(Board& board)
{
    if (!_pending.has_value())
        return;

    // Restore board exactly as it was before entering promotion state.
    const PendingPromotionState pending = _pending.value();

    const int fromX = static_cast<int>(pending.from.getX());
    const int fromY = static_cast<int>(pending.from.getY());
    const int toX   = static_cast<int>(pending.to.getX());
    const int toY   = static_cast<int>(pending.to.getY());

    board.getCase(fromX, fromY).setPiece(pending.pawn);
    board.getCase(toX, toY).setPiece(pending.capturedPiece);
    _pending.reset();
}

void PromotionService::consumeCapturedPiece(std::array<Player, 2>& players, Piece* capturedPiece)
{
    if (capturedPiece == nullptr)
        return;

    const int ownerIndex = (capturedPiece->getColor() == PieceColor::White) ? 0 : 1;
    players[ownerIndex].removePiece(*capturedPiece);
}
