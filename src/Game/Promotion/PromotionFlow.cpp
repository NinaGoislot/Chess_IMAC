#include "Game/Promotion/PromotionFlow.hpp"
#include <imgui.h>
#include <array>
#include <string>

namespace {
constexpr std::array<PieceType, 4> PromotionChoices = {
    PieceType::Queen,
    PieceType::Bishop,
    PieceType::Rook,
    PieceType::Knight,
};
} // namespace

PromotionFlow::PromotionFlow(MatchState& matchState, const TextureManager& textures)
    : _match(&matchState)
    , _textures(&textures)
{
}

bool PromotionFlow::getHasPendingPromotion() const
{
    return _match != nullptr && _match->getHasPendingPromotion();
}

void PromotionFlow::drawPopup()
{
    if (_match == nullptr || _textures == nullptr)
        return;

    if (!getHasPendingPromotion())
        return;

    if (!ImGui::IsPopupOpen("Promotion du pion"))
    {
        ImGui::OpenPopup("Promotion du pion");
    }

    if (!ImGui::BeginPopupModal("Promotion du pion", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    const std::optional<MatchState::PendingPromotionInfo> pending = _match->getPendingPromotion();
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
    if (_match == nullptr)
        return false;

    return _match->choosePromotion(type);
}

void PromotionFlow::cancelChoice()
{
    if (_match == nullptr)
        return;

    _match->cancelPendingPromotion();
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

