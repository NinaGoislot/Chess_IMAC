#include "Player.hpp"
#include "Render/TextureManager.hpp"


// CONSTRUCTORS

Player::Player()
	: _color(PieceColor::White)
	, _name("Nullos")
	, _piecesSet(_color)
{
}

Player::Player(PieceColor color, const std::string& name)
	: _color(color)
	, _name(name)
	, _piecesSet(_color)
{
}

Player::Player(PieceColor color, const std::string& name, const TextureManager& textures)
	: _color(color)
	, _name(name)
	, _piecesSet(_color, &textures)
{
}

// GETTERS

const std::vector<std::unique_ptr<Piece>>& Player::getAllPieces() const
{
	return _piecesSet.getAllPieces();
}

// UPDATES

void Player::addPiece(const Piece& piece)
{
	_piecesSet.addPiece(piece);
}

void Player::removePiece(const Piece& piece)
{
	_piecesSet.pieceEaten(&piece);
}

// FUNCTIONS

bool Player::owns(const Piece& piece) const
{
	return _piecesSet.isAlive(&piece);
}

void Player::resetPieces()
{
	_piecesSet.clear();
}

std::size_t Player::aliveCount() const
{
	return _piecesSet.aliveCount();
}