#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

void Image::unload()
{
    if (m_loaded)
    {
        free(m_data);
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

    int comp; // Not used

    m_data = stbi_load(path.c_str(), &m_xSize, &m_ySize, &comp, 0);

    std::cout << "Comp: " << comp << "\n";

    // Set whether or not the image was loaded by checking validity of stb_load.
    // If stb_load returns NULL, the image was not properly loaded.
    m_loaded = (m_data != nullptr);
}

void Image::draw(const Image& src, int srcX, int srcY, int scrW, int srcH, int destX, int destY, int destW, int destH)
{

}

Image::Image(int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    m_xSize = w;
    m_ySize = h;

    m_data = (unsigned char*)malloc(sizeof(unsigned char) * size_t(m_xSize * m_ySize * 4));

    for (int i = 0; i < m_xSize * m_ySize * 4; i += 4)
    {
        m_data[i] = r;
        m_data[i + 1] = g;
        m_data[i + 2] = b;
        m_data[i + 3] = a;
    }

    m_loaded = true;
}

Image::~Image()
{
    unload();
}