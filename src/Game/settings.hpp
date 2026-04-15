#pragma once
#include <imgui.h>
#include <glm/vec3.hpp>

// Runtime settings
struct settings {
    // 2D rendering settings
    ImVec4 whiteColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 blackColor = ImVec4(0.10f, 0.15f, 0.20f, 1.00f);
    ImVec4 _highlight{0.2f, 0.8f, 0.2f, 1.f};

    float buttonSize = 50.f;

    // 3D rendering settings
    bool use3D = true;
    bool drawPieces3D = true;
    bool drawSkybox = true;
    bool animatePieces = true;
    float pieceMoveDuration = 0.5f;
    bool cameraPieceTarget = false;

    ImVec4 skyboxTopColor    = ImVec4(0.20f, 0.35f, 0.62f, 1.00f);
    ImVec4 skyboxBottomColor = ImVec4(0.85f, 0.90f, 0.98f, 1.00f);

    float  boardThickness     = 0.18f;
    ImVec4 boardSideColor     = ImVec4(0.28f, 0.18f, 0.10f, 1.00f);
    float  boardSideThickness = 0.30f;
    float  boardSideDrop      = 0.14f;
    float  cameraYawDegrees   = 45.f;
    float  cameraPitchDegrees = 32.f;
    float  cameraDistance     = 13.5f;

    // Getters
    ImVec4 getWhite() const { return whiteColor; }
    ImVec4 getBlack() const { return blackColor; }
    ImVec4 getHighlight() const { return _highlight; }

    glm::vec3 getWhiteVec3() const { return glm::vec3{whiteColor.x, whiteColor.y, whiteColor.z}; }
    glm::vec3 getBlackVec3() const { return glm::vec3{blackColor.x, blackColor.y, blackColor.z}; }
    glm::vec3 getBoardGapColorVec3() const
    {
        return glm::vec3{
            (whiteColor.x + blackColor.x) * 0.5f,
            (whiteColor.y + blackColor.y) * 0.5f,
            (whiteColor.z + blackColor.z) * 0.5f,
        };
    }
    glm::vec3 getBoardSideColorVec3() const { return glm::vec3{boardSideColor.x, boardSideColor.y, boardSideColor.z}; }
    glm::vec3 getSkyboxTopColorVec3() const { return glm::vec3{skyboxTopColor.x, skyboxTopColor.y, skyboxTopColor.z}; }
    glm::vec3 getSkyboxBottomColorVec3() const { return glm::vec3{skyboxBottomColor.x, skyboxBottomColor.y, skyboxBottomColor.z}; }
};
