#pragma once
#include <string>
#include "image.h"

struct Color
{
    unsigned char r, g, b, a;
};

class Texture
{
private:
    unsigned int m_id;

    int m_xSize = 0, m_ySize = 0;

    bool m_loaded;

public:
    Texture(const std::string& path);

    Texture(const Texture& i) = delete;

    Texture();

    Texture& operator=(const Texture& i) = delete;

    int getWidth() const { return m_xSize; }

    int getHeight() const { return m_ySize; }

    int getId() const { return m_id; }

    void bind(unsigned int textureUnit) const;

    void unbind() const;

    void load(const std::string& path);

    void load(const Image& image);

    ~Texture();

    void reset();
};