#pragma once

#include <cstddef>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <string>

#include "Game/settings.hpp"
#include "Render/3D/Material.hpp"
#include "Render/3D/Shader.hpp"

namespace Render3D {

class ResourceManager;

// Lighting parameters for the board shader.
struct BoardLighting {
    glm::vec3 topLightDirection{0.f, 1.f, 0.f};
    glm::vec3 topLightColor{1.f, 1.f, 1.f};
    float     topLightStrength = 0.7f;
    glm::vec3 sideLightDirection{0.f, 1.f, 0.f};
    glm::vec3 sideLightColor{1.f, 1.f, 1.f};
    float     sideLightStrength = 0.5f;
    float     ambientStrength = 0.2f;
};

// Lighting parameters for the explosion shader.
struct ExplosionPassSettings {
    glm::vec3 lightDirection{0.f, 1.f, 0.f};
    float     ambientStrength = 0.25f;
};

// Owns low-level OpenGL rendering for board primitives and skybox.
class GLRenderer {
public:
    // Constructors
    GLRenderer() = default;
    ~GLRenderer();

    // Non-copyable
    GLRenderer(const GLRenderer&)            = delete;
    GLRenderer& operator=(const GLRenderer&) = delete;

    // Lifecycle
    bool initialize(const std::string& shaderDir);
    void destroy();

    // Framebuffer-backed frame helpers.
    bool beginFrame(int width, int height);
    void endFrame();
    unsigned int colorTextureId() const;
    int framebufferWidth() const;
    int framebufferHeight() const;

    // Render pipeline helpers
    bool beginBoardPass() const;
    void setBoardLighting(const BoardLighting& lighting) const;

    bool beginExplosionPass(const ExplosionPassSettings& settings) const;
    void endExplosionPass() const;

    // Draw utilities
    void setMaterial(const Material& material) const;
    void drawCube(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material) const;
    void drawIndexedMesh(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material, unsigned int vao, int indexCount) const;
    void drawIndexedMesh(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material, unsigned int vao, int indexCount, std::size_t indexOffset) const;
    void drawMesh(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material, unsigned int vao, int indexCount) const;

    void drawExplosionCube(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material, float progress) const;
    void drawExplosionIndexedMesh(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material, unsigned int vao, int indexCount, float progress) const;
    void drawExplosionIndexedMesh(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material, unsigned int vao,
                                  int indexCount, std::size_t indexOffset, float progress) const;

    void drawSkybox(const glm::mat4& view, const glm::mat4& projection, const settings& gameSettings, const ResourceManager& resourceManager) const;

private:
    // Cached uniform locations for board shader.
    struct UniformLocations {
        int mvp      = -1;
        int model    = -1;
        int color    = -1;
        int topLightDir = -1;
        int topLightColor = -1;
        int topLightStrength = -1;
        int sideLightDir = -1;
        int sideLightColor = -1;
        int sideLightStrength = -1;
        int ambient  = -1;
        int textureSampler = -1;
        int useTexture = -1;
        int textureScale = -1;
        int useMeshUv = -1;

        bool isValid() const
        {
            return mvp >= 0 && model >= 0 && color >= 0
                   && topLightDir >= 0 && topLightColor >= 0 && topLightStrength >= 0
                   && sideLightDir >= 0 && sideLightColor >= 0 && sideLightStrength >= 0
                   && ambient >= 0
                   && textureSampler >= 0 && useTexture >= 0 && textureScale >= 0
                   && useMeshUv >= 0;
        }
    };

    // Cached uniform locations for skybox shader.
    struct SkyboxUniformLocations {
        int vp          = -1;
        int topColor    = -1;
        int bottomColor = -1;
        int cubemap     = -1;
        int useCubemap  = -1;

        bool isValid() const
        {
            return vp >= 0
                   && topColor >= 0
                   && bottomColor >= 0
                   && cubemap >= 0
                   && useCubemap >= 0;
        }
    };

    // Cached uniform locations for explosion shader.
    struct ExplosionUniformLocations {
        int mvp      = -1;
        int model    = -1;
        int color    = -1;
        int lightDir = -1;
        int ambient  = -1;
        int progress = -1;

        bool isValid() const
        {
            return mvp >= 0 && model >= 0 && color >= 0 && lightDir >= 0 && ambient >= 0 && progress >= 0;
        }
    };

    // Internal helpers
    static UniformLocations queryUniformLocations(const Shader& shader);
    static ExplosionUniformLocations queryExplosionUniformLocations(const Shader& shader);
    void initializeCubeGeometry();
    bool ensureFramebufferSize(int width, int height);
    void destroyFramebuffer();

    void drawGeometry(const glm::mat4& viewProjection, const glm::mat4& model, unsigned int vao, int drawCount, bool indexed,
                      std::size_t indexOffsetBytes = 0u) const;
    void drawExplosionGeometry(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material,
                               float progress, unsigned int vao, int drawCount, bool indexed, std::size_t indexOffsetBytes = 0u) const;

    // Shared cube geometry state.
    unsigned int _vao         = 0;
    unsigned int _vbo         = 0;
    bool         _initialized = false;
    bool         _skyboxReady = false;

    // Off-screen framebuffer resources.
    unsigned int _fbo          = 0;
    unsigned int _colorTexture = 0;
    unsigned int _depthStencil = 0;
    int          _framebufferW = 0;
    int          _framebufferH = 0;

    // Shader programs used by render passes.
    Shader _boardShader;
    Shader _pieceExplosionShader;
    Shader _skyboxShader;

    UniformLocations          _boardUniforms{};
    ExplosionUniformLocations _pieceExplosionUniforms{};
    SkyboxUniformLocations    _skyboxUniforms{};
    bool                      _pieceExplosionReady = false;
};

} // namespace Render3D
