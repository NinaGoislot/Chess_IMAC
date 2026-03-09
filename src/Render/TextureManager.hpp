#pragma once
#include <imgui.h>

class TextureManager
{
public:
    ImTextureID whitePawn{};
    ImTextureID blackPawn{};

    void load();
};