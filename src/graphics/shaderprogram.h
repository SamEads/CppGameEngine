#pragma once
#include <string>

class ShaderProgram
{
protected:
	unsigned int m_id;
	int* m_locs;

	void unload() const;

public:
	ShaderProgram();
	ShaderProgram(const std::string& vert, const std::string& frag);
	
	void load(const std::string& vert, const std::string& frag);

	ShaderProgram(const ShaderProgram& other) = delete;
	ShaderProgram& operator=(const ShaderProgram& other) = delete;

	void bind();
	void unbind();

	~ShaderProgram();
};