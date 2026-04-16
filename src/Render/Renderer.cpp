#include "Renderer.hpp"
#include <imgui.h>
#include <algorithm>

namespace {
bool hasTile(const std::vector<Vector2D>& tiles, int x, int y)
{
    return std::any_of(
        tiles.begin(),
        tiles.end(),
        [&](const Vector2D& tile) {
            return static_cast<int>(tile.getX()) == x && static_cast<int>(tile.getY()) == y;
        }
    );
}
} // namespace

Renderer::Renderer(TextureManager& textures)
    : _textures(textures)
{
}

void Renderer::initialize(const AppConfig& config)
{
    _scene3D.initialize(config);
}

/**
 *
 * Dessine la vue 3D dans la fenetre courante et renvoie la tuile cliquee si applicable
 * @param board : etat courant du plateau.
 * @param gameSettings : reglages de rendu 3D.
 * @param currentTurn : couleur du joueur actif.
 * @param deltaTimeSeconds : temps ecoule pour les animations.
 * @param kirbyPosition : position optionnelle de Kirby.
 * @param selection : etat de selection a afficher.
 * @return Tuile cliquee si une interaction a eu lieu.
 */
std::optional<BoardClick> Renderer::draw3DView(const Board& board, const settings& gameSettings, PieceColor currentTurn, float deltaTimeSeconds, std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection)
{
    _hoveredTile.reset();

    if (!gameSettings.use3D)
    {
        ImGui::TextDisabled("Vue 3D desactivee dans Settings.");
        return std::optional<BoardClick>{};
    }

    return draw3DBoard(board, gameSettings, currentTurn, deltaTimeSeconds, kirbyPosition, selection);
}

/**
 *
 * Dessine la vue 2D dans la fenetre courante et renvoie la tuile cliquee si applicable.
 * @param board : etat courant du plateau.
 * @param gameSettings : reglages de rendu 2D.
 * @param kirbyPosition : position optionnelle de Kirby.
 * @param selection : etat de selection a afficher.
 * @return Tuile cliquee si une interaction a eu lieu.
 */
std::optional<BoardClick> Renderer::draw2DView(const Board& board, const settings& gameSettings, std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection)
{
    _hoveredTile.reset();
    return draw2DBoard(board, gameSettings, kirbyPosition, selection);
}

/**
 *
 * Rend le plateau via le pipeline 3D et gère la detection de clic/survol.
 * @param board : etat courant du plateau.
 * @param gameSettings : reglages de rendu 3D.
 * @param currentTurn : couleur du joueur actif.
 * @param deltaTimeSeconds : temps ecoule pour les animations.
 * @param kirbyPosition : position optionnelle de Kirby.
 * @param selection : etat de selection a afficher.
 * @return Tuile cliquee si une interaction a eu lieu.
 */
std::optional<BoardClick> Renderer::draw3DBoard(const Board& board, const settings& gameSettings, PieceColor currentTurn, float deltaTimeSeconds, std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection)
{
    ImVec2 available = ImGui::GetContentRegionAvail();
    available.x      = std::max(available.x, 64.f);
    available.y      = std::max(available.y, 64.f);

    _scene3D.render(board, gameSettings, currentTurn, static_cast<int>(available.x), static_cast<int>(available.y), deltaTimeSeconds, kirbyPosition, selection);

    ImTextureID texture = _scene3D.getColorTexture();
    if (texture == nullptr)
    {
        ImGui::TextDisabled("Vue 3D indisponible.");
        return std::optional<BoardClick>{};
    }

    const ImVec2 imageStart = ImGui::GetCursorScreenPos();
    ImGui::Image(texture, available, ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));

    std::optional<BoardClick> clickedCase;
    const bool                isHovered = ImGui::IsItemHovered();
    if (isHovered)
    {
        const ImVec2 mousePosition = ImGui::GetIO().MousePos;
        const float  localX        = mousePosition.x - imageStart.x;
        const float  localY        = mousePosition.y - imageStart.y;

        int tileX = 0;
        int tileY = 0;
        if (_scene3D.pickBoardTile(gameSettings, localX, localY, available.x, available.y, &tileX, &tileY))
        {
            _hoveredTile = BoardClick{tileX, tileY};

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                clickedCase = BoardClick{tileX, tileY};
        }
    }

    return clickedCase;
}

/**
 *
 * Rend le plateau via le pipeline 2D et gère la detection de clic.
 * @param board : etat courant du plateau.
 * @param gameSettings : reglages de rendu 2D.
 * @param kirbyPosition : position optionnelle de Kirby.
 * @param selection : etat de selection a afficher.
 * @return Tuile cliquee si une interaction a eu lieu.
 */
std::optional<BoardClick> Renderer::draw2DBoard(const Board& board, const settings& gameSettings, std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection)
{
    std::optional<BoardClick> clickedCase;

    for (int y = 0; y < Board::SIZE; y++)
    {
        for (int x = 0; x < Board::SIZE; x++)
        {
            if (!clickedCase.has_value() && draw2DCase(board, gameSettings, x, y, kirbyPosition, selection))
                clickedCase = BoardClick{x, y};

            if (x < Board::SIZE - 1)
                ImGui::SameLine();
        }
    }

    return clickedCase;
}

/**
 *
 * Dessine une tuile 2D, applique les couleurs de selection et renvoie si elle a ete cliquee.
 * @param board : etat courant du plateau.
 * @param gameSettings : reglages de rendu 2D.
 * @param x : coordonnee X de la tuile.
 * @param y : coordonnee Y de la tuile.
 * @param kirbyPosition : position optionnelle de Kirby.
 * @param selection : etat de selection a afficher.
 * @return Vrai si la tuile a ete cliquee.
 */
bool Renderer::draw2DCase(const Board& board, const settings& gameSettings, int x, int y, std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection)
{
    ImGui::PushID(x + y * Board::SIZE);

    bool        white       = (x + y) % 2 == 0;
    const Case& currentCase = board.getCase(x, y);

    ImVec4     color         = white ? gameSettings.getWhite() : gameSettings.getBlack();
    const bool isHighlighted = hasTile(selection.highlighted, x, y);
    const bool isSelected    = selection.selected.has_value()
                            && static_cast<int>(selection.selected->getX()) == x
                            && static_cast<int>(selection.selected->getY()) == y;

    if (isHighlighted || isSelected)
    {
        const ImVec4 selectedOwnPieceColor{0.20f, 0.45f, 1.f, 1.f};
        const ImVec4 availableMoveColor{0.20f, 0.75f, 0.25f, 1.f};
        const ImVec4 captureMoveColor{1.f, 0.55f, 0.f, 1.f};

        if (isSelected)
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

    if (ImGui::IsItemHovered())
        _hoveredTile = BoardClick{x, y};

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
