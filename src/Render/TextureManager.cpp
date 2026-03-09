#include "TextureManager.hpp"
#include "TextureLoader.hpp"

void TextureManager::load()
{
    whitePawn = LoadTexture("/textures/pieces/white_pawn.png");
    // blackPawn = LoadTexture("../../textures/pieces/black_pawn.png");

    printf("White pawn texture: %p\n", whitePawn);
}