#include <imgui.h>
#include <filesystem>
#include "Managers/InputManager.hpp"
#include "Managers/SceneManager.hpp"
#include "quick_imgui/quick_imgui.hpp"
#include "utilities/AppConfig.hpp"

int main(int argc, char** argv)
{
    std::filesystem::path exePath = std::filesystem::absolute(argv[0]);
    std::string           baseDir = exePath.parent_path().string();

    AppConfig config;
    config.assetRoot  = baseDir + "/assets";
    config.shaderPath = baseDir + "/shaders";

    if (!std::filesystem::exists(config.assetRoot) || !std::filesystem::exists(config.shaderPath))
    {
        config.assetRoot  = baseDir + "/../assets";
        config.shaderPath = baseDir + "/../shaders";
    }

    SceneManager& sceneManager = SceneManager::instance();
    bool          initialized  = false;

    quick_imgui::loop(
        "Chess",
        {
            .init                     = [&]() {
                sceneManager.init(config);
                InputManager::instance().bindSettings(sceneManager.getGame().getSettings());
                initialized = true; },
            .loop                     = [&]() { sceneManager.renderCurrentScene(); },
            .cursor_position_callback = [&](double xpos, double ypos) {
                if (!initialized)
                    return;

                InputManager::instance().onCursorPosition(xpos, ypos); },
            .scroll_callback          = [&](double xoffset, double yoffset) {
                if (!initialized)
                    return;

                InputManager::instance().onScroll(xoffset, yoffset); },
        },
        [&]() { return sceneManager.getShouldQuit(); }
    );

    return 0;
}
