#pragma once
#include <string>

class Piece
{
public:
    Piece(const std::string& name) { _name = name; };
    Piece(Piece &&) = default;
    Piece(const Piece &) = default;
    Piece &operator=(Piece &&) = default;
    Piece &operator=(const Piece &) = default;
    ~Piece();

    std::strng getName() const { return _name; }
    
private:
    std::string _name;
};

Piece::Piece(const std::string& name)
{
    _name = name;
}

Piece::~Piece()
{
}