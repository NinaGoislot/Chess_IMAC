#include "Game/Promotion/PromotionFlow.hpp"
#include <imgui.h>
#include <array>
#include <string>
#include <utility>

namespace {
constexpr std::array<PieceType, 4> PromotionChoices = {
    PieceType::Queen,
    PieceType::Bishop,
    PieceType::Rook,
    PieceType::Knight,
};
} // namespace

PromotionFlow::PromotionFlow(Board& board, std::array<Player, 2>& players, TurnManager& turnManager, const TextureManager& textures)
    : _board(&board)
    , _players(&players)
    , _turnManager(&turnManager)
    , _textures(&textures)
{
}

bool PromotionFlow::hasPendingPromotion() const
{
    return _board != nullptr && _board->hasPendingPromotion();
}

void PromotionFlow::setOnMoveValidated(std::function<void()> callback)
{
    _onMoveValidated = std::move(callback);
}

void PromotionFlow::drawPopup()
{
    if (_board == nullptr || _textures == nullptr)
        return;

    if (!hasPendingPromotion())
        return;

    if (!ImGui::IsPopupOpen("Promotion du pion"))
    {
        ImGui::OpenPopup("Promotion du pion");
    }

    if (!ImGui::BeginPopupModal("Promotion du pion", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    const std::optional<Board::PendingPromotionInfo> pending = _board->getPendingPromotion();
    if (!pending.has_value())
    {
        ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
        return;
    }

    ImGui::Text("Choisissez une piece pour la promotion :");
    ImGui::Spacing();

    for (std::size_t i = 0; i < PromotionChoices.size(); ++i)
    {
        const PieceType choice  = PromotionChoices[i];
        const char*     label   = pieceLabel(choice);
        ImTextureID     texture = _textures->getPieceTexture(pending->color, choice);

        ImGui::BeginGroup();

        const std::string buttonId = std::string("##promotion_") + std::to_string(static_cast<int>(i));
        if (ImGui::ImageButton(buttonId.c_str(), texture, ImVec2(56.f, 56.f)))
        {
            if (confirmChoice(choice))
            {
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::TextUnformatted(label);
        ImGui::EndGroup();

        if (i + 1 < PromotionChoices.size())
            ImGui::SameLine();
    }

    ImGui::Spacing();
    if (ImGui::Button("Annuler le coup"))
    {
        cancelChoice();
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

bool PromotionFlow::confirmChoice(PieceType type)
{
    if (_board == nullptr || _players == nullptr || _turnManager == nullptr)
        return false;

    const std::optional<Board::PendingPromotionInfo> pending = _board->getPendingPromotion();
    if (!pending.has_value())
        return false;

    Player& owner         = (pending->color == PieceColor::White) ? (*_players)[0] : (*_players)[1];
    Piece*  promotedPiece = owner.addPiece(type);
    if (promotedPiece == nullptr)
        return false;

    if (!_board->confirmPromotion(promotedPiece))
    {
        owner.removePiece(*promotedPiece);
        return false;
    }

    if (_onMoveValidated)
        _onMoveValidated();
    else
        _turnManager->nextTurn();

    return true;
}

void PromotionFlow::cancelChoice()
{
    if (_board == nullptr)
        return;

    _board->cancelPendingPromotion();
}

const char* PromotionFlow::pieceLabel(PieceType type) const
{
    switch (type)
    {
    case PieceType::Queen:
        return "Dame";
    case PieceType::Bishop:
        return "Fou";
    case PieceType::Rook:
        return "Tour";
    case PieceType::Knight:
        return "Cavalier";
    default:
        return "";
    }
}
