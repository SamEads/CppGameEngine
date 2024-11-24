#include "image.h"
#include "../utility/random.h"
#include <glad/glad.h>
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <iostream>

void Image::unload()
{
    if (m_loaded)
    {
        free(m_data);
        m_loaded = false;
    }
}

void Image::save(const FileSystem::Path& path)
{
    if (m_loaded)
    {
        stbi_write_png(path.string().c_str(), m_xSize, m_ySize, m_comp, m_data, m_xSize * 4);
    }
}

Image::Image(const std::string& path)
{
    load(path);
}

Image::Image(const Image& i)
    : m_data(nullptr), m_xSize(i.m_xSize), m_ySize(i.m_ySize), m_loaded(i.m_loaded)
{
    if (i.m_loaded && i.m_data)
    {
        m_data = (unsigned char*)malloc(sizeof(unsigned char) * size_t(m_xSize * m_ySize * 4));
        if (m_data)
        {
            std::memcpy(m_data, i.m_data, sizeof(unsigned char) * size_t(m_xSize * m_ySize * 4));
        }
    }
}

Image& Image::operator=(const Image& i)
{
    if (this == &i)
    {
        return *this;
    }

    unload();

    m_xSize = i.m_xSize;
    m_ySize = i.m_ySize;
    m_loaded = i.m_loaded;

    if (i.m_loaded && i.m_data)
    {
        m_data = (unsigned char*)malloc(sizeof(unsigned char) * size_t(m_xSize * m_ySize * 4));
        if (m_data)
        {
            std::memcpy(m_data, i.m_data, sizeof(unsigned char) * size_t(m_xSize * m_ySize * 4));
        }
    }

    return *this;
}

unsigned char* Image::getData() const
{
    return m_data;
}

void Image::load(const std::string& path)
{
    if (m_loaded)
    {
        return;
    }

    m_data = stbi_load(path.c_str(), &m_xSize, &m_ySize, &m_comp, 0);

    // Set whether or not the image was loaded by checking validity of stb_load.
    // If stb_load returns NULL, the image was not properly loaded.
    m_loaded = (m_data != nullptr);
}

void Image::draw(const Image& src, int srcX, int srcY, int srcW, int srcH,
    int destX, int destY, int destW, int destH, int padding)
{
    if (!m_loaded || !src.m_loaded) return;
    if (srcW <= 0 || srcH <= 0 || destW <= 0 || destH <= 0) return;

    // Original bounds checking
    if (srcX < 0) {
        srcW += srcX;
        srcX = 0;
    }
    if (srcY < 0) {
        srcH += srcY;
        srcY = 0;
    }
    if (srcX + srcW > src.m_xSize) srcW = src.m_xSize - srcX;
    if (srcY + srcH > src.m_ySize) srcH = src.m_ySize - srcY;

    if (destX < 0) {
        destW += destX;
        destX = 0;
    }
    if (destY < 0) {
        destH += destY;
        destY = 0;
    }
    if (destX + destW > m_xSize) destW = m_xSize - destX;
    if (destY + destH > m_ySize) destH = m_ySize - destY;

    if (srcW <= 0 || srcH <= 0 || destW <= 0 || destH <= 0) return;

    float scaleX = float(srcW) / float(destW);
    float scaleY = float(srcH) / float(destH);

    for (int y = -padding; y < destH + padding; y++) {
        for (int x = -padding; x < destW + padding; x++) {
            // Calculate destination position including padding
            int destPosX = destX + x;
            int destPosY = destY + y;

            // Skip if outside destination bounds
            if (destPosX < 0 || destPosX >= m_xSize ||
                destPosY < 0 || destPosY >= m_ySize) continue;

            // Calculate source position
            float srcFloatX = x * scaleX;
            float srcFloatY = y * scaleY;
            int sourceX = srcX + int(srcFloatX);
            int sourceY = srcY + int(srcFloatY);

            // Clamp source coordinates to edges for padding
            sourceX = std::max(srcX, std::min(sourceX, srcX + srcW - 1));
            sourceY = std::max(srcY, std::min(sourceY, srcY + srcH - 1));

            int srcIndex = (sourceY * src.m_xSize + sourceX) * 4;
            int destIndex = (destPosY * m_xSize + destPosX) * 4;

            m_data[destIndex + 0] = src.m_data[srcIndex + 0];
            m_data[destIndex + 1] = src.m_data[srcIndex + 1];
            m_data[destIndex + 2] = src.m_data[srcIndex + 2];
            m_data[destIndex + 3] = src.m_data[srcIndex + 3];
        }
    }
}

Image::Image(int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    m_xSize = w;
    m_ySize = h;

    m_data = (unsigned char*)malloc(sizeof(unsigned char) * size_t(m_xSize * m_ySize * 4));

    for (int i = 0; i < m_xSize * m_ySize; ++i)
    {
        m_data[i * 4 + 0] = r;
        m_data[i * 4 + 1] = g;
        m_data[i * 4 + 2] = b;
        m_data[i * 4 + 3] = a;
    }

    m_loaded = true;
}

Image::Image(Texture* t)
{
    m_xSize = t->getWidth();
    m_ySize = t->getHeight();

    t->bind(0);
    m_data = (unsigned char*)malloc(sizeof(unsigned char) * size_t(m_xSize * m_ySize * 4));
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data);
    t->unbind();

    m_loaded = true;
}

Image::~Image()
{
    unload();
}