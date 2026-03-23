#pragma once

#include <imgui.h>
#include <glm/glm.hpp>
#include "Board/Board.hpp"
#include "Game/settings.hpp"
#include "3D/Shader.hpp"


class Board3DRenderer
{
public:
    // constructor and destructor
    Board3DRenderer() = default;
    ~Board3DRenderer();

    // non-copyable
    Board3DRenderer(const Board3DRenderer&)            = delete;
    Board3DRenderer& operator=(const Board3DRenderer&) = delete;

    // main render function and texture
    void render(const Board& board, const settings& gameSettings, int width, int height);
    ImTextureID colorTexture() const;

    

private:
    // initialization and cleanup functions
    void initializeIfNeeded();
    void ensureFramebufferSize(int width, int height);
    void destroyFramebuffer();
    void destroyGlResources();

    // OpenGL resources
    unsigned int _vao           = 0;
    unsigned int _vbo           = 0;
    unsigned int _fbo           = 0;
    unsigned int _colorTexture  = 0;
    unsigned int _depthStencil  = 0;
    int          _framebufferW  = 0;
    int          _framebufferH  = 0;
    bool         _initialized   = false;

    // shader and uniform locations
    Shader _shader;
    int _mvpLoc      = -1;
    int _modelLoc    = -1;
    int _colorLoc    = -1;
    int _lightDirLoc = -1;
    int _ambientLoc  = -1;

    // rendering helper functions
    bool prepareRenderState(int width, int height);
    glm::mat4 calculateCameraViewProjection(const settings& gameSettings, float aspect) const;
    void setupStaticLighting() const;
    void drawBoardTiles(const glm::mat4& viewProjection, const settings& gameSettings) const;
    void drawPieces(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings) const;
};
