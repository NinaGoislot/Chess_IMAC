#pragma once

#include <imgui.h>

namespace UiTheme {

struct Rgba {
    float r;
    float g;
    float b;
    float a;

    ImVec4 toImVec4() const { return ImVec4(r, g, b, a); }
};

inline constexpr Rgba primaryButton{0.12f, 0.36f, 0.66f, 1.0f};
inline constexpr Rgba primaryButtonHovered{0.18f, 0.47f, 0.82f, 1.0f};
inline constexpr Rgba primaryButtonActive{0.08f, 0.28f, 0.52f, 1.0f};

inline constexpr Rgba secondaryButton{0.16f, 0.23f, 0.34f, 1.0f};
inline constexpr Rgba secondaryButtonHovered{0.22f, 0.31f, 0.44f, 1.0f};
inline constexpr Rgba secondaryButtonActive{0.12f, 0.18f, 0.28f, 1.0f};

inline constexpr Rgba dangerButton{0.74f, 0.20f, 0.18f, 1.0f};
inline constexpr Rgba dangerButtonHovered{0.86f, 0.27f, 0.24f, 1.0f};
inline constexpr Rgba dangerButtonActive{0.60f, 0.14f, 0.12f, 1.0f};

inline constexpr Rgba panelHeader{0.95f, 0.97f, 1.00f, 1.0f};
inline constexpr Rgba panelSubtle{0.74f, 0.80f, 0.88f, 1.0f};
inline constexpr Rgba panelStrong{0.88f, 0.93f, 0.99f, 1.0f};

inline constexpr Rgba winner{0.12f, 0.70f, 0.26f, 1.0f};
inline constexpr Rgba turnWhite{0.84f, 0.86f, 0.90f, 1.0f};
inline constexpr Rgba turnBlack{0.18f, 0.20f, 0.24f, 1.0f};

inline constexpr Rgba menuAccent{0.26f, 0.58f, 0.95f, 1.0f};
inline constexpr Rgba menuHighlight{0.58f, 0.78f, 0.98f, 1.0f};
inline constexpr Rgba mutedText{0.68f, 0.74f, 0.82f, 1.0f};

} // namespace UiTheme
