#pragma once
#include <string>
#include <glad/glad.h>

// Small utility wrapper around OpenGL shader program compilation and use.
class Shader {
public:
    // Init function: loads, compiles, and links shader files.
    bool load(const std::string& vertexPath, const std::string& fragmentPath);
    // Binds shader program as current OpenGL program.
    void use() const;

    // Getters
    GLint getUniform(const std::string& name) const;

private:
    // OpenGL program id.
    GLuint _program = 0;

    // Reads full file content from disk.
    std::string loadFile(const std::string& path);
    // Compiles one shader stage and returns shader id.
    GLuint compile(GLenum type, const std::string& source);
};
