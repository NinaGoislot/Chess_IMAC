#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Pieces/Piece.hpp"
#include "PiecesSet.hpp"

class TextureManager;

// Represents one player with identity, color, and owned pieces.
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

    // Creates and adds one piece of the given type.
    Piece*      addPiece(PieceType type);
    // Removes a piece from alive state after capture.
    void        removePiece(const Piece& piece);
    // Returns true if this player owns the given piece instance.
    bool        owns(const Piece& piece) const;
    // Resets the player's full piece set.
    void        resetPieces();
    std::size_t getAliveCount() const;

private:
    // Player color used for move ownership and rendering.
    PieceColor  _color;
    // Player display name shown in UI.
    std::string _name;
    // Owned piece container and lifecycle manager.
    PiecesSet   _piecesSet;
};

