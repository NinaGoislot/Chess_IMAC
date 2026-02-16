#include "PiecesSet.hpp"
#include "Pieces/pawn.hpp"

PiecesSet::PiecesSet() {
    std::shared_ptr<Piece> pawn1 = std::make_shared<Pawn>();
    addPiece(pawn1);
}  

void PiecesSet::addPiece(const std::shared_ptr<Piece>& piece) {
    _alivePieces.insert(piece);
}

// bool PiecesSet::getPieceStatus(const Piece& piece) const {
//     return _alivePieces.contains(piece);
// }

void PiecesSet::pieceEaten(const std::shared_ptr<Piece>& piece) {
    _alivePieces.erase(piece);
    // _pieceEatenCallback(*piece);        
}