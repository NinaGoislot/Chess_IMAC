#include <imgui.h>
#include <optional>
#include "Managers/Game.hpp"
#include "Managers/InputManager.hpp"
#include "Managers/SceneManager.hpp"
#include "quick_imgui/quick_imgui.hpp"

int main()
{
    std::optional<InputManager> inputManager;

    // pieecest.stCallback = [&](Piece const& piece) {
    //     std::cout << "A piece has been eaten: " << piece._name << "\n";
    // };
    SceneManager& sceneManager = SceneManager::instance();

    quick_imgui::loop(
        "Chess",
        {
            .init                     = [&]() {
                sceneManager.init();
                inputManager.emplace(Game::instance().getSettings());
            },
            .loop                     = [&]() {
                if (inputManager.has_value())
                    inputManager->update();
                sceneManager.renderCurrentScene();
            },
            .mouse_button_callback    = [&](int button, int action, int mods) {
                if (inputManager.has_value())
                    inputManager->onMouseButton(button, action, mods);
            },
            .cursor_position_callback = [&](double xpos, double ypos) {
                if (inputManager.has_value())
                    inputManager->onCursorPosition(xpos, ypos);
            },
            .scroll_callback          = [&](double xoffset, double yoffset) {
                if (inputManager.has_value())
                    inputManager->onScroll(xoffset, yoffset);
            },
        },
        [&]() { return sceneManager.shouldQuit(); }
    );
}