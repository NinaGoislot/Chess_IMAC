#include "GLRenderer.hpp"

#include <glad/glad.h>

#include <algorithm>
#include <iostream>

#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Primitives.hpp"
#include "ResourceManager.hpp"

namespace {

constexpr int CUBE_VERTEX_COUNT = 36;

} // namespace

namespace Render3D {

GLRenderer::~GLRenderer()
{
    destroy();
}

GLRenderer::UniformLocations GLRenderer::queryUniformLocations(const Shader& shader)
{
    UniformLocations locations;
    locations.mvp      = shader.getUniform("uMVP");
    locations.model    = shader.getUniform("uModel");
    locations.color    = shader.getUniform("uColor");
    locations.topLightDir = shader.getUniform("uTopLightDirection");
    locations.topLightColor = shader.getUniform("uTopLightColor");
    locations.topLightStrength = shader.getUniform("uTopLightStrength");
    locations.sideLightDir = shader.getUniform("uSideLightDirection");
    locations.sideLightColor = shader.getUniform("uSideLightColor");
    locations.sideLightStrength = shader.getUniform("uSideLightStrength");
    locations.ambient  = shader.getUniform("uAmbientStrength");
    locations.textureSampler = shader.getUniform("uTexture");
    locations.useTexture     = shader.getUniform("uUseTexture");
    locations.textureScale   = shader.getUniform("uTextureScale");
    return locations;
}

GLRenderer::ExplosionUniformLocations GLRenderer::queryExplosionUniformLocations(const Shader& shader)
{
    ExplosionUniformLocations locations;
    locations.mvp      = shader.getUniform("uMVP");
    locations.model    = shader.getUniform("uModel");
    locations.color    = shader.getUniform("uColor");
    locations.lightDir = shader.getUniform("uLightDirection");
    locations.ambient  = shader.getUniform("uAmbientStrength");
    locations.progress = shader.getUniform("uExplosionProgress");
    return locations;
}

void GLRenderer::initializeCubeGeometry()
{
    const auto& vertices = Primitives::cubeVertices();

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

    glBindVertexArray(0);
}

bool GLRenderer::ensureFramebufferSize(int width, int height)
{
    if (_fbo != 0 && _framebufferW == width && _framebufferH == height)
        return true;

    destroyFramebuffer();
    _framebufferW = width;
    _framebufferH = height;

    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glGenTextures(1, &_colorTexture);
    glBindTexture(GL_TEXTURE_2D, _colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _framebufferW, _framebufferH, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTexture, 0);

    glGenRenderbuffers(1, &_depthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _framebufferW, _framebufferH);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthStencil);

    const bool framebufferReady = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (!framebufferReady)
    {
        std::cout << "3D board framebuffer is not complete.\n";
        destroyFramebuffer();
        return false;
    }

    return true;
}

void GLRenderer::destroyFramebuffer()
{
    if (_depthStencil != 0)
    {
        glDeleteRenderbuffers(1, &_depthStencil);
        _depthStencil = 0;
    }

    if (_colorTexture != 0)
    {
        glDeleteTextures(1, &_colorTexture);
        _colorTexture = 0;
    }

    if (_fbo != 0)
    {
        glDeleteFramebuffers(1, &_fbo);
        _fbo = 0;
    }

    _framebufferW = 0;
    _framebufferH = 0;
}

