// #pragma once

// #include <memory>
// #include <string>
// #include "Pieces/Piece.hpp"
// #include "PiecesSet.hpp"

// class TextureManager;

// class Player {
// public:
//     Player();
//     Player(PieceColor color, const std::string& name);
//     Player(PieceColor color, const std::string& name, const TextureManager& textures);

//     const PieceColor& getColor() const { return _color; }
//     const std::string& getName() const { return _name; }

//     std::unique_ptr<Piece> takePiece(const PieceType& type);

//     void addPiece(const Piece& piece);
//     void removePiece(const Piece& piece);
//     bool owns(const Piece& piece) const;
//     void resetPieces();
//     std::size_t aliveCount() const;

// private:
//     PieceColor _color;
//     std::string _name;
//     PiecesSet   _piecesSet;
// };

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Pieces/Piece.hpp"
#include "PiecesSet.hpp"

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

    // Setters
    void setName(const std::string& name) { _name = name; }

    // Functions
    Piece*      addPiece(PieceType type);
    void        addPiece(const Piece& piece);
    void        removePiece(const Piece& piece);
    bool        owns(const Piece& piece) const;
    void        resetPieces();
    std::size_t aliveCount() const;

private:
    // Parameters
    PieceColor  _color;
    std::string _name;
    PiecesSet   _piecesSet;
};