#pragma once
#include <imgui.h>

struct settings {
    ImVec4 whiteColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 blackColor = ImVec4(0.10f, 0.15f, 0.20f, 1.00f);
    ImVec4 _highlight{0.2f, 0.8f, 0.2f, 1.f};
    
    float buttonSize = 50.f;

    void display();

    ImVec4 getWhite() const { return whiteColor; }
    ImVec4 getBlack() const { return blackColor; }
    ImVec4 getHighlight() const { return _highlight; }
};
