#pragma once
#include <string>

// Stores runtime paths used to load assets and shaders.
struct AppConfig {
    // Absolute or resolved path to the assets directory.
    std::string assetPath;
    // Absolute or resolved path to the shaders directory.
    std::string shaderPath;
};