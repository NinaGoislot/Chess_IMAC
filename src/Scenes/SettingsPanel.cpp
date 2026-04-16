#include "Scenes/SettingsPanel.hpp"
#include <imgui.h>

namespace SettingsPanel {

/**
 *
 * Affiche les reglages graphiques et d'interaction avec sections repliables.
 * @param gameSettings : structure de settings modifiable.
 * @return Aucun.
 */
void draw(settings& gameSettings)
{
    ImGui::Begin("Settings");

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{8.f, 9.f});

    if (ImGui::CollapsingHeader("2D", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::ColorEdit4("White Tile Color", &gameSettings.whiteColor.x);
        ImGui::ColorEdit4("Black Tile Color", &gameSettings.blackColor.x);

        const ImVec4 boardGapPreview{
            (gameSettings.whiteColor.x + gameSettings.blackColor.x) * 0.5f,
            (gameSettings.whiteColor.y + gameSettings.blackColor.y) * 0.5f,
            (gameSettings.whiteColor.z + gameSettings.blackColor.z) * 0.5f,
            1.f,
        };
        ImGui::ColorButton("##boardGapPreview", boardGapPreview, ImGuiColorEditFlags_NoTooltip, ImVec2{42.f, 16.f});
        ImGui::SameLine();
        ImGui::TextUnformatted("Board Color Between Tiles (auto)");

        ImGui::SliderFloat("Button Size", &gameSettings.buttonSize, 25.f, 100.f);
    }

    if (ImGui::CollapsingHeader("3D", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Render Board In 3D", &gameSettings.use3D);

        if (gameSettings.use3D && ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Draw Pieces In 3D", &gameSettings.drawPieces3D);
            ImGui::Checkbox("Draw Skybox", &gameSettings.drawSkybox);

            if (gameSettings.drawSkybox)
            {
                ImGui::ColorEdit3("Skybox Top Color", &gameSettings.skyboxTopColor.x);
                ImGui::ColorEdit3("Skybox Bottom Color", &gameSettings.skyboxBottomColor.x);
            }
        }

        if (gameSettings.use3D && ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Animate Piece Movement", &gameSettings.animatePieces);
            if (gameSettings.animatePieces)
            {
                ImGui::SliderFloat("Move Animation Duration", &gameSettings.pieceMoveDuration, 0.05f, 0.6f, "%.2fs");
            }
        }

        if (gameSettings.use3D && ImGui::CollapsingHeader("Board Geometry", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderFloat("Board Thickness", &gameSettings.boardThickness, 0.05f, 0.4f);
            ImGui::ColorEdit3("Board Side Color", &gameSettings.boardSideColor.x);
            ImGui::SliderFloat("Board Side Width", &gameSettings.boardSideThickness, 0.05f, 0.8f);
            ImGui::SliderFloat("Board Side Depth", &gameSettings.boardSideDrop, 0.f, 0.5f);
        }

        if (gameSettings.use3D && ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Use Piece POV Camera", &gameSettings.cameraPieceTarget);

            if (gameSettings.cameraPieceTarget)
            {
                ImGui::SliderFloat("POV Yaw", &gameSettings.cameraYawDegrees, -180.f, 180.f);
                ImGui::SliderFloat("POV Pitch", &gameSettings.cameraPitchDegrees, -75.f, 75.f);
                ImGui::TextDisabled("Zoom is disabled in POV mode.");
            }
            else
            {
                ImGui::SliderFloat("Camera Yaw", &gameSettings.cameraYawDegrees, -180.f, 180.f);
                ImGui::SliderFloat("Camera Pitch", &gameSettings.cameraPitchDegrees, 10.f, 80.f);
                ImGui::SliderFloat("Camera Distance", &gameSettings.cameraDistance, 6.f, 25.f);
            }
        }
    }

    ImGui::PopStyleVar();

    ImGui::End();
}

} // namespace SettingsPanel
