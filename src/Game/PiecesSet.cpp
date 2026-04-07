#include "PiecesSet.hpp"
// // #include "Pieces/pawn.hpp"



PiecesSet::PiecesSet() = default;

void PiecesSet::addPiece(const Piece& piece) {
    _alivePieces.insert(&piece);
}

bool PiecesSet::isAlive(const Piece& piece) const {
    return _alivePieces.contains(&piece);
}

void PiecesSet::pieceEaten(const Piece& piece) {
    _alivePieces.erase(&piece);
}

void PiecesSet::clear() {
    _alivePieces.clear();
}

std::size_t PiecesSet::aliveCount() const {
    return _alivePieces.size();
}



// PiecesSet::PiecesSet() {
//     std::shared_ptr<Piece> pawn1 = std::make_shared<Pawn>();
//     addPiece(pawn1);
// }  

// PiecesSet::PiecesSet() {
    
// // }  

// void PiecesSet::addPiece(const std::shared_ptr<Piece>& piece) {
//     _alivePieces.insert(piece);
// }

// // bool PiecesSet::getPieceStatus(const Piece& piece) const {
// //     return _alivePieces.contains(piece);
// // }

// void PiecesSet::pieceEaten(const std::shared_ptr<Piece>& piece) {
//     _alivePieces.erase(piece);
//     // _pieceEatenCallback(*piece);        
// }