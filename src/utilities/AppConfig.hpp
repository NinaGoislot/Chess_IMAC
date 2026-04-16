#pragma once
#include <string>

// Stores resolved directories for 3D assets
struct AssetPaths {
    // Directory containing chess piece model files
    std::string models;
    // Root textures directory
    std::string textures;
    // Directory containing skybox face images
    std::string skybox;
    // Directory containing board textures
    std::string board;
};

// Stores runtime paths used to load assets and shaders
struct AppConfig {
    // Absolute or resolved path to the assets directory
    std::string assetRoot;
    // Absolute or resolved path to the shaders directory
    std::string shaderPath;

    std::string texture(const std::string& name) const { return assetRoot + "/textures/" + name; }
    std::string models() const { return assetRoot + "/models"; }
    std::string textures() const { return assetRoot + "/textures"; }
    std::string skybox() const { return texture("skybox/day"); }
};