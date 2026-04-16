#include "UI/GameUiComponents.hpp"
#include "utilities/PieceColorUtils.hpp"
#include "utilities/UiTheme.hpp"

namespace {

bool drawButtonWithColors(const char* label, const ImVec2& size, const ImVec4& base, const ImVec4& hovered, const ImVec4& active)
{
    ImGui::PushStyleColor(ImGuiCol_Button, base);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, active);

    const bool clicked = ImGui::Button(label, size);

    ImGui::PopStyleColor(3);
    return clicked;
}

} // namespace

namespace GameUiComponents {

bool drawPrimaryButton(const char* label, const ImVec2& size)
{
    return drawButtonWithColors(
        label,
        size,
        UiTheme::primaryButton.toImVec4(),
        UiTheme::primaryButtonHovered.toImVec4(),
        UiTheme::primaryButtonActive.toImVec4()
    );
}

bool drawSecondaryButton(const char* label, const ImVec2& size)
{
    return drawButtonWithColors(
        label,
        size,
        UiTheme::secondaryButton.toImVec4(),
        UiTheme::secondaryButtonHovered.toImVec4(),
        UiTheme::secondaryButtonActive.toImVec4()
    );
}

bool drawDangerButton(const char* label, const ImVec2& size)
{
    return drawButtonWithColors(
        label,
        size,
        UiTheme::dangerButton.toImVec4(),
        UiTheme::dangerButtonHovered.toImVec4(),
        UiTheme::dangerButtonActive.toImVec4()
    );
}

void drawMenuHero(const MenuHeroText& heroText)
{
    ImGui::TextColored(UiTheme::menuHighlight.toImVec4(), "%s", heroText.title);
    ImGui::TextColored(UiTheme::mutedText.toImVec4(), "%s", heroText.subtitle);
}

void drawTurnStatusCard(int turnNumber, const std::string& activePlayerName, PieceColor activeColor)
{
    // ImGui::TextColored(UiTheme::panelHeader.toImVec4(), "Statut de partie");
    // ImGui::Separator();

    ImGui::Text("Tour actuel: #%d", turnNumber);

    const ImVec4 colorPill = (activeColor == PieceColor::White) ? UiTheme::turnWhite.toImVec4() : UiTheme::turnBlack.toImVec4();
    ImGui::ColorButton("##activeTurnColor", colorPill, ImGuiColorEditFlags_NoTooltip, ImVec2{16.f, 16.f});
    ImGui::SameLine();
    ImGui::Text("Joueur actif: %s (%s)", activePlayerName.c_str(), PieceColorUtils::toFrenchLabel(activeColor));

    ImGui::Separator();
}

void drawWinnerBanner(const Player* winner)
{
    const char* winnerName = (winner != nullptr && !winner->getName().empty()) ? winner->getName().c_str() : "joueur inconnu";
    ImGui::TextColored(UiTheme::winner.toImVec4(), "Partie terminee ! Victoire %s", winnerName);
}

void drawMoveHistoryList(const std::vector<std::string>& history)
{
    ImGui::BeginChild("Scrolling");

    for (const std::string& move : history)
    {
        ImGui::Text("%s", move.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
}

} // namespace GameUiComponents
