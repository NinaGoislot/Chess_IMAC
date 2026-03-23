#pragma once
#include <imgui.h>

struct settings {
    // 2D rendering settings
    ImVec4 whiteColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 blackColor = ImVec4(0.10f, 0.15f, 0.20f, 1.00f);
    float buttonSize = 50.f;

    // 3D rendering settings
    bool use3D = true;
    bool drawPieces3D = true;
    bool cameraPieceTarget = true;
    
    float boardThickness = 0.18f;
    float cameraYawDegrees = 45.f;
    float cameraPitchDegrees = 32.f;
    float cameraDistance = 13.5f;

    // UI settings
    void display();

    ImVec4 getWhite() const { return whiteColor; }
    ImVec4 getBlack() const { return blackColor; }
};
