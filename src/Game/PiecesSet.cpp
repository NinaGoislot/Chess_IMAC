// #include "PiecesSet.hpp"
// #include "Pieces/pawn.hpp"



// PiecesSet::PiecesSet(TextureManager& textures) {
//     std::shared_ptr<Piece> pawn1 = std::make_shared<Pawn>(textures.whitePawn);
//     addPiece(pawn1);
// }  

// void PiecesSet::addPiece(const std::shared_ptr<Piece>& piece) {
//     _alivePieces.insert(piece);
// }

// bool PiecesSet::getPieceStatus(const std::shared_ptr<Piece>& piece) const {
//     return _alivePieces.contains(piece); // C++20
// }

// void PiecesSet::pieceEaten(const std::shared_ptr<Piece>& piece) {
//     _alivePieces.erase(piece);
// }