bool GLRenderer::initialize(const std::string& shaderDir)
{
    if (_initialized)
        return true;

    const bool boardLoaded = _boardShader.load(shaderDir + "/board.vs.glsl", shaderDir + "/board.fs.glsl");
    if (!boardLoaded)
    {
        return false;
    }

    _boardUniforms = queryUniformLocations(_boardShader);
    if (!_boardUniforms.isValid())
    {
        return false;
    }

    const bool pieceExplosionLoaded = _pieceExplosionShader.load(shaderDir + "/piece_explosion.vs.glsl", shaderDir + "/piece_explosion.fs.glsl");
    if (pieceExplosionLoaded)
    {
        _pieceExplosionUniforms = queryExplosionUniformLocations(_pieceExplosionShader);
        _pieceExplosionReady    = _pieceExplosionUniforms.isValid();
    }

    initializeCubeGeometry();

    const bool skyboxShaderLoaded = _skyboxShader.load(shaderDir + "/skybox.vs.glsl", shaderDir + "/skybox.fs.glsl");
    if (skyboxShaderLoaded)
    {
        _skyboxUniforms.vp          = _skyboxShader.getUniform("uVP");
        _skyboxUniforms.topColor    = _skyboxShader.getUniform("uTopColor");
        _skyboxUniforms.bottomColor = _skyboxShader.getUniform("uBottomColor");
        _skyboxUniforms.cubemap     = _skyboxShader.getUniform("uSkybox");
        _skyboxUniforms.useCubemap  = _skyboxShader.getUniform("uUseCubemap");
        _skyboxReady                = _skyboxUniforms.isValid();
    }

    _initialized = true;
    return true;
}

void GLRenderer::destroy()
{
    destroyFramebuffer();

    glDeleteBuffers(1, &_vbo);
    _vbo = 0;

    glDeleteVertexArrays(1, &_vao);
    _vao = 0;

    _boardUniforms  = UniformLocations{};
    _pieceExplosionUniforms = ExplosionUniformLocations{};
    _skyboxUniforms = SkyboxUniformLocations{};
    _pieceExplosionReady = false;
    _skyboxReady    = false;
    _initialized    = false;
}

