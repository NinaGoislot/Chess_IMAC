#pragma once
#include <glad/glad.h>
#include <imgui.h>
#include <stb_image/stb_image.h>


// Loads an image file into an OpenGL texture and returns its ImGui texture id.
inline ImTextureID LoadTexture(const char* filename, bool logFailure = true)
{
    int            width    = 0;
    int            height   = 0;
    int            channels = 0;
    unsigned char* data     = stbi_load(filename, &width, &height, &channels, 4);

    if (!data)
    {
        if (logFailure)
        {
            printf("Failed to load texture: %s\n", filename);
        }
        return nullptr;
    }

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );

    stbi_image_free(data);

    return (ImTextureID)(intptr_t)tex;
}
