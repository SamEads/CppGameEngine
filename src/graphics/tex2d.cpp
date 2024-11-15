#include "tex2d.h"
#include <raylib.h>
#include <rlgl.h>
#include <unordered_map>
#include "../memory/pointers.h"

void Tex2D::unload()
{
	if (m_id != 0)
	{
		rlUnloadTexture(m_id);
		m_id = 0;
	}
}

Tex2D::Tex2D() : m_id(0), m_size{ 0, 0 }, m_format(0) {}

Tex2D::Tex2D(const std::string& path)
{
	load(path);
}

void Tex2D::set(int id, Vec2<int> size, int format)
{
	unload();

	m_id = id;
	m_size = size;
	m_format = format;
}

Tex2D::~Tex2D()
{
	unload();
}

void Tex2D::load(const std::string& path)
{
	std::cout << "[INFO] Loading texture: \"" << path << "\"\n";
	
	unload();

	Image i = LoadImage(path.c_str());

	m_id = rlLoadTexture(i.data, i.width, i.height, i.format, i.mipmaps);
	m_size = { i.width, i.height };
	m_format = i.format;

	UnloadImage(i);
}

void Tex2D::reset()
{
	unload();
}
