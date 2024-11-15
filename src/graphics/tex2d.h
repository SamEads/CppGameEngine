#pragma once
#include "../utility/vec.h"

struct Img2D
{
    void* data;
    Vec2<int> size;
    int format;
};

class Tex2D
{
private:
    unsigned int m_id = 0;
    Vec2<int> m_size = { 0, 0 };
    int m_format;

    void unload();

public:
    Tex2D();
    Tex2D(const std::string& path);

    void load(const std::string& path);
    void reset();
    
    Tex2D(const Tex2D& other) = delete;
    Tex2D& operator=(const Tex2D& other) = delete;

    void set(int id, Vec2<int> size, int format);
    
    const unsigned int getId() const { return m_id; }
    const Vec2<int> getSize() const { return m_size; }
    const int getFormat() const { return m_format; }

    ~Tex2D();
};