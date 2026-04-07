#include <imgui.h>
#include "Managers/SceneManager.hpp"
#include "quick_imgui/quick_imgui.hpp"

int main()
{
    SceneManager& sceneManager = SceneManager::instance();

    quick_imgui::loop(
        "Chess",
        {
            .init = [&]() { sceneManager.init(); },
            .loop =
                [&]() {
                    sceneManager.renderCurrentScene();
                },
        }, [&]() { return sceneManager.shouldQuit(); }
    );
}