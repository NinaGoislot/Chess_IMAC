#pragma once

#include <cstdint>
#include <string>
#include <vector>

// CPU-side texture pixels extracted from assets (e.g., GLB files).
struct TextureData {
    int width = 0;
    int height = 0;
    int channels = 0;
    std::vector<std::uint8_t> pixels{};
    std::string label{};

    bool isValid() const
    {
        return width > 0 && height > 0 && channels > 0 && !pixels.empty();
    }
};
