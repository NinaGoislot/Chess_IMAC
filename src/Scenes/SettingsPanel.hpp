#pragma once

#include "Model/settings.hpp"

// Draws the UI panel used to tweak runtime game/render settings.
namespace SettingsPanel {
// Render function: displays editable controls for game settings.
void draw(settings& gameSettings);
}
