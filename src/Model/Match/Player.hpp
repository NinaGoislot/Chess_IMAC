#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Model/PieceFactory/Piece.hpp"
#include "Model/PieceFactory/PiecesSet.hpp"

class TextureManager;

class Player {
public:
    // Constructors
    Player();
    Player(PieceColor color, const std::string& name);
    Player(PieceColor color, const std::string& name, const TextureManager& textures);

    // Getters
    const PieceColor&                          getColor() const { return _color; }
    const std::string&                         getName() const { return _name; }
    const std::vector<std::unique_ptr<Piece>>& getAllPieces() const;
    std::size_t                                getAliveCount() const;

    // Setters
    void setName(const std::string& name) { _name = name; }

    // lifecycle
    Piece* addPiece(PieceType type);
    void   removePiece(const Piece& piece);
    bool   owns(const Piece& piece) const;
    void   resetPieces();

private:
    // Parameters
    PieceColor  _color;
    std::string _name;
    PiecesSet   _piecesSet;
};
