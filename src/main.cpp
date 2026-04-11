#include <imgui.h>
#include <optional>
#include <filesystem>

#include "utilities/AppConfig.hpp"
#include "Managers/Game.hpp"
#include "Managers/InputManager.hpp"
#include "Managers/SceneManager.hpp"
#include "Render/Renderer.hpp"
#include "Render/TextureManager.hpp"
#include "quick_imgui/quick_imgui.hpp"

int main(int argc, char** argv)
{
    // -------- CONFIGURATION --------
    std::filesystem::path exePath = std::filesystem::absolute(argv[0]);
    std::string baseDir = exePath.parent_path().string();

    AppConfig config;
    config.assetPath = baseDir + "/assets";
    config.shaderPath = baseDir + "/shaders";

    if (!std::filesystem::exists(config.assetPath) || !std::filesystem::exists(config.shaderPath)) {
        config.assetPath = baseDir + "/../assets";
        config.shaderPath = baseDir + "/../shaders";
    }

    // -------- MAIN LOOP --------
    std::optional<InputManager> inputManager;
    SceneManager& sceneManager = SceneManager::instance();

    quick_imgui::loop(
        "Chess",
        {
            .init = [&]() {
                sceneManager.init(config);
                inputManager.emplace(Game::instance().getSettings());
            },
            .loop = [&]() {
                // No more update() call needed here!
                sceneManager.renderCurrentScene();
            },
            // mouse_button_callback is entirely removed!
            .cursor_position_callback = [&](double xpos, double ypos) {
                if (inputManager)
                    inputManager->onCursorPosition(xpos, ypos);
            },
            .scroll_callback = [&](double xoffset, double yoffset) {
                if (inputManager)
                    inputManager->onScroll(xoffset, yoffset);
            },
        },
        [&]() { return sceneManager.shouldQuit(); }
    );
    
    return 0;
}