#pragma once

#include <string>

class Image
{
private:
    unsigned char* m_data;

    int m_xSize = 0, m_ySize = 0;

    bool m_loaded = false;

public:
    Image(const std::string& path);

    Image(const Image& i);

    Image() = default;

    Image(int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    Image& operator=(const Image& i);

    unsigned char* getData() const;

    int getWidth() const { return m_xSize; }

    int getHeight() const { return m_ySize; }

    bool loaded() const { return m_loaded; }

    void load(const std::string& path);

    void draw(const Image& src, int srcX, int srcY, int scrW, int srcH, int destX, int destY, int destW, int destH);

    void unload();

    ~Image();
};
