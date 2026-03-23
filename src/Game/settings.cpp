#include "settings.hpp"
#include <imgui.h>

void settings::display()
{
    ImGui::Begin("Settings");

    // 2D rendering settings
    ImGui::ColorEdit4("White Color", (float*)&whiteColor);
    ImGui::ColorEdit4("Black Color", (float*)&blackColor);
    ImGui::SliderFloat("Button Size", &buttonSize, 25.f, 100.f);

    ImGui::Separator();

    // 3D rendering settings
    ImGui::Checkbox("Render Board In 3D", &use3D);

    if (use3D)
    {
        ImGui::Checkbox("Draw Pieces In 3D", &drawPieces3D);
        ImGui::SliderFloat("Board Thickness", &boardThickness, 0.05f, 0.4f);
        ImGui::SliderFloat("Camera Yaw", &cameraYawDegrees, -180.f, 180.f);
        ImGui::SliderFloat("Camera Pitch", &cameraPitchDegrees, 10.f, 80.f);
        ImGui::SliderFloat("Camera Distance", &cameraDistance, 6.f, 25.f);
    }
    
    ImGui::End();
};
