#include <imgui.h>
#include <filesystem>

#include "utilities/AppConfig.hpp"
#include "Managers/SceneManager.hpp"
#include "quick_imgui/quick_imgui.hpp"

// Application entry point: resolves runtime paths, initializes scenes, and runs UI loop.
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
    SceneManager& sceneManager = SceneManager::instance();
    bool          initialized  = false;

    quick_imgui::loop(
        "Chess",
        {
            // Init callback: builds initial scene and input manager.
            .init = [&]() {
                sceneManager.init(config);
                initialized = true;
            },
            // Render callback: draws current active scene each frame.
            .loop = [&]() {
                // No more update() call needed here!
                sceneManager.renderCurrentScene();
            },
            // mouse_button_callback is entirely removed!
            // Cursor callback: updates camera orbit through input manager.
            .cursor_position_callback = [&](double xpos, double ypos) {
                if (!initialized)
                    return;

                sceneManager.getGame().onCursorPosition(xpos, ypos);
            },
            // Scroll callback: updates camera zoom through input manager.
            .scroll_callback = [&](double xoffset, double yoffset) {
                if (!initialized)
                    return;

                sceneManager.getGame().onScroll(xoffset, yoffset);
            },
        },
        [&]() { return sceneManager.getShouldQuit(); }
    );
    
    return 0;
}
