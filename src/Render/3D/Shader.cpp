#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

std::string Shader::loadFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cout << "Shader file open error: " << path << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compile(GLenum type, const std::string& source)
{
    if (source.empty())
    {
        std::cout << "Shader source is empty.\n";
        return 0;
    }

    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, log);
        std::cout << "Shader error:\n" << log << "\n";
    }

    return shader;
}

bool Shader::load(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vs = loadFile(vertexPath);
    std::string fs = loadFile(fragmentPath);

    if (vs.empty() || fs.empty())
    {
        return false;
    }

    GLuint vertex = compile(GL_VERTEX_SHADER, vs);
    GLuint fragment = compile(GL_FRAGMENT_SHADER, fs);

    if (vertex == 0 || fragment == 0)
    {
        return false;
    }

    _program = glCreateProgram();
    glAttachShader(_program, vertex);
    glAttachShader(_program, fragment);
    glLinkProgram(_program);

    int success;
    glGetProgramiv(_program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char log[1024];
        glGetProgramInfoLog(_program, 1024, nullptr, log);
        std::cout << "Program error:\n" << log << "\n";
        return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return true;
}

void Shader::use() const
{
    glUseProgram(_program);
}

GLint Shader::getUniform(const std::string& name) const
{
    return glGetUniformLocation(_program, name.c_str());
}