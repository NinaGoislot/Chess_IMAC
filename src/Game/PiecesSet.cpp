#include "PiecesSet.hpp"
#include "Pieces/pawn.hpp"

PiecesSet::PiecesSet() {
    std::shared_ptr<Piece> pawn1 = std::make_shared<Pawn>();
    addPiece(pawn1);
}  

void PiecesSet::addPiece(std::shared_ptr<Piece>& piece) {
    _allPieces[piece] = true;
}

bool PiecesSet::getPieceStatus(std::shared_ptr<Piece>& piece) const {
    auto it = _allPieces.find(piece);
    if (it != _allPieces.end()) {
        return it->second;
    }
    return false; // Piece not found
}

void PiecesSet::pieceEaten(std::shared_ptr<Piece>& piece) {
    auto it = _allPieces.find(piece);
    if (it != _allPieces.end()) {
        it->second = false;
    }
}    