bool GLRenderer::beginFrame(int width, int height)
{
    if (width <= 0 || height <= 0)
        return false;
    if (!_initialized)
        return false;
    if (!ensureFramebufferSize(width, height))
        return false;

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glViewport(0, 0, _framebufferW, _framebufferH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(0.08f, 0.08f, 0.10f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return true;
}

void GLRenderer::endFrame()
{
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int GLRenderer::colorTextureId() const
{
    return _colorTexture;
}

int GLRenderer::framebufferWidth() const
{
    return _framebufferW;
}

int GLRenderer::framebufferHeight() const
{
    return _framebufferH;
}

bool GLRenderer::beginBoardPass() const
{
    if (!_initialized || !_boardUniforms.isValid())
        return false;

    _boardShader.use();
    glUniform1i(_boardUniforms.textureSampler, 0);
    glUniform1i(_boardUniforms.useTexture, 0);
    glUniform1f(_boardUniforms.textureScale, 1.f);
    return true;
}

void GLRenderer::setBoardLighting(const BoardLighting& lighting) const
{
    if (!_boardUniforms.isValid())
        return;

    glUniform3f(_boardUniforms.topLightDir, lighting.topLightDirection.x, lighting.topLightDirection.y, lighting.topLightDirection.z);
    glUniform3f(_boardUniforms.topLightColor, lighting.topLightColor.x, lighting.topLightColor.y, lighting.topLightColor.z);
    glUniform1f(_boardUniforms.topLightStrength, lighting.topLightStrength);

    glUniform3f(_boardUniforms.sideLightDir, lighting.sideLightDirection.x, lighting.sideLightDirection.y, lighting.sideLightDirection.z);
    glUniform3f(_boardUniforms.sideLightColor, lighting.sideLightColor.x, lighting.sideLightColor.y, lighting.sideLightColor.z);
    glUniform1f(_boardUniforms.sideLightStrength, lighting.sideLightStrength);

    glUniform1f(_boardUniforms.ambient, lighting.ambientStrength);
}

bool GLRenderer::beginExplosionPass(const ExplosionPassSettings& settings) const
{
    if (!_pieceExplosionReady || !_pieceExplosionUniforms.isValid())
        return false;

    _pieceExplosionShader.use();
    glUniform3f(_pieceExplosionUniforms.lightDir, settings.lightDirection.x, settings.lightDirection.y, settings.lightDirection.z);
    glUniform1f(_pieceExplosionUniforms.ambient, settings.ambientStrength);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    return true;
}

void GLRenderer::endExplosionPass() const
{
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void GLRenderer::setMaterial(const Material& material) const
{
    if (!_boardUniforms.isValid())
        return;

    glUniform3f(_boardUniforms.color, material.color.x, material.color.y, material.color.z);

    const bool useTexture = material.hasTexture();
    glUniform1i(_boardUniforms.useTexture, useTexture ? 1 : 0);
    glUniform1f(_boardUniforms.textureScale, material.textureScale);

    if (useTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.textureId);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void GLRenderer::drawGeometry(const glm::mat4& viewProjection, const glm::mat4& model, unsigned int vao, int drawCount, bool indexed) const
{
    if (!_boardUniforms.isValid() || vao == 0 || drawCount <= 0)
        return;

    const glm::mat4 mvp = viewProjection * model;
    glUniformMatrix4fv(_boardUniforms.mvp, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(_boardUniforms.model, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(vao);
    if (indexed)
        glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(GL_TRIANGLES, 0, drawCount);
}

void GLRenderer::drawExplosionGeometry(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material,
                                       float progress, unsigned int vao, int drawCount, bool indexed) const
{
    if (!_pieceExplosionUniforms.isValid() || vao == 0 || drawCount <= 0)
        return;

    const glm::mat4 mvp = viewProjection * model;
    glUniformMatrix4fv(_pieceExplosionUniforms.mvp, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(_pieceExplosionUniforms.model, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(_pieceExplosionUniforms.color, material.color.x, material.color.y, material.color.z);
    glUniform1f(_pieceExplosionUniforms.progress, std::clamp(progress, 0.f, 1.f));

    glBindVertexArray(vao);
    if (indexed)
        glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(GL_TRIANGLES, 0, drawCount);
}

void GLRenderer::drawCube(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material) const
{
    if (_vao == 0)
        return;

    setMaterial(material);
    drawGeometry(viewProjection, model, _vao, CUBE_VERTEX_COUNT, false);
}

void GLRenderer::drawIndexedMesh(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material, unsigned int vao, int indexCount) const
{
    setMaterial(material);
    drawGeometry(viewProjection, model, vao, indexCount, true);
}

void GLRenderer::drawMesh(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material, unsigned int vao, int indexCount) const
{
    drawIndexedMesh(viewProjection, model, material, vao, indexCount);
}

void GLRenderer::drawExplosionCube(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material, float progress) const
{
    if (_vao == 0)
        return;

    drawExplosionGeometry(viewProjection, model, material, progress, _vao, CUBE_VERTEX_COUNT, false);
}

void GLRenderer::drawExplosionIndexedMesh(const glm::mat4& viewProjection, const glm::mat4& model, const Material& material, unsigned int vao, int indexCount, float progress) const
{
    drawExplosionGeometry(viewProjection, model, material, progress, vao, indexCount, true);
}

void GLRenderer::drawSkybox(const glm::mat4& view, const glm::mat4& projection, const settings& gameSettings, const ResourceManager& resourceManager) const
{
    if (!_skyboxReady || !_skyboxUniforms.isValid() || _vao == 0)
        return;

    const glm::vec3 topColor    = gameSettings.getSkyboxTopColorVec3();
    const glm::vec3 bottomColor = gameSettings.getSkyboxBottomColorVec3();
    const unsigned int cubemapTexture = resourceManager.getSkyboxCubemap();
    const bool hasCubemapTexture = cubemapTexture != 0;

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    const glm::mat4 skyboxView = glm::mat4{glm::mat3{view}};
    const glm::mat4 skyboxVp   = projection * skyboxView;

    _skyboxShader.use();
    glUniformMatrix4fv(_skyboxUniforms.vp, 1, GL_FALSE, glm::value_ptr(skyboxVp));
    glUniform3f(_skyboxUniforms.topColor, topColor.x, topColor.y, topColor.z);
    glUniform3f(_skyboxUniforms.bottomColor, bottomColor.x, bottomColor.y, bottomColor.z);
    glUniform1i(_skyboxUniforms.cubemap, 0);
    glUniform1i(_skyboxUniforms.useCubemap, hasCubemapTexture ? 1 : 0);

    if (hasCubemapTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    }

    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, CUBE_VERTEX_COUNT);

    glBindVertexArray(0);

    if (hasCubemapTexture)
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

} // namespace Render3D