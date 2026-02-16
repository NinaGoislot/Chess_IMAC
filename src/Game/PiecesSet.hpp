#pragma once

#include <memory>
#include <unordered_set>
#include "Pieces/Piece.hpp"


class PiecesSet {
public:
    PiecesSet();

    void addPiece(const std::shared_ptr<Piece>& piece);
    bool getPieceStatus(const std::shared_ptr<Piece>& piece) const;
    void pieceEaten(const std::shared_ptr<Piece>& piece);

private:
    std::unordered_set<std::shared_ptr<Piece>> _alivePieces;
};