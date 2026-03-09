#pragma once

#include <string>
#include "PiecesSet.hpp"

class Player {
public:
    Player();
    explicit Player(const std::string& name) : _name(name) {};

private:
    std::string _name;
    PiecesSet   _piecesSet;
};