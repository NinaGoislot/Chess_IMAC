#include "Player.hpp"

Player::Player()
	: _color(PieceColor::White)
	, _name("Nullos")
{
}

Player::Player(PieceColor color, const std::string& name)
	: _color(color)
	, _name(name)
{
}

void Player::addPiece(const Piece& piece)
{
	_piecesSet.addPiece(piece);
}

void Player::removePiece(const Piece& piece)
{
	_piecesSet.pieceEaten(piece);
}

bool Player::owns(const Piece& piece) const
{
	return _piecesSet.isAlive(piece);
}

void Player::resetPieces()
{
	_piecesSet.clear();
}

std::size_t Player::aliveCount() const
{
	return _piecesSet.aliveCount();
}