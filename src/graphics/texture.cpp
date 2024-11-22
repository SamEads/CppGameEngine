#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "texture.h"
#include "image.h"

Texture::Texture(const std::string& path) : Texture()
{
    load(path);
}

Texture::Texture() :
    m_id(-1),
    m_loaded(false) {}

void Texture::bind(unsigned int textureUnit) const
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::load(const std::string& path)
{
    Image image(path);

    m_loaded = image.loaded();
    if (!m_loaded)
    {
        return;
    }

    // Get texture size from image
    m_xSize = image.getWidth(); m_ySize = image.getHeight();

    std::cout << m_xSize << " " << m_ySize << "\n";

    // Generate the texture ID and store in ID
    glGenTextures(1, &m_id);

    // Texture must be binded to make adjustments like setting data
    glBindTexture(GL_TEXTURE_2D, m_id);

    // Repeat texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Set texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_xSize, m_ySize, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)image.getData());

    // Unbind texture when finished
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::load(const Image& image)
{
    m_loaded = image.loaded();
    if (!m_loaded)
    {
        return;
    }

    // Get texture size from image
    m_xSize = image.getWidth(); m_ySize = image.getHeight();

    std::cout << m_xSize << " " << m_ySize << "\n";

    // Generate the texture ID and store in ID
    glGenTextures(1, &m_id);

    // Texture must be binded to make adjustments like setting data
    glBindTexture(GL_TEXTURE_2D, m_id);

    // Repeat texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Set texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_xSize, m_ySize, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)image.getData());

    // Unbind texture when finished
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    reset();
}

void Texture::reset()
{
    if (m_loaded)
    {
        glDeleteTextures(1, &m_id);
        m_loaded = false;
    }
}
