#include "shaderprogram.h"
#include <raylib.h>

ShaderProgram::ShaderProgram() : m_id(0), m_locs(nullptr) {}

ShaderProgram::ShaderProgram(const std::string& vert, const std::string& frag)
{
	load(vert, frag);
}

void ShaderProgram::load(const std::string& vert, const std::string& frag)
{
	Shader shader = LoadShaderFromMemory(vert.c_str(), frag.c_str());

	m_id = shader.id;
	m_locs = shader.locs;
}

void ShaderProgram::unload() const
{
	if (m_id != 0)
	{
		Shader s { m_id, m_locs };
		UnloadShader(s);
	}
}

void ShaderProgram::bind()
{

}

void ShaderProgram::unbind()
{

}

ShaderProgram::~ShaderProgram()
{
	unload();
}