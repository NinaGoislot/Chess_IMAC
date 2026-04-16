#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include "Model/Match/Player.hpp"
#include "Model/PieceFactory/Piece.hpp"


namespace GameUiComponents {

struct MenuHeroText {
    const char* title;
    const char* subtitle;
};

bool drawPrimaryButton(const char* label, const ImVec2& size);
bool drawSecondaryButton(const char* label, const ImVec2& size);
bool drawDangerButton(const char* label, const ImVec2& size);

/**
 * Draws the menu title block with a strong title and a muted subtitle.
 * @param heroText: title/subtitle bundle for the menu hero section.
 * @return Nothing.
 */
void drawMenuHero(const MenuHeroText& heroText);

/**
 * Draws a compact status card for the currently active turn.
 * @param turnNumber: 1-based turn number currently being played.
 * @param activePlayerName: display name of the player who must play now.
 * @param activeColor: active side color (white or black).
 * @return Nothing.
 */
void drawTurnStatusCard(int turnNumber, const std::string& activePlayerName, PieceColor activeColor);

/**
 * Draws a prominent winner banner when the match is over.
 * @param winner: optional winner pointer, can be nullptr when missing.
 * @return Nothing.
 */
void drawWinnerBanner(const Player* winner);

/**
 * Draws move history rows with auto-scroll behavior.
 * @param history: chronological move labels to display.
 * @return Nothing.
 */
void drawMoveHistoryList(const std::vector<std::string>& history);

} // namespace GameUiComponents
