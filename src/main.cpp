#include <imgui.h>
#include <optional>
#include <probabimac/engine.hpp>
#include <probabimac/exponentielle.hpp>
#include <probabimac/weibull.hpp>
#include "Game/settings.hpp"
#include "Managers/Game.hpp"
#include "Managers/InputManager.hpp"
#include "quick_imgui/quick_imgui.hpp"


int main()
{
    float value{0.f};

    // game has to be optional because it needs to be initialized after ImGui is set up (textures need to be loaded in the right context)
    std::optional<Game> game;
    settings            gameSettings;

    // Variables aléatoires pour tester la loi de Weibull
    StochasticEngine moteurRandom;
    LoiWeibull       usurePion(1.5, 10.0);

    InputManager inputManager{gameSettings};

    // pieecest.stCallback = [&](Piece const& piece) {
    //     std::cout << "A piece has been eaten: " << piece._name << "\n";
    // };

    quick_imgui::loop(
        "Chess",
        {
            .init = [&]() {
                game.emplace();
                game->init();
            },
            .loop =
                [&]() {
                    inputManager.update();
                    // ImGui::SetNextWindowSize(ImVec2(540.f, 560.f), ImGuiCond_Always);
                    ImGui::Begin("Le jeu d'échec de fou");
                    if (game.has_value())
                        game->displayBoard(gameSettings, ImGui::GetIO().DeltaTime);
                    gameSettings.display();
                    ImGui::End();
                },
            .mouse_button_callback    = [&](int button, int action, int mods) { inputManager.onMouseButton(button, action, mods); },
            .cursor_position_callback = [&](double xpos, double ypos) { inputManager.onCursorPosition(xpos, ypos); },
            .scroll_callback          = [&](double xoffset, double yoffset) { inputManager.onScroll(xoffset, yoffset); },
        }
    );
}