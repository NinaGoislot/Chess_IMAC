#pragma once

#include "Model/PieceFactory/Piece.hpp"

namespace PieceColorUtils {

inline const char* toFrenchLabel(PieceColor color)
{
    return (color == PieceColor::White) ? "Blanc" : "Noir";
}

} // namespace PieceColorUtils
