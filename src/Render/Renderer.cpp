#include "Renderer.hpp"
#include <imgui.h>
#include <algorithm>


Renderer::Renderer(TextureManager& textures)
    : _textures(textures)
{
}

void Renderer::initialize(const AppConfig& config)
{
    _scene3D.initialize(config);
}

std::optional<BoardClick> Renderer::draw(const Board& board, const settings& gameSettings, PieceColor currentTurn, float deltaTimeSeconds, std::optional<std::pair<int, int>> kirbyPosition)
{
    if (gameSettings.use3D)
        return draw3DBoard(board, gameSettings, currentTurn, deltaTimeSeconds, kirbyPosition);

    return draw2DBoard(board, gameSettings, kirbyPosition);
}

std::optional<BoardClick> Renderer::draw3DBoard(const Board& board, const settings& gameSettings, PieceColor currentTurn, float deltaTimeSeconds, std::optional<std::pair<int, int>> kirbyPosition)
{
    ImVec2 available = ImGui::GetContentRegionAvail();
    available.x      = std::max(available.x, 64.f);
    available.y      = std::max(available.y, 64.f);

    _scene3D.render(board, gameSettings, currentTurn, static_cast<int>(available.x), static_cast<int>(available.y), deltaTimeSeconds, kirbyPosition);

    ImTextureID texture = _scene3D.getColorTexture();
    if (texture == nullptr)
    {
        return draw2DBoard(board, gameSettings, kirbyPosition);
    }

    const ImVec2 imageStart = ImGui::GetCursorScreenPos();
    ImGui::Image(texture, available, ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));

    std::optional<BoardClick> clickedCase;
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        const ImVec2 mousePosition = ImGui::GetIO().MousePos;
        const float  localX        = mousePosition.x - imageStart.x;
        const float  localY        = mousePosition.y - imageStart.y;

        int tileX = 0;
        int tileY = 0;
        if (_scene3D.pickBoardTile(gameSettings, localX, localY, available.x, available.y, &tileX, &tileY))
        {
            clickedCase = BoardClick{tileX, tileY};
        }
    }

    return clickedCase;
}

std::optional<BoardClick> Renderer::draw2DBoard(const Board& board, const settings& gameSettings, std::optional<std::pair<int, int>> kirbyPosition)
{
    std::optional<BoardClick> clickedCase;

    for (int y = 0; y < Board::SIZE; y++)
    {
        for (int x = 0; x < Board::SIZE; x++)
        {
            if (!clickedCase.has_value() && draw2DCase(board, gameSettings, x, y, kirbyPosition))
                clickedCase = BoardClick{x, y};

            if (x < Board::SIZE - 1)
                ImGui::SameLine();
        }
    }

    return clickedCase;
}

bool Renderer::draw2DCase(const Board& board, const settings& gameSettings, int x, int y, std::optional<std::pair<int, int>> kirbyPosition)
{
    ImGui::PushID(x + y * Board::SIZE);

    bool        white       = (x + y) % 2 == 0;
    const Case& currentCase = board.getCase(x, y);

    ImVec4 color = white ? gameSettings.getWhite() : gameSettings.getBlack();
    if (currentCase.getIsActive())
    {
        const ImVec4 selectedOwnPieceColor{0.20f, 0.45f, 1.f, 1.f};
        const ImVec4 availableMoveColor{0.20f, 0.75f, 0.25f, 1.f};
        const ImVec4 captureMoveColor{1.f, 0.55f, 0.f, 1.f};

        if (board.isSelectedCase(x, y))
        {
            color = selectedOwnPieceColor;
        }
        else if (!currentCase.getHasPiece())
        {
            color = availableMoveColor;
        }
        else
        {
            color = captureMoveColor;
        }
    }

    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

    bool   clicked = false;
    ImVec2 pos     = ImGui::GetCursorScreenPos();

    const bool  kirbyHere = kirbyPosition.has_value() && kirbyPosition->first == x && kirbyPosition->second == y;
    const char* label     = " ";

    if (kirbyHere)
    {
        const ImVec4 kirbyColor{0.96f, 0.48f, 0.82f, 1.0f};
        ImGui::PopStyleColor(3);
        ImGui::PushStyleColor(ImGuiCol_Button, kirbyColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, kirbyColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, kirbyColor);
    }

    if (ImGui::Button(label, ImVec2{gameSettings.buttonSize, gameSettings.buttonSize}))
    {
        clicked = true;
    }

    if (!kirbyHere && currentCase.getHasPiece())
    {
        Piece* piece = currentCase.getPiece();

        ImGui::SetCursorScreenPos(pos);
        piece->draw(gameSettings);
    }

    ImGui::PopStyleColor(3);
    ImGui::PopID();

    return clicked;
}
