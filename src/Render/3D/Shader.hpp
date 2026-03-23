#pragma once
#include <string>
#include <glad/glad.h>

class Shader {
public:
    bool load(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;

    GLint getUniform(const std::string& name) const;

private:
    GLuint _program = 0;

    std::string loadFile(const std::string& path);
    GLuint compile(GLenum type, const std::string& source);
};