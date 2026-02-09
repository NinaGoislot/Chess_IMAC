#include "_settings.hpp"
#include <imgui.h>

void settings::display()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("White Color", (float*)&whiteColor);
    ImGui::ColorEdit4("Black Color", (float*)&blackColor);
    ImGui::SliderFloat("Button Size", &buttonSize, 25.f, 100.f);
    ImGui::End();
};
