#pragma once

#include <memory>
#include <map>
#include "Pieces/Piece.hpp"


class PiecesSet {
public:
    PiecesSet();

    void addPiece(std::shared_ptr<Piece>& piece);
    bool getPieceStatus(std::shared_ptr<Piece>& piece) const;
    void pieceEaten(std::shared_ptr<Piece>& piece);

private:
    std::map<std::shared_ptr<Piece>, bool> _allPieces;
};