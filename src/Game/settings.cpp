#include "settings.hpp"
#include <imgui.h>

void settings::display()
{
    ImGui::Begin("Settings");

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{8.f, 9.f});

    ImGui::TextUnformatted("2D");
    ImGui::Separator();

    ImGui::ColorEdit4("White Tile Color", (float*)&whiteColor);
    ImGui::ColorEdit4("Black Tile Color", (float*)&blackColor);

    const ImVec4 boardGapPreview{
        (whiteColor.x + blackColor.x) * 0.5f,
        (whiteColor.y + blackColor.y) * 0.5f,
        (whiteColor.z + blackColor.z) * 0.5f,
        1.f,
    };
    ImGui::ColorButton("##boardGapPreview", boardGapPreview, ImGuiColorEditFlags_NoTooltip, ImVec2{42.f, 16.f});
    ImGui::SameLine();
    ImGui::TextUnformatted("Board Color Between Tiles (auto)");

    ImGui::SliderFloat("Button Size", &buttonSize, 25.f, 100.f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextUnformatted("3D");
    ImGui::Separator();
    ImGui::Checkbox("Render Board In 3D", &use3D);

    if (use3D)
    {
        ImGui::Spacing();

        ImGui::TextUnformatted("Scene");
        ImGui::Separator();
        ImGui::Checkbox("Draw Pieces In 3D", &drawPieces3D);
        ImGui::Checkbox("Draw Skybox", &drawSkybox);

        if (drawSkybox)
        {
            ImGui::ColorEdit3("Skybox Top Color", (float*)&skyboxTopColor);
            ImGui::ColorEdit3("Skybox Bottom Color", (float*)&skyboxBottomColor);
        }

        ImGui::Spacing();

        ImGui::TextUnformatted("Board Geometry");
        ImGui::Separator();

        ImGui::SliderFloat("Board Thickness", &boardThickness, 0.05f, 0.4f);
        ImGui::ColorEdit3("Board Side Color", (float*)&boardSideColor);
        ImGui::SliderFloat("Board Side Width", &boardSideThickness, 0.05f, 0.8f);
        ImGui::SliderFloat("Board Side Depth", &boardSideDrop, 0.f, 0.5f);

        ImGui::Spacing();

        ImGui::TextUnformatted("Camera");
        ImGui::Separator();

        ImGui::Checkbox("Use Camera Piece", &cameraPieceTarget);
        ImGui::SliderFloat("Camera Yaw", &cameraYawDegrees, -180.f, 180.f);
        ImGui::SliderFloat("Camera Pitch", &cameraPitchDegrees, 10.f, 80.f);
        ImGui::SliderFloat("Camera Distance", &cameraDistance, 6.f, 25.f);
    }

    ImGui::PopStyleVar();
    
    ImGui::End();
